#ifndef __NATASHA_TLOD_FREEGAME_H__
#define __NATASHA_TLOD_FREEGAME_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "game_tlod.h"

namespace natasha {

class TLODFreeGame : public SlotsGameMod {
 public:
  TLODFreeGame(GameLogic& logic, StaticCascadingReels3X5& reels,
               Paytables3X5& paytables, Lines3X5& lines, BetList& lstBet)
      : SlotsGameMod(logic),
        m_reels(reels),
        m_paytables(paytables),
        m_lines(lines),
        m_lstBet(lstBet) {}
  virtual ~TLODFreeGame() {}

 public:
  virtual ::natashapb::CODE init() { return ::natashapb::OK; }

  // start - start cur game module for user
  //    basegame does not need to handle this
  virtual ::natashapb::CODE start(::natashapb::UserGameModInfo* pUser,
                                  const ::natashapb::StartGameMod* pStart,
                                  CtrlID nextCtrlID) {
    assert(pStart->has_freegame());
    assert(pStart->has_parentctrlid());

    if (pStart->freegame().freenums() != TLOD_DEFAULT_FREENUMS) {
      return ::natashapb::INVALID_START_FREEGAME_NUMS;
    }

    if (pStart->freegame().lines() != TLOD_DEFAULT_PAY_LINES) {
      return ::natashapb::INVALID_START_LINES;
    }

    if (pStart->freegame().times() != TLOD_DEFAULT_TIMES) {
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

    pUser->Clear();

    auto freeinfo = pUser->mutable_freeinfo();
    freeinfo->set_curbet(pStart->freegame().bet());
    freeinfo->set_curlines(pStart->freegame().lines());
    freeinfo->set_curtimes(pStart->freegame().times());
    freeinfo->set_curnums(0);
    freeinfo->set_lastnums(pStart->freegame().freenums());
    freeinfo->set_totalwin(0);

    setGameCtrlID(*pUser->mutable_gamectrlid(), pStart->parentctrlid(),
                  nextCtrlID, ::natashapb::FREE_GAME);

    return ::natashapb::OK;
  }

  // isIn - is in current game module
  virtual bool isIn(const ::natashapb::UserGameModInfo* pUser) {
    if (pUser->has_freeinfo()) {
      return pUser->freeinfo().lastnums() > 0;
    }

    return false;
  }

  // reviewGameCtrl - check & fix gamectrl params from client
  virtual ::natashapb::CODE reviewGameCtrl(
      ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUser) {
    assert(pUser->has_cascadinginfo());

    auto spinctrl = pGameCtrl->mutable_spin();
    spinctrl->set_bet(pUser->cascadinginfo().curbet());
    spinctrl->set_lines(TLOD_DEFAULT_PAY_LINES);
    spinctrl->set_times(TLOD_DEFAULT_TIMES);
    spinctrl->set_totalbet(pUser->cascadinginfo().curbet() *
                           TLOD_DEFAULT_PAY_LINES);
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
    return ::natashapb::OK;
  }

  // procSpinResult - count spin result
  virtual ::natashapb::CODE procSpinResult(
      ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::RandomResult* pRandomResult, CtrlID nextCtrlID,
      ::natashapb::UserGameLogicInfo* pLogicUser) {
    return ::natashapb::OK;
  }

  // getGameModType - get GAMEMODTYPE
  virtual ::natashapb::GAMEMODTYPE getGameModType() {
    return ::natashapb::FREE_GAME;
  }

 protected:
  StaticCascadingReels3X5& m_reels;
  Paytables3X5& m_paytables;
  Lines3X5& m_lines;
  BetList& m_lstBet;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_FREEGAME_H__