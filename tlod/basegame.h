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
  TLODBaseGame(StaticCascadingReels3X5& reels, Paytables3X5& paytables,
               Lines3X5& lines, BetList& lstBet)
      : m_reels(reels),
        m_paytables(paytables),
        m_lines(lines),
        m_lstBet(lstBet) {}
  virtual ~TLODBaseGame() {}

 public:
  virtual bool init() { return true; }

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
    ::natashapb::GameResultInfo gri;
    TLODCountScatter(gri, pRandomResult->retstaticcascading3x5().sb3x5(),
                     m_paytables, TLOD_SYMBOL_S, pGameCtrl->spin().bet());

    TLODCountAllLine(*pSpinResult,
                     pRandomResult->retstaticcascading3x5().sb3x5(), m_lines,
                     m_paytables, pGameCtrl->spin().bet());

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

#endif  // __NATASHA_TLOD_BASEGAME_H__