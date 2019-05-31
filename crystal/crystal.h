#ifndef __NATASHA_CRYSTAL_H__
#define __NATASHA_CRYSTAL_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "basegame.h"
#include "freegame.h"
#include "game_tlod.h"

namespace natasha {

class Crystal : public GameLogic {
 public:
  Crystal() {}
  virtual ~Crystal() {}

 public:
  virtual ::natashapb::CODE init(const char* cfgpath);

  // getMainGameMod - get current main game module
  virtual GameMod* getMainGameMod(UserInfo* pLogicUser, bool isComeInGame);

 protected:
  StaticCascadingReels3X5 m_reels;
  Paytables3X5 m_paytables;
  Lines3X5 m_lines;
  BetList m_lstBet;
};

}  // namespace natasha

#endif  // __NATASHA_CRYSTAL_H__