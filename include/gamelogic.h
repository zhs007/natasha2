#ifndef __NATASHA_GAMELOGIC_H__
#define __NATASHA_GAMELOGIC_H__

#include <assert.h>
#include <map>
#include <vector>
#include "../protoc/base.pb.h"
#include "array.h"
#include "gamemod.h"
// #include "symbolblock.h"
#include "utils.h"

namespace natasha {

class GameLogic {
 public:
  typedef std::map< ::natashapb::GAMEMODTYPE, GameMod*> MapGameMod;
  typedef MapGameMod::const_iterator ConstMapGameModIter;

 public:
  GameLogic();
  virtual ~GameLogic();

 public:
  virtual ::natashapb::CODE init();

  virtual ::natashapb::CODE userComeIn(
      ::natashapb::UserGameLogicInfo* pLogicUser);

  virtual ::natashapb::CODE gameCtrl(
      const ::natashapb::GameCtrl* pGameCtrl,
      ::natashapb::UserGameLogicInfo* pLogicUser);

  // getMainGameMod - get current main game module
  virtual GameMod* getMainGameMod(::natashapb::UserGameLogicInfo* pLogicUser,
                                  bool isComeInGame);

 public:
  // addGameMod - init game module
  //   Only for init
  ::natashapb::CODE addGameMod(::natashapb::GAMEMODTYPE gmt, GameMod* pMod);

  // getGameMod - get game module
  GameMod* getGameMod(::natashapb::GAMEMODTYPE gmt);

  // getUserGameModInfo - get user game module info
  ::natashapb::UserGameModInfo* getUserGameModInfo(
      ::natashapb::UserGameLogicInfo* pLogicUser, ::natashapb::GAMEMODTYPE gmt);

  // getConstUserGameModInfo - get user game module info
  const ::natashapb::UserGameModInfo* getConstUserGameModInfo(
      const ::natashapb::UserGameLogicInfo* pLogicUser,
      ::natashapb::GAMEMODTYPE gmt) const;

  // startGameMod - start game module for user
  //   Only for gamectrl
  ::natashapb::CODE startGameMod(::natashapb::GAMEMODTYPE gmt,
                                 const ::natashapb::StartGameMod* pStart,
                                 CtrlID nextCtrlID,
                                 ::natashapb::UserGameLogicInfo* pLogicUser);

 protected:
  MapGameMod m_mapGameMod;
};

}  // namespace natasha

#endif  // __NATASHA_GAMELOGIC_H__