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

#ifdef NATASHA_DEBUG
    printSymbolBlock3X5("randomReels",
                        &pRandomResult->nrrr3x5().symbolblock().sb3x5(),
                        MUSEUM_SYMBOL_MAPPING);
#endif  // NATASHA_DEBUG

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

    pSpinResult->Clear();

    this->buildSpinResultSymbolBlock(pSpinResult, pUser, pGameCtrl,
                                     pRandomResult, pLogicUser);

    // printRandomResult("countSpinResult", pRandomResult, TLOD_SYMBOL_MAPPING);

    // printf("start TLODCountScatter");
    // First check free
    ::natashapb::GameResultInfo gri;
    MuseumCountScatter(gri, pSpinResult->symbolblock().sb3x5(), m_paytables,
                       MUSEUM_SYMBOL_S,
                       pGameCtrl->spin().bet() * MUSEUM_DEFAULT_PAY_LINES);
    if (gri.typegameresult() == ::natashapb::SCATTER_LEFT) {
      auto pCurGRI = pSpinResult->add_lstgri();
      gri.set_win(0);
      gri.set_realwin(0);

      pCurGRI->CopyFrom(gri);

      // printSpinResult("countSpinResult", pSpinResult, TLOD_SYMBOL_MAPPING);

      if (pUser->cascadinginfo().freestate() == ::natashapb::NO_FREEGAME) {
        pCurGRI->set_typegameresult(::natashapb::SCATTEREX_LEFT);
        pCurGRI->set_win(0);
        pCurGRI->set_realwin(0);

        pSpinResult->set_infg(true);
        pSpinResult->set_fgnums(MUSEUM_DEFAULT_FREENUMS);
        pSpinResult->set_realfgnums(MUSEUM_DEFAULT_FREENUMS);

        // printSpinResult("countSpinResult", pSpinResult, TLOD_SYMBOL_MAPPING);

        return ::natashapb::OK;

      } else if (pUser->cascadinginfo().freestate() ==
                 ::natashapb::END_FREEGAME) {
        pSpinResult->set_win(pSpinResult->win() + gri.win());
        pSpinResult->set_realwin(pSpinResult->realwin() + gri.realwin());
      } else {
        return ::natashapb::INVALID_CASCADING_FREESTATE;
      }
    }

    // printf("end TLODCountScatter");

    // check all line payout
    MuseumCountWays(*pSpinResult, pSpinResult->symbolblock().sb3x5(),
                    m_paytables, pGameCtrl->spin().bet());

    // printf("end TLODCountAllLine");

    pSpinResult->set_awardmul(pUser->cascadinginfo().turnnums() + 1);
    pSpinResult->set_realwin(pSpinResult->win() * pSpinResult->awardmul());

    // printSpinResult("countSpinResult", pSpinResult, TLOD_SYMBOL_MAPPING);

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
      this->setCurGameCtrlID(pUser, pGameCtrl->ctrlid());

      // printGameCtrlID("tlod basegame", pUser->gamectrlid());

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

      // printSymbolBlock3X5("onSpinEnd", sb3x5, TLOD_SYMBOL_MAPPING);
    } else {
      auto scrr = pRandomResult->mutable_scrr3x5();
      scrr->set_reelsindex(-1);
    }

    return ::natashapb::OK;
  }

  // buildSpinResultSymbolBlock - build spin result's symbol block
  virtual ::natashapb::CODE buildSpinResultSymbolBlock(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::UserGameLogicInfo* pLogicUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pLogicUser != NULL);

    auto sb = pSpinResult->mutable_symbolblock();
    auto sb3x5 = sb->mutable_sb3x5();
    sb3x5->CopyFrom(pRandomResult->scrr3x5().symbolblock().sb3x5());

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

 protected:
  NormalReels3X5& m_reels;
  Paytables3X5& m_paytables;
  BetList& m_lstBet;
  ::natashapb::MuseumConfig& m_cfg;
};

}  // namespace natasha

#endif  // __NATASHA_MUSEUM_BASEGAME_H__