#ifndef __NATASHA_MUSEUM_H__
#define __NATASHA_MUSEUM_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "basegame.h"
#include "freegame.h"
#include "game_museum.h"

namespace natasha {

// countRTP_museum - count rtp
void countRTP_museum();

// Museum
class Museum : public GameLogic {
 public:
  Museum() {}
  virtual ~Museum() {}

 public:
  virtual ::natashapb::CODE init(const char* cfgpath);

  // getMainGameMod - get current main game module
  virtual GameMod* getMainGameMod(UserInfo* pUser, bool isComeInGame);
#ifdef NATASHA_COUNTRTP
 public:
  virtual void onInitRTP() {
    addRTPModule(::natashapb::BASE_GAME, MeseumMaxPayoutNums, MeseumMaxSymbols);
    addRTPModuleBonus(::natashapb::BASE_GAME, "wildbomb", 6);
    addRTPModule(::natashapb::FREE_GAME, MeseumMaxPayoutNums, MeseumMaxSymbols);
    addRTPModuleBonus(::natashapb::FREE_GAME, "wildbomb", 6);
  }

  virtual void onRTPAddPayoutGRI(::natashapb::GAMEMODTYPE module,
                                 const ::natashapb::SpinResult& spinret,
                                 const ::natashapb::GameResultInfo& gri,
                                 const ::natashapb::UserGameModInfo* pUser) {
    if (gri.typegameresult() == ::natashapb::SPECIAL) {
      auto turnnums = pUser->cascadinginfo().turnnums();
      if (turnnums > 6) {
        turnnums = 6;
      }

      m_rtp.addBonusPayout(module, "wildbomb", turnnums - 1, gri.realwin());
    } else {
      m_rtp.addSymbolPayout(module, gri.symbol(), gri.lstsymbol_size(),
                            gri.realwin() * spinret.awardmul());
    }
  }
#endif  // NATASHA_COUNTRTP

#ifdef NATASHA_RUNINCPP
 public:
  void initConfig();
#endif  // NATASHA_RUNINCPP

 protected:
  NormalReels3X5 m_reels;
  Paytables3X5 m_paytables;
  BetList m_lstBet;
  ::natashapb::MuseumConfig m_cfg;
};  // namespace natasha

}  // namespace natasha

#endif  // __NATASHA_MUSEUM_H__