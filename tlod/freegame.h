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
  TLODFreeGame(StaticCascadingReels3X5& reels, Paytables3X5& paytables,
               Lines3X5& lines, BetList& lstBet)
      : m_reels(reels),
        m_paytables(paytables),
        m_lines(lines),
        m_lstBet(lstBet) {}
  virtual ~TLODFreeGame() {}

 public:
  virtual bool init() { return true; }

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

  virtual bool randomReels(::natashapb::RandomResult* pRandomResult,
                           const ::natashapb::GameCtrl* pGameCtrl,
                           const ::natashapb::UserGameModInfo* pUser) {
    m_reels.random(pRandomResult, pUser);

    return true;
  }

  virtual bool spin(::natashapb::SpinResult* pSpinResult,
                    const ::natashapb::GameCtrl* pGameCtrl,
                    const ::natashapb::RandomResult* pRandomResult,
                    const ::natashapb::UserGameModInfo* pUser) {
    return true;
  }

  virtual bool onSpinEnd(::natashapb::UserGameModInfo* pUser,
                         const ::natashapb::GameCtrl* pGameCtrl,
                         const ::natashapb::SpinResult* pSpinResult,
                         const ::natashapb::RandomResult* pRandomResult) {
    return true;
  }

 protected:
  StaticCascadingReels3X5& m_reels;
  Paytables3X5& m_paytables;
  Lines3X5& m_lines;
  BetList& m_lstBet;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_FREEGAME_H__