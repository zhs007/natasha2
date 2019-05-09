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
  virtual ::natashapb::CODE onUserComeIn(
      const ::natashapb::UserGameLogicInfo* pLogicUser,
      ::natashapb::UserGameModInfo* pUser) {
    assert(pUser != NULL);

    // 版本号用来区分数据版本
    // 版本号也可以用于判断数据是否已经初始化
    if (pUser->ver() != MUSEUM_BG_UGMI_VER) {
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

  // start - start cur game module for user
  //    basegame does not need to handle this
  virtual ::natashapb::CODE start(::natashapb::UserGameModInfo* pUser,
                                  const ::natashapb::StartGameMod* pStart) {
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
    if (pUser->cascadinginfo().turnnums() > 0) {
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
      const ::natashapb::UserGameModInfo* pUser) {
    randomReels3x5(m_reels, pRandomResult, pUser);

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
                       MUSEUM_SYMBOL_S,
                       pGameCtrl->spin().bet() * MUSEUM_DEFAULT_PAY_LINES);
    if (gri.typegameresult() == ::natashapb::SCATTER_LEFT) {
      auto pCurGRI = pSpinResult->add_lstgri();
      pCurGRI->CopyFrom(gri);
      pSpinResult->set_fgnums(pCfg->fgnums());
    }

    // check all line payout
    MuseumCountWays(*pSpinResult, pSpinResult->symbolblock().sb3x5(),
                    m_paytables, pGameCtrl->spin().bet());

    auto bonuswin =
        procWildBomb(pGameCtrl->spin().bet(), *pCfg, pUser, pSpinResult);

    auto turnnums = pUser->cascadinginfo().turnnums();
    if (turnnums >= pCfg->bgmultipliers_size()) {
      turnnums = pCfg->bgmultipliers_size() - 1;
    }

    pSpinResult->set_awardmul(pCfg->bgbonusprize(turnnums));
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

    if (pSpinResult->fgnums() > 0) {
      auto fgcache = pUser->mutable_fgcache();
      fgcache->set_fgnums(fgcache->fgnums() + pSpinResult->fgnums());
    }

    // if need start free game
    if (pSpinResult->realfgnums() > 0) {
      this->setCurGameCtrlID(pUser, pGameCtrl->ctrlid());

      ::natashapb::StartGameMod sp;

      auto parentctrlid = sp.mutable_parentctrlid();
      parentctrlid->CopyFrom(pUser->gamectrlid());
      // parentctrlid->set_baseid();

      auto fg = sp.mutable_freegame();
      fg->set_bet(pGameCtrl->spin().bet());
      fg->set_lines(MUSEUM_DEFAULT_PAY_LINES);
      fg->set_times(MUSEUM_DEFAULT_TIMES);
      fg->set_freenums(MUSEUM_DEFAULT_FREENUMS);

      auto code = m_logic.startGameMod(::natashapb::FREE_GAME, &sp, pLogicUser);
      if (code != ::natashapb::OK) {
        return code;
      }

      pUser->mutable_cascadinginfo()->set_isend(true);
      this->addRespinHistory(pUser, pSpinResult->realwin(), pSpinResult->win(),
                             pSpinResult->awardmul(), true);

      return ::natashapb::OK;
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
    if (pUser->cascadinginfo().isend()) {
      pUser->mutable_cascadinginfo()->set_turnnums(0);
      pUser->mutable_cascadinginfo()->set_turnwin(0);

      this->clearRespinHistory(pUser);
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

    sb3x5->Clear();

    auto cfg = (const ::natashapb::MuseumRTPConfig*)pCfg;
    if (cfg != NULL) {
      auto spTriggered =
          randWArr(*cfg, pUser->cascadinginfo().turnnums(),
                   pRandomResult->nrrr3x5().symbolblock().sb3x5(), sb3x5);

      pSpinResult->set_specialtriggered(spTriggered);

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
        return false;
      }
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

  int randWArr(const ::natashapb::MuseumRTPConfig& cfg, int turnnums,
               const ::natashapb::SymbolBlock3X5& srcsb3x5,
               ::natashapb::SymbolBlock3X5* sb3x5) {
    int nums = 0;

    if (turnnums >= cfg.bgmysterywild_size()) {
      turnnums = cfg.bgmysterywild_size() - 1;
    }

    auto mwweight = cfg.bgmysterywild(turnnums);

    for (int y = 0; y < MUSEUM_HEIGHT; ++y) {
      setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH, MUSEUM_HEIGHT>(
          sb3x5, 0, y,
          getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                         MUSEUM_HEIGHT>(&srcsb3x5, 0, y));

      for (int x = 1; x < MUSEUM_WIDTH; ++x) {
        auto cs = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                 MUSEUM_HEIGHT>(&srcsb3x5, x, y);
        if (cs != MUSEUM_SYMBOL_S) {
          auto ci = randWeightConfig(mwweight);
          if (ci == 0) {
            setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                           MUSEUM_HEIGHT>(sb3x5, x, y, MUSEUM_SYMBOL_W);

            nums++;

            continue;
          }
        }

        setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                       MUSEUM_HEIGHT>(sb3x5, x, y, cs);
      }
    }

    return nums;
  }

  void bomb(::natashapb::SymbolBlock3X5& tmp,
            const ::natashapb::SymbolBlock3X5& sb3x5, int x, int y,
            ::natashapb::GameResultInfo* pGRI) {
    for (int cy = y - 1; cy <= y + 1; ++cy) {
      if (cy >= 0 && cy < MUSEUM_HEIGHT) {
        for (int cx = x - 1; cx <= x + 1; ++cx) {
          if (cx >= 0 && cx < MUSEUM_WIDTH) {
            auto ctmps =
                getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                               MUSEUM_HEIGHT>(&tmp, cx, cy);
            if (ctmps == 0) {
              if (cx == x && cy == y) {
                auto cp = pGRI->add_lstpos();
                cp->set_x(cx);
                cp->set_y(cy);

                setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                               MUSEUM_HEIGHT>(&tmp, cx, cy, -1);
              } else {
                auto cs =
                    getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                   MUSEUM_HEIGHT>(&sb3x5, cx, cy);

                if (cs != MUSEUM_SYMBOL_W && cs != MUSEUM_SYMBOL_S) {
                  auto cp = pGRI->add_lstpos();
                  cp->set_x(cx);
                  cp->set_y(cy);

                  setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                 MUSEUM_HEIGHT>(&tmp, cx, cy, -1);
                }
              }
            }
          }
        }
      }
    }
  }

  MoneyType procWildBomb(MoneyType bet, const ::natashapb::MuseumRTPConfig& cfg,
                         const ::natashapb::UserGameModInfo* pUser,
                         ::natashapb::SpinResult* pSpinResult) {
    if (pSpinResult->specialtriggered() > 0) {
      ::natashapb::SymbolBlock3X5 tmp;
      auto sb3x5 = pSpinResult->symbolblock().sb3x5();
      auto pGRI = pSpinResult->add_lstgri();

      removeBlock3X5WithGameResult(&tmp, pSpinResult);

      for (int y = 0; y < MUSEUM_HEIGHT; ++y) {
        for (int x = 1; x < MUSEUM_WIDTH; ++x) {
          auto ctmps = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                      MUSEUM_HEIGHT>(&tmp, x, y);
          auto cs = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                   MUSEUM_HEIGHT>(&sb3x5, x, y);
          if (ctmps == 0 && cs == MUSEUM_SYMBOL_W) {
            bomb(tmp, sb3x5, x, y, pGRI);
          }
        }
      }

      if (pGRI->lstpos_size() > 0) {
        auto turnnums = pUser->cascadinginfo().turnnums();

        if (turnnums >= cfg.bgbonusprize_size()) {
          turnnums = cfg.bgbonusprize_size() - 1;
        }

        pGRI->set_mul(cfg.bgbonusprize(turnnums));
        pGRI->set_typegameresult(::natashapb::SPECIAL);
        pGRI->set_win(bet * pGRI->mul() * pGRI->lstpos_size());
        pGRI->set_realwin(pGRI->win());

        return pGRI->realwin();
      } else {
        pSpinResult->mutable_lstgri()->RemoveLast();
      }
    }

    return 0;
  }

 protected:
  NormalReels3X5& m_reels;
  Paytables3X5& m_paytables;
  BetList& m_lstBet;
  ::natashapb::MuseumConfig& m_cfg;
};

}  // namespace natasha

#endif  // __NATASHA_MUSEUM_BASEGAME_H__