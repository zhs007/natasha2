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
  virtual GameMod* getMainGameMod(::natashapb::UserGameLogicInfo* pLogicUser,
                                  bool isComeInGame);
#ifdef NATASHA_COUNTRTP
 public:
  virtual void onInitRTP() {
    addRTPModule(::natashapb::BASE_GAME, MeseumMaxSymbols, MeseumMaxPayoutNums);
  }
#endif  // NATASHA_COUNTRTP
 protected:
  NormalReels3X5 m_reels;
  Paytables3X5 m_paytables;
  // Lines3X5 m_lines;
  BetList m_lstBet;
};  // namespace natasha

}  // namespace natasha

#endif  // __NATASHA_MUSEUM_H__