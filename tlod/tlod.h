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

class TLOD : public GameLogic {
 public:
  TLOD() {}
  virtual ~TLOD() {}

 public:
  virtual ::natashapb::CODE init();

  // getMainGameMod - get current main game module
  virtual GameMod* getMainGameMod(::natashapb::UserGameLogicInfo* pLogicUser,
                                  bool isComeInGame);

 protected:
  StaticCascadingReels3X5 m_reels;
  Paytables3X5 m_paytables;
  Lines3X5 m_lines;
  BetList m_lstBet;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_H__