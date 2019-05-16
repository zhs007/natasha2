#ifndef __NATASHA_MUSEUM_FREEGAME_H__
#define __NATASHA_MUSEUM_FREEGAME_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "game_museum.h"

namespace natasha {

const int32_t MUSEUM_FG_UGMI_VER = 1;

class MuseumFreeGame : public SlotsGameMod {
 public:
  MuseumFreeGame(GameLogic& logic, NormalReels3X5& reels,
                 Paytables3X5& paytables, BetList& lstBet,
                 ::natashapb::MuseumConfig& cfg)
      : SlotsGameMod(logic, ::natashapb::FREE_GAME),
        m_reels(reels),
        m_paytables(paytables),
        m_lstBet(lstBet),
        m_cfg(cfg) {}
  virtual ~MuseumFreeGame() {}

 public:
  virtual ::natashapb::CODE init() { return ::natashapb::OK; }

  // start - start cur game module for user
  //    basegame does not need to handle this
  virtual ::natashapb::CODE start(::natashapb::UserGameModInfo* pUser,
                                  const ::natashapb::StartGameMod* pStart) {
    assert(pStart->has_freegame());
    assert(pStart->has_parentctrlid());

    if (pStart->freegame().freenums() != MUSEUM_DEFAULT_FREENUMS) {
      return ::natashapb::INVALID_START_FREEGAME_NUMS;
    }

    if (pStart->freegame().lines() != MUSEUM_DEFAULT_PAY_LINES) {
      return ::natashapb::INVALID_START_LINES;
    }

    if (pStart->freegame().times() != MUSEUM_DEFAULT_TIMES) {
      return ::natashapb::INVALID_START_TIMES;
    }

    auto it =
        std::find(m_lstBet.begin(), m_lstBet.end(), pStart->freegame().bet());
    if (it == m_lstBet.end()) {
      return ::natashapb::INVALID_START_BET;
    }

    if (pStart->parentctrlid().ctrlid() <= 0) {
      return ::natashapb::INVALID_PARENTID;
    }

    if (pStart->parentctrlid().gamemod() != ::natashapb::BASE_GAME) {
      return ::natashapb::INVALID_PARENT_GAMEMOD;
    }

    if (this->isIn(pUser)) {
      return ::natashapb::ALREADY_IN_FREEGAME;
    }

    this->clearUGMI(pUser);

    auto freeinfo = pUser->mutable_freeinfo();
    freeinfo->set_curbet(pStart->freegame().bet());
    freeinfo->set_curlines(pStart->freegame().lines());
    freeinfo->set_curtimes(pStart->freegame().times());
    freeinfo->set_curnums(0);
    freeinfo->set_lastnums(pStart->freegame().freenums());
    freeinfo->set_totalwin(0);

    pUser->mutable_cascadinginfo()->set_isend(true);

    // printGameCtrlID("tlod start freegame", pStart->parentctrlid());

    setGameCtrlID(*pUser->mutable_gamectrlid(), pStart->parentctrlid(), 0,
                  ::natashapb::FREE_GAME);

    return ::natashapb::OK;
  }

  // onUserComeIn -
  virtual ::natashapb::CODE onUserComeIn(
      const ::natashapb::UserGameLogicInfo* pLogicUser,
      ::natashapb::UserGameModInfo* pUser) {
    assert(pUser != NULL);

    // 版本号用来区分数据版本
    // 版本号也可以用于判断数据是否已经初始化
    if (pUser->ver() != MUSEUM_FG_UGMI_VER) {
      auto code = this->clearUGMI(pUser);
      if (code != ::natashapb::OK) {
        return code;
      }

      pUser->mutable_cascadinginfo()->set_isend(true);

      auto pGameCtrl = new ::natashapb::GameCtrl();
      code = this->makeInitScenario(pGameCtrl, pLogicUser, pUser);
      if (code != ::natashapb::OK) {
        return code;
      }
    }

    return ::natashapb::OK;
  }

  // isIn - is in current game module
  virtual bool isIn(const ::natashapb::UserGameModInfo* pUser) {
    if (pUser->has_freeinfo()) {
      if (pUser->freeinfo().lastnums() > 0) {
        return true;
      }

      return !pUser->cascadinginfo().isend();
    }

    return false;
  }

  // reviewGameCtrl - check & fix gamectrl params from client
  virtual ::natashapb::CODE reviewGameCtrl(
      ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUser) {
    assert(pUser->has_cascadinginfo());
    assert(pUser->has_freeinfo());

    if (!pGameCtrl->has_freespin()) {
      return ::natashapb::INVALID_GAMECTRL_GAMEMOD;
    }

    auto spinctrl = pGameCtrl->mutable_freespin();
    spinctrl->set_bet(pUser->freeinfo().curbet());
    spinctrl->set_lines(MUSEUM_DEFAULT_PAY_LINES);
    spinctrl->set_times(MUSEUM_DEFAULT_TIMES);
    spinctrl->set_totalbet(pUser->freeinfo().curbet() *
                           MUSEUM_DEFAULT_PAY_LINES);
    spinctrl->set_realbet(0);

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
      const ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::UserGameLogicInfo* pLogicUser) {
    auto turnnums = pUser->cascadinginfo().turnnums();
    auto pCfg = getUserConfig(pLogicUser);
    if (turnnums >= pCfg->bgmysterywild_size()) {
      turnnums = pCfg->bgmysterywild_size() - 1;
    }

    auto mwweight = pCfg->bgmysterywild(turnnums);

    FuncOnFillReels f =
        std::bind(museum_onfill, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3, mwweight);

    randomReels3x5(m_reels, pRandomResult, pUser, f);

    return ::natashapb::OK;
  }

  // countSpinResult - count spin result
  virtual ::natashapb::CODE countSpinResult(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::UserGameLogicInfo* pLogicUser) {
    assert(pSpinResult != NULL);
    assert(pGameCtrl != NULL);
    assert(pRandomResult != NULL);
    assert(pUser != NULL);

    auto pCfg = getUserConfig(pLogicUser);

#ifdef NATASHA_DEBUG
    printRandomResult("countSpinResult", pRandomResult, MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG

    pSpinResult->Clear();

    this->buildSpinResultSymbolBlock(pSpinResult, pUser, pGameCtrl,
                                     pRandomResult, pLogicUser, pCfg);

    // First check free
    ::natashapb::GameResultInfo gri;
    MuseumCountScatter(gri, pSpinResult->symbolblock().sb3x5(), m_paytables,
                       MUSEUM_SYMBOL_S, pGameCtrl->freespin().totalbet());
    if (gri.typegameresult() == ::natashapb::SCATTER_LEFT) {
      auto pCurGRI = pSpinResult->add_lstgri();
      pCurGRI->CopyFrom(gri);
      pSpinResult->set_fgnums(pCfg->fgnums());
    }

    // check all line payout
    MuseumCountWays(*pSpinResult, pSpinResult->symbolblock().sb3x5(),
                    m_paytables, pGameCtrl->spin().bet());

    auto bonuswin = museum_procWildBomb<::natashapb::FREE_GAME>(
        pGameCtrl->spin().bet(), *pCfg, pUser, pSpinResult);

    auto turnnums = pUser->cascadinginfo().turnnums();
    if (turnnums >= pCfg->bgmultipliers_size()) {
      turnnums = pCfg->bgmultipliers_size() - 1;
    }

    pSpinResult->set_awardmul(pCfg->bgmultipliers(turnnums));
    pSpinResult->set_realwin(pSpinResult->win() * pSpinResult->awardmul() +
                             bonuswin);

#ifdef NATASHA_DEBUG
    printSpinResult("countSpinResult", pSpinResult, MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG

    return ::natashapb::OK;
  }

  // procSpinResult - proc spin result
  virtual ::natashapb::CODE procSpinResult(
      ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::RandomResult* pRandomResult,
      ::natashapb::UserGameLogicInfo* pLogicUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pLogicUser != NULL);

    // if need start free game
    if (pSpinResult->realfgnums() > 0) {
      auto fi = pUser->mutable_freeinfo();
      fi->set_lastnums(fi->lastnums() + pSpinResult->realfgnums());
    }

    // if respin
    if (pSpinResult->realwin() > 0) {
      this->setCurGameCtrlID(pUser, pGameCtrl->ctrlid());

      pUser->mutable_cascadinginfo()->set_turnwin(
          pUser->cascadinginfo().turnwin() + pSpinResult->realwin());

      pUser->mutable_cascadinginfo()->set_curbet(pGameCtrl->spin().bet());
      pUser->mutable_cascadinginfo()->set_turnnums(
          pUser->cascadinginfo().turnnums() + 1);
      pUser->mutable_cascadinginfo()->set_isend(false);
      // printGameCtrlID("tlod basegame", pUser->gamectrlid());
    } else {
      pUser->mutable_cascadinginfo()->set_isend(true);
    }

    this->addRespinHistory(pUser, pSpinResult->realwin(), pSpinResult->win(),
                           pSpinResult->awardmul(), false);

    return ::natashapb::OK;
  }

  // onSpinStart - on spin start
  virtual ::natashapb::CODE onSpinStart(
      ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      ::natashapb::UserGameLogicInfo* pLogicUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pLogicUser != NULL);
    assert(pUser->has_freeinfo());
    // assert(pUser->freeinfo().lastnums() > 0);
    assert(pUser->has_cascadinginfo());

    if (pUser->freeinfo().lastnums() == 0) {
      assert(!pUser->cascadinginfo().isend());
    }

    bool isrespin = true;
    if (pUser->cascadinginfo().isend()) {
      pUser->mutable_cascadinginfo()->set_turnnums(0);
      pUser->mutable_cascadinginfo()->set_turnwin(0);

      this->clearRespinHistory(pUser);

      isrespin = false;
    }

    if (!isrespin) {
      auto fi = pUser->mutable_freeinfo();
      fi->set_lastnums(fi->lastnums() - 1);
      fi->set_curnums(fi->curnums() + 1);
    }

    return ::natashapb::OK;
  }

  // onSpinEnd - on spin end
  virtual ::natashapb::CODE onSpinEnd(
      ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      ::natashapb::SpinResult* pSpinResult,
      ::natashapb::RandomResult* pRandomResult,
      ::natashapb::UserGameLogicInfo* pLogicUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pLogicUser != NULL);

#ifdef NATASHA_SERVER
    pSpinResult->mutable_spin()->CopyFrom(pGameCtrl->spin());
#endif  // NATASHA_SERVER

    if (pSpinResult->lstgri_size() > 0) {
      auto sb = pUser->mutable_symbolblock();
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
      const ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::UserGameLogicInfo* pLogicUser, const void* pCfg) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pLogicUser != NULL);

    auto sb = pSpinResult->mutable_symbolblock();
    auto sb3x5 = sb->mutable_sb3x5();

    auto cfg = (const ::natashapb::MuseumRTPConfig*)pCfg;
    if (cfg != NULL) {
      auto turnnums = pUser->cascadinginfo().turnnums();
#ifdef NATASHA_DEBUG
      printf("buildSpinResultSymbolBlock %d\n", turnnums);
      printSymbolBlock3X5("buildSpinResultSymbolBlock first",
                          &(pRandomResult->nrrr3x5().symbolblock().sb3x5()),
                          MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG

      if (turnnums == 0) {
        auto spTriggered = museum_randWArr<::natashapb::FREE_GAME>(
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

    pUser->set_ver(MUSEUM_FG_UGMI_VER);

    return ::natashapb::OK;
  }

  // isCompeleted - isCompeleted
  //              - 游戏特殊状态是否已结束
  virtual bool isCompeleted(::natashapb::UserGameModInfo* pUser) {
    if (isIn(pUser)) {
      return false;
    }

    return true;
  }

 public:
  const ::natashapb::MuseumRTPConfig* getUserConfig(
      const ::natashapb::UserGameLogicInfo* pLogicUser) {
    auto rtpcfg = m_cfg.rtp().find(pLogicUser->configname());
    if (rtpcfg != m_cfg.rtp().end()) {
      return &rtpcfg->second;
    }

    return NULL;
  }

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

  //       if (turnnums >= cfg.fgbonusprize_size()) {
  //         turnnums = cfg.fgbonusprize_size() - 1;
  //       }

  //       pGRI->set_mul(cfg.fgbonusprize(turnnums));
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

#endif  // __NATASHA_MUSEUM_FREEGAME_H__