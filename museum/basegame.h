#ifndef __NATASHA_MUSEUM_BASEGAME_H__
#define __NATASHA_MUSEUM_BASEGAME_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "game_museum.h"

namespace natasha {

const int32_t MUSEUM_BG_UGMI_VER = 1;

class MuseumBaseGame : public SlotsGameMod {
 public:
  MuseumBaseGame(GameLogic& logic, NormalReels3X5& reels,
                 Paytables3X5& paytables, BetList& lstBet,
                 ::natashapb::MuseumConfig& cfg)
      : SlotsGameMod(logic, ::natashapb::BASE_GAME),
        m_reels(reels),
        m_paytables(paytables),
        m_lstBet(lstBet),
        m_cfg(cfg) {}
  virtual ~MuseumBaseGame() {}

 public:
  virtual ::natashapb::CODE init() { return ::natashapb::OK; }

  // onUserComeIn -
  virtual ::natashapb::CODE onUserComeIn(const UserInfo* pUser,
                                         ::natashapb::UserGameModInfo* pUGMI) {
    assert(pUser != NULL);
    assert(pUGMI != NULL);

    // 版本号用来区分数据版本
    // 版本号也可以用于判断数据是否已经初始化
    if (pUGMI->ver() != MUSEUM_BG_UGMI_VER) {
      auto code = this->clearUGMI(pUGMI);
      if (code != ::natashapb::OK) {
        return code;
      }

      pUGMI->mutable_cascadinginfo()->set_isend(true);

      ::natashapb::GameCtrl gamectrl;
      code = this->makeInitScenario(&gamectrl, pUser, pUGMI);
      if (code != ::natashapb::OK) {
        return code;
      }
    }

    return ::natashapb::OK;
  }

  // start - start cur game module for user
  //    basegame does not need to handle this
  virtual ::natashapb::CODE start(::natashapb::UserGameModInfo* pUGMI,
                                  const ::natashapb::StartGameMod* pStart,
                                  const UserInfo* pUser) {
    return ::natashapb::INVALID_START_GAMEMOD_TO_START;
  }

  // isIn - is in current game module
  virtual bool isIn(const ::natashapb::UserGameModInfo* pUser) { return true; }

  // reviewGameCtrl - check & fix gamectrl params from client
  virtual ::natashapb::CODE reviewGameCtrl(
      ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUser) {
    assert(pUser->has_cascadinginfo());

    if (!pGameCtrl->has_spin()) {
      return ::natashapb::INVALID_GAMECTRL_GAMEMOD;
    }

    auto spinctrl = pGameCtrl->mutable_spin();

    // if respin
    if (!pUser->cascadinginfo().isend()) {
      spinctrl->set_bet(pUser->cascadinginfo().curbet());
      spinctrl->set_lines(MUSEUM_DEFAULT_PAY_LINES);
      spinctrl->set_times(MUSEUM_DEFAULT_TIMES);
      spinctrl->set_totalbet(spinctrl->bet() * MUSEUM_DEFAULT_PAY_LINES);
      spinctrl->set_realbet(0);
    } else {
      // spinctrl->set_bet(pUser->cascadinginfo().curbet());
      spinctrl->set_lines(MUSEUM_DEFAULT_PAY_LINES);
      spinctrl->set_times(MUSEUM_DEFAULT_TIMES);
      spinctrl->set_totalbet(spinctrl->bet() * MUSEUM_DEFAULT_PAY_LINES);
      spinctrl->set_realbet(spinctrl->bet() * MUSEUM_DEFAULT_PAY_LINES);
    }

    // check bet
    auto it = std::find(m_lstBet.begin(), m_lstBet.end(), spinctrl->bet());
    if (it == m_lstBet.end()) {
      return ::natashapb::INVALID_BET;
    }

    return ::natashapb::OK;
  }

  // randomReels - random reels
  virtual ::natashapb::CODE randomReels(
      ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUGMI, const UserInfo* pUser) {
    auto turnnums = pUGMI->cascadinginfo().turnnums();
    auto pCfg = (const ::natashapb::MuseumRTPConfig*)getUserConfig(pUser);
    if (turnnums >= pCfg->bgmysterywild_size()) {
      turnnums = pCfg->bgmysterywild_size() - 1;
    }

    auto mwweight = pCfg->bgmysterywild(turnnums);

    FuncOnFillReels f =
        std::bind(museum_onfill, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, mwweight);

    randomReels3x5(m_reels, pRandomResult, pUGMI, f);

    return ::natashapb::OK;
  }

  // countSpinResult - count spin result
  virtual ::natashapb::CODE countSpinResult(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::UserGameModInfo* pUGMI, const UserInfo* pUser) {
    assert(pSpinResult != NULL);
    assert(pGameCtrl != NULL);
    assert(pRandomResult != NULL);
    assert(pUGMI != NULL);
    assert(pUser != NULL);

    auto pCfg = (const ::natashapb::MuseumRTPConfig*)getUserConfig(pUser);

#ifdef NATASHA_DEBUG
    printRandomResult("countSpinResult", pRandomResult, MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG

    pSpinResult->Clear();

    this->buildSpinResultSymbolBlock(pSpinResult, pUGMI, pGameCtrl,
                                     pRandomResult, pUser, pCfg);

    // First check free
    ::natashapb::GameResultInfo gri;
    MuseumCountScatter(gri, pSpinResult->symbolblock().sb3x5(), m_paytables,
                       MUSEUM_SYMBOL_S, pGameCtrl->spin().totalbet());
    if (gri.typegameresult() == ::natashapb::SCATTER_LEFT) {
      auto pCurGRI = pSpinResult->add_lstgri();
      pCurGRI->CopyFrom(gri);
      pSpinResult->set_fgnums(pCfg->fgnums());

      pSpinResult->set_win(pSpinResult->win() + pCurGRI->win());
    }

    // check all line payout
    MuseumCountWays(*pSpinResult, pSpinResult->symbolblock().sb3x5(),
                    m_paytables, pGameCtrl->spin().bet());

    auto bonuswin = museum_procWildBomb<::natashapb::BASE_GAME>(
        pGameCtrl->spin().bet(), *pCfg, pUGMI, pSpinResult);

    auto turnnums = pUGMI->cascadinginfo().turnnums();
    if (turnnums >= pCfg->bgmultipliers_size()) {
      turnnums = pCfg->bgmultipliers_size() - 1;
    }

    pSpinResult->set_awardmul(pCfg->bgmultipliers(turnnums));
    pSpinResult->set_realwin(pSpinResult->win() * pSpinResult->awardmul() +
                             bonuswin);

    if (pSpinResult->lstgri_size() == 0 &&
        pUGMI->fgcache().fgnums() + pSpinResult->fgnums() > 0) {
      pSpinResult->set_realfgnums(pUGMI->fgcache().fgnums() +
                                  pSpinResult->fgnums());

      // printf("realfgnums %d\n", pSpinResult->realfgnums());
    }

#ifdef NATASHA_DEBUG
    printSpinResult("countSpinResult", pSpinResult, MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG

    return ::natashapb::OK;
  }

  // procSpinResult - proc spin result
  virtual ::natashapb::CODE procSpinResult(
      ::natashapb::UserGameModInfo* pUGMI,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::RandomResult* pRandomResult, UserInfo* pUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pUGMI != NULL);

    if (pSpinResult->fgnums() > 0) {
      auto fgcache = pUGMI->mutable_fgcache();
      fgcache->set_fgnums(fgcache->fgnums() + pSpinResult->fgnums());

      // printf("fgnums %d\n", fgcache->fgnums());
    }

    // if respin
    if (pSpinResult->lstgri_size() > 0) {
      this->setCurGameCtrlID(pUGMI, pGameCtrl->ctrlid());

      pUGMI->mutable_cascadinginfo()->set_turnwin(
          pUGMI->cascadinginfo().turnwin() + pSpinResult->realwin());

      pUGMI->mutable_cascadinginfo()->set_curbet(pGameCtrl->spin().bet());
      pUGMI->mutable_cascadinginfo()->set_turnnums(
          pUGMI->cascadinginfo().turnnums() + 1);
      pUGMI->mutable_cascadinginfo()->set_isend(false);
      // printGameCtrlID("tlod basegame", pUser->gamectrlid());
    } else {
      pUGMI->mutable_cascadinginfo()->set_isend(true);
    }

    this->addRespinHistory(pUGMI, pSpinResult->realwin(), pSpinResult->win(),
                           pSpinResult->awardmul(), false);

    // if need start free game
    if (pSpinResult->realfgnums() > 0) {
      this->setCurGameCtrlID(pUGMI, pGameCtrl->ctrlid());

      ::natashapb::StartGameMod sp;

      auto parentctrlid = sp.mutable_parentctrlid();
      parentctrlid->CopyFrom(pUGMI->gamectrlid());
      // parentctrlid->set_baseid();

      auto fg = sp.mutable_freegame();
      fg->set_bet(pGameCtrl->spin().bet());
      fg->set_lines(MUSEUM_DEFAULT_PAY_LINES);
      fg->set_times(MUSEUM_DEFAULT_TIMES);
      fg->set_freenums(pSpinResult->realfgnums());

      auto code = m_logic.startGameMod(::natashapb::FREE_GAME, &sp, pUser);
      if (code != ::natashapb::OK) {
        return code;
      }

      // printf("start FG\n");
    }

    return ::natashapb::OK;
  }

  // onSpinStart - on spin start
  virtual ::natashapb::CODE onSpinStart(::natashapb::UserGameModInfo* pUGMI,
                                        const ::natashapb::GameCtrl* pGameCtrl,
                                        UserInfo* pUser) {
    if (pUGMI->cascadinginfo().isend()) {
      pUGMI->mutable_cascadinginfo()->set_turnnums(0);
      pUGMI->mutable_cascadinginfo()->set_turnwin(0);

      pUGMI->mutable_fgcache()->set_fgnums(0);

      this->clearRespinHistory(pUGMI);
    }

    return ::natashapb::OK;
  }

  // onSpinEnd - on spin end
  virtual ::natashapb::CODE onSpinEnd(::natashapb::UserGameModInfo* pUGMI,
                                      const ::natashapb::GameCtrl* pGameCtrl,
                                      ::natashapb::SpinResult* pSpinResult,
                                      ::natashapb::RandomResult* pRandomResult,
                                      UserInfo* pUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pUGMI != NULL);

#ifdef NATASHA_SERVER
    pSpinResult->mutable_spin()->CopyFrom(pGameCtrl->spin());
#endif  // NATASHA_SERVER

    if (pSpinResult->lstgri_size() > 0) {
      auto sb = pUGMI->mutable_symbolblock();
      auto sb3x5 = sb->mutable_sb3x5();

      sb3x5->CopyFrom(pSpinResult->symbolblock().sb3x5());
      removeBlock3X5WithGameResult(sb3x5, pSpinResult);
      cascadeBlock3X5(sb3x5);

#ifdef NATASHA_DEBUG
      printSymbolBlock3X5("onSpinEnd", sb3x5, MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG
    } else {
      auto nrrr = pRandomResult->mutable_nrrr3x5();
      // nrrr->set_reelsindex(-1);
    }

    return ::natashapb::OK;
  }

  // buildSpinResultSymbolBlock - build spin result's symbol block
  virtual ::natashapb::CODE buildSpinResultSymbolBlock(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::UserGameModInfo* pUGMI,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult, const UserInfo* pUser,
      const void* pCfg) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pUGMI != NULL);

    auto sb = pSpinResult->mutable_symbolblock();
    auto sb3x5 = sb->mutable_sb3x5();

    auto cfg = (const ::natashapb::MuseumRTPConfig*)pCfg;
    if (cfg != NULL) {
      auto turnnums = pUGMI->cascadinginfo().turnnums();
#ifdef NATASHA_DEBUG
      printf("buildSpinResultSymbolBlock %d\n", turnnums);
      printSymbolBlock3X5("buildSpinResultSymbolBlock first",
                          &(pRandomResult->nrrr3x5().symbolblock().sb3x5()),
                          MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG

      if (turnnums == 0) {
        auto spTriggered = museum_randWArr<::natashapb::BASE_GAME>(
            *cfg, pRandomResult->nrrr3x5().symbolblock().sb3x5(), sb3x5);

        pSpinResult->set_specialtriggered(spTriggered);
      } else {
        pSpinResult->set_specialtriggered(1);
        sb3x5->CopyFrom(pRandomResult->nrrr3x5().symbolblock().sb3x5());
      }

#ifdef NATASHA_DEBUG
      printSymbolBlock3X5("buildSpinResultSymbolBlock", sb3x5,
                          MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG
    }

    return ::natashapb::OK;
  }

  // clearUGMI - clear UserGameModInfo
  //           - 初始化用户游戏模块数据
  virtual ::natashapb::CODE clearUGMI(::natashapb::UserGameModInfo* pUser) {
    clearUGMI_BaseCascadingInfo(*pUser->mutable_cascadinginfo(),
                                MUSEUM_DEFAULT_PAY_LINES, MUSEUM_DEFAULT_TIMES);

    clearUGMI_GameCtrlID(*pUser->mutable_gamectrlid());

    pUser->set_ver(MUSEUM_BG_UGMI_VER);

    return ::natashapb::OK;
  }

  // isCompeleted - isCompeleted
  //              - 游戏特殊状态是否已结束
  virtual bool isCompeleted(::natashapb::UserGameModInfo* pUser) {
    if (pUser->has_spinresult()) {
      if (pUser->spinresult().lstgri_size() > 0) {
        // printf("not isCompeleted\n");

        return false;
      }
    }

    return true;
  }

 public:
  // const ::natashapb::MuseumRTPConfig* getUserConfig(
  //     const ::natashapb::UserGameLogicInfo* pLogicUser) {
  //   auto rtpcfg = m_cfg.rtp().find(pLogicUser->configname());
  //   if (rtpcfg != m_cfg.rtp().end()) {
  //     return &rtpcfg->second;
  //   }

  //   return NULL;
  // }

  // int randWArr(const ::natashapb::MuseumRTPConfig& cfg,
  //              const ::natashapb::SymbolBlock3X5& srcsb3x5,
  //              ::natashapb::SymbolBlock3X5* sb3x5) {
  //   int nums = 0;

  //   auto mwweight = cfg.bgmysterywild(0);

  //   for (int y = 0; y < MUSEUM_HEIGHT; ++y) {
  //     setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //     MUSEUM_HEIGHT>(
  //         sb3x5, 0, y,
  //         getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                        MUSEUM_HEIGHT>(&srcsb3x5, 0, y));

  //     for (int x = 1; x < MUSEUM_WIDTH; ++x) {
  //       auto cs = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                                MUSEUM_HEIGHT>(&srcsb3x5, x, y);
  //       if (cs != MUSEUM_SYMBOL_S) {
  //         auto ci = randWeightConfig(mwweight);
  //         if (ci == 0) {
  //           setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                          MUSEUM_HEIGHT>(sb3x5, x, y, MUSEUM_SYMBOL_W);

  //           nums++;

  //           continue;
  //         }
  //       }

  //       setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                      MUSEUM_HEIGHT>(sb3x5, x, y, cs);
  //     }
  //   }

  //   return nums;
  // }

  // void bomb(::natashapb::SymbolBlock3X5& tmp,
  //           const ::natashapb::SymbolBlock3X5& sb3x5, int x, int y,
  //           ::natashapb::GameResultInfo* pGRI) {
  //   // printf("bomb %d %d\n", x, y);
  //   for (int cy = y - 1; cy <= y + 1; ++cy) {
  //     if (cy >= 0 && cy < MUSEUM_HEIGHT) {
  //       for (int cx = x - 1; cx <= x + 1; ++cx) {
  //         if (cx >= 0 && cx < MUSEUM_WIDTH) {
  //           auto ctmps =
  //               getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                              MUSEUM_HEIGHT>(&tmp, cx, cy);
  //           if (ctmps == 0) {
  //             if (cx == x && cy == y) {
  //               auto cp = pGRI->add_lstpos();
  //               cp->set_x(cx);
  //               cp->set_y(cy);

  //               setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                              MUSEUM_HEIGHT>(&tmp, cx, cy, -1);
  //             } else {
  //               auto cs =
  //                   getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                                  MUSEUM_HEIGHT>(&sb3x5, cx, cy);

  //               if (cs != MUSEUM_SYMBOL_W && cs != MUSEUM_SYMBOL_S) {
  //                 auto cp = pGRI->add_lstpos();
  //                 cp->set_x(cx);
  //                 cp->set_y(cy);

  //                 setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                                MUSEUM_HEIGHT>(&tmp, cx, cy, -1);
  //               }
  //             }
  //           }
  //         }
  //       }
  //     }
  //   }
  // }

  // MoneyType procWildBomb(MoneyType bet, const ::natashapb::MuseumRTPConfig&
  // cfg,
  //                        const ::natashapb::UserGameModInfo* pUser,
  //                        ::natashapb::SpinResult* pSpinResult) {
  //   if (pSpinResult->specialtriggered() > 0) {
  //     ::natashapb::SymbolBlock3X5 tmp;
  //     auto sb3x5 = pSpinResult->symbolblock().sb3x5();
  //     auto pGRI = pSpinResult->add_lstgri();

  //     removeBlock3X5WithGameResult(&tmp, pSpinResult);

  //     for (int y = 0; y < MUSEUM_HEIGHT; ++y) {
  //       for (int x = 1; x < MUSEUM_WIDTH; ++x) {
  //         auto ctmps = getSymbolBlock<::natashapb::SymbolBlock3X5,
  //         MUSEUM_WIDTH,
  //                                     MUSEUM_HEIGHT>(&tmp, x, y);
  //         auto cs = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
  //                                  MUSEUM_HEIGHT>(&sb3x5, x, y);
  //         if (cs == MUSEUM_SYMBOL_W) {
  //           bomb(tmp, sb3x5, x, y, pGRI);
  //         }
  //       }
  //     }

  //     if (pGRI->lstpos_size() > 0) {
  //       auto turnnums = pUser->cascadinginfo().turnnums();

  //       if (turnnums >= cfg.bgbonusprize_size()) {
  //         turnnums = cfg.bgbonusprize_size() - 1;
  //       }

  //       pGRI->set_mul(cfg.bgbonusprize(turnnums));
  //       pGRI->set_typegameresult(::natashapb::SPECIAL);
  //       pGRI->set_win(bet * pGRI->mul() * pGRI->lstpos_size());
  //       pGRI->set_realwin(pGRI->win());

  //       return pGRI->realwin();
  //     } else {
  //       pSpinResult->mutable_lstgri()->RemoveLast();
  //     }
  //   }

  //   return 0;
  // }

 protected:
  NormalReels3X5& m_reels;
  Paytables3X5& m_paytables;
  BetList& m_lstBet;
  ::natashapb::MuseumConfig& m_cfg;
};

}  // namespace natasha

#endif  // __NATASHA_MUSEUM_BASEGAME_H__