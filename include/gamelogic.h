#ifndef __NATASHA_GAMELOGIC_H__
#define __NATASHA_GAMELOGIC_H__

#include <assert.h>
#include <map>
#include <vector>
#include "../protoc/base.pb.h"
#include "array.h"
#include "gamemod.h"
#include "symbolblock.h"
#include "utils.h"

namespace natasha {

class GameLogic {
 public:
  typedef std::map< ::natashapb::GAMEMODTYPE, GameMod*> MapGameMod;

 public:
  GameLogic();
  virtual ~GameLogic();

 public:
  virtual bool init();

  virtual bool gameCtrl(const ::natashapb::UserGameLogicInfo* pUser);

 public:
  bool addGameMod(::natashapb::GAMEMODTYPE gmt, GameMod* pMod);

 protected:
  MapGameMod m_mapGameMod;
};

}  // namespace natasha

#endif  // __NATASHA_GAMELOGIC_H__