#ifndef __NATASHA_TLOD_BASEGAME_H__
#define __NATASHA_TLOD_BASEGAME_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "game_tlod.h"

namespace natasha {

class TLODBaseGame : public SlotsGameMod {
 public:
  TLODBaseGame(GameLogic& logic, StaticCascadingReels3X5& reels,
               Paytables3X5& paytables, Lines3X5& lines, BetList& lstBet)
      : SlotsGameMod(logic),
        m_reels(reels),
        m_paytables(paytables),
        m_lines(lines),
        m_lstBet(lstBet) {}
  virtual ~TLODBaseGame() {}

 public:
  virtual ::natashapb::CODE init() { return ::natashapb::OK; }

  // start - start cur game module for user
  //    basegame does not need to handle this
  virtual ::natashapb::CODE start(::natashapb::UserGameModInfo* pUser,
                                  const ::natashapb::StartGameMod* pStart,
                                  CtrlID nextCtrlID) {
    return ::natashapb::INVALID_START_GAMEMOD_TO_START;
  }

  // isIn - is in current game module
  virtual bool isIn(const ::natashapb::UserGameModInfo* pUser) { return true; }

  // reviewGameCtrl - check & fix gamectrl params from client
  virtual ::natashapb::CODE reviewGameCtrl(
      ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUser) {
    assert(pUser->has_cascadinginfo());

    auto spinctrl = pGameCtrl->mutable_spin();

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
      const ::natashapb::UserGameModInfo* pUser) {
    pSpinResult->Clear();

    // First check free
    ::natashapb::GameResultInfo gri;
    TLODCountScatter(gri, pRandomResult->retstaticcascading3x5().sb3x5(),
                     m_paytables, TLOD_SYMBOL_S, pGameCtrl->spin().bet());
    if (gri.typegameresult() == ::natashapb::SCATTER_LEFT) {
      auto pCurGRI = pSpinResult->add_lstgri();
      pCurGRI->CopyFrom(gri);

      if (pUser->cascadinginfo().freestate() == ::natashapb::NO_FREEGAME) {
        pCurGRI->set_typegameresult(::natashapb::SCATTEREX_LEFT);
        pCurGRI->set_win(0);
        pCurGRI->set_realwin(0);

        pSpinResult->set_fgnums(TLOD_DEFAULT_FREENUMS);

        return ::natashapb::OK;

      } else if (pUser->cascadinginfo().freestate() ==
                 ::natashapb::END_FREEGAME) {
        pSpinResult->set_win(pSpinResult->win() + gri.win());
        pSpinResult->set_realwin(pSpinResult->realwin() + gri.realwin());
      } else {
        return ::natashapb::INVALID_CASCADING_FREESTATE;
      }
    }

    TLODCountAllLine(*pSpinResult,
                     pRandomResult->retstaticcascading3x5().sb3x5(), m_lines,
                     m_paytables, pGameCtrl->spin().bet());

    return ::natashapb::OK;
  }

  // procSpinResult - count spin result
  virtual ::natashapb::CODE procSpinResult(
      ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::RandomResult* pRandomResult, CtrlID nextCtrlID,
      ::natashapb::UserGameLogicInfo* pLogicUser) {
    // if need start free game
    if (pSpinResult->fgnums() > 0) {
      ::natashapb::StartGameMod sp;
      auto parentctrlid = sp.mutable_parentctrlid();
      parentctrlid->CopyFrom(pUser->gamectrlid());

      m_logic.startGameMod(::natashapb::FREE_GAME, &sp, nextCtrlID, pLogicUser);
    }

    // if respin
    if (pSpinResult->win() > 0) {
    }

    return ::natashapb::OK;
  }

 protected:
  StaticCascadingReels3X5& m_reels;
  Paytables3X5& m_paytables;
  Lines3X5& m_lines;
  BetList& m_lstBet;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_BASEGAME_H__