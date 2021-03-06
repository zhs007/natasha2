#ifndef __NATASHA_TLOD_H__
#define __NATASHA_TLOD_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "basegame.h"
#include "freegame.h"
#include "game_tlod.h"

namespace natasha {

// countRTP_tlod - count rtp
void countRTP_tlod();

// TLOD
class TLOD : public GameLogic {
 public:
  TLOD() {}
  virtual ~TLOD() {}

 public:
  virtual ::natashapb::CODE init(const char* cfgpath);

  // getMainGameMod - get current main game module
  virtual GameMod* getMainGameMod(UserInfo* pUser, bool isComeInGame);
#ifdef NATASHA_COUNTRTP
 public:
  virtual void onInitRTP() {
    addRTPModule(::natashapb::BASE_GAME, TLODMaxPayoutNums, TLODMaxSymbols);
    addRTPModule(::natashapb::FREE_GAME, TLODMaxPayoutNums, TLODMaxSymbols);
  }
#endif  // NATASHA_COUNTRTP

#ifdef NATASHA_RUNINCPP
  void initConfig();
#endif  // NATASHA_RUNINCPP
 protected:
  StaticCascadingReels3X5 m_reels;
  Paytables3X5 m_paytables;
  Lines3X5 m_lines;
  BetList m_lstBet;
};  // namespace natasha

}  // namespace natasha

#endif  // __NATASHA_TLOD_H__