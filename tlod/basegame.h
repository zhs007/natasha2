#ifndef __NATASHA_TLOD_BASEGAME_H__
#define __NATASHA_TLOD_BASEGAME_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "game_tlod.h"

namespace natasha {

const int32_t TLOD_BG_UGMI_VER = 1;

class TLODBaseGame : public SlotsGameMod {
 public:
  TLODBaseGame(GameLogic& logic, StaticCascadingReels3X5& reels,
               Paytables3X5& paytables, Lines3X5& lines, BetList& lstBet)
      : SlotsGameMod(logic, ::natashapb::BASE_GAME),
        m_reels(reels),
        m_paytables(paytables),
        m_lines(lines),
        m_lstBet(lstBet) {}
  virtual ~TLODBaseGame() {}

 public:
  virtual ::natashapb::CODE init() { return ::natashapb::OK; }

  // onUserComeIn -
  virtual ::natashapb::CODE onUserComeIn(::natashapb::UserGameModInfo* pUser) {
    assert(pUser != NULL);

    // 版本号用来区分数据版本
    // 版本号也可以用于判断数据是否已经初始化
    if (pUser->ver() != TLOD_BG_UGMI_VER) {
      return this->clearUGMI(pUser);
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
      spinctrl->set_lines(TLOD_DEFAULT_PAY_LINES);
      spinctrl->set_times(TLOD_DEFAULT_TIMES);
      spinctrl->set_totalbet(spinctrl->bet() * TLOD_DEFAULT_PAY_LINES);
      spinctrl->set_realbet(0);
    } else {
      // spinctrl->set_bet(pUser->cascadinginfo().curbet());
      spinctrl->set_lines(TLOD_DEFAULT_PAY_LINES);
      spinctrl->set_times(TLOD_DEFAULT_TIMES);
      spinctrl->set_totalbet(spinctrl->bet() * TLOD_DEFAULT_PAY_LINES);
      spinctrl->set_realbet(spinctrl->bet() * TLOD_DEFAULT_PAY_LINES);
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
    m_reels.random(pRandomResult, pUser);

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
    TLODCountScatter(gri, pSpinResult->symbolblock().sb3x5(), m_paytables,
                     TLOD_SYMBOL_S, pGameCtrl->spin().bet());
    if (gri.typegameresult() == ::natashapb::SCATTER_LEFT) {
      auto pCurGRI = pSpinResult->add_lstgri();
      gri.set_win(0);
      gri.set_realwin(0);

      pCurGRI->CopyFrom(gri);

      printSpinResult("countSpinResult", pSpinResult, TLOD_SYMBOL_MAPPING);

      if (pUser->cascadinginfo().freestate() == ::natashapb::NO_FREEGAME) {
        pCurGRI->set_typegameresult(::natashapb::SCATTEREX_LEFT);
        pCurGRI->set_win(0);
        pCurGRI->set_realwin(0);

        pSpinResult->set_infg(true);
        pSpinResult->set_fgnums(TLOD_DEFAULT_FREENUMS);
        pSpinResult->set_realfgnums(TLOD_DEFAULT_FREENUMS);

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
    TLODCountAllLine(*pSpinResult, pSpinResult->symbolblock().sb3x5(), m_lines,
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

      printGameCtrlID("tlod basegame", pUser->gamectrlid());

      ::natashapb::StartGameMod sp;

      auto parentctrlid = sp.mutable_parentctrlid();
      parentctrlid->CopyFrom(pUser->gamectrlid());
      // parentctrlid->set_baseid();

      auto fg = sp.mutable_freegame();
      fg->set_bet(pGameCtrl->spin().bet());
      fg->set_lines(TLOD_DEFAULT_PAY_LINES);
      fg->set_times(TLOD_DEFAULT_TIMES);
      fg->set_freenums(TLOD_DEFAULT_FREENUMS);

      auto code = m_logic.startGameMod(::natashapb::FREE_GAME, &sp, pLogicUser);
      if (code != ::natashapb::OK) {
        return code;
      }

      return ::natashapb::OK;
    }

    // if respin
    if (pSpinResult->win() > 0) {
      this->setCurGameCtrlID(pUser, pGameCtrl->ctrlid());

      // printGameCtrlID("tlod basegame", pUser->gamectrlid());
    }

    return ::natashapb::OK;
  }

  // onSpinStart - on spin start
  virtual ::natashapb::CODE onSpinStart(
      ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      ::natashapb::UserGameLogicInfo* pLogicUser) {
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
                                TLOD_DEFAULT_PAY_LINES, TLOD_DEFAULT_TIMES);

    clearUGMI_GameCtrlID(*pUser->mutable_gamectrlid());

    pUser->set_ver(TLOD_BG_UGMI_VER);

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
  StaticCascadingReels3X5& m_reels;
  Paytables3X5& m_paytables;
  Lines3X5& m_lines;
  BetList& m_lstBet;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_BASEGAME_H__