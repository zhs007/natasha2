#include "../include/gamelogic.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace natasha {

GameLogic::GameLogic() {}

GameLogic::~GameLogic() {}

::natashapb::CODE GameLogic::init() { return ::natashapb::OK; }

::natashapb::CODE GameLogic::gameCtrl(
    const ::natashapb::GameCtrl* pGameCtrl,
    ::natashapb::UserGameLogicInfo* pLogicUser) {
  return ::natashapb::OK;
}

// addGameMod - init game module
//   Only for init
::natashapb::CODE GameLogic::addGameMod(::natashapb::GAMEMODTYPE gmt,
                                        GameMod* pMod) {
  assert(gmt != ::natashapb::NULL_MOD);
  assert(m_mapGameMod.find(gmt) == m_mapGameMod.end());

  m_mapGameMod[gmt] = pMod;

  return ::natashapb::OK;
}

// startGameMod - start game module for user
//   Only for gamectrl
::natashapb::CODE GameLogic::startGameMod(
    ::natashapb::GAMEMODTYPE gmt, const ::natashapb::StartGameMod* pStart,
    CtrlID nextCtrlID, ::natashapb::UserGameLogicInfo* pLogicUser) {
  auto it = m_mapGameMod.find(gmt);
  assert(it != m_mapGameMod.end());

  return it->second->start(pLogicUser->mutable_freegame(), pStart, nextCtrlID);
}

}  // namespace natasha