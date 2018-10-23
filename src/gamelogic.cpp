#include "../include/gamelogic.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace natasha {

GameLogic::GameLogic() {}

GameLogic::~GameLogic() {}

bool GameLogic::init() { return true; }

bool GameLogic::gameCtrl(const ::natashapb::GameCtrl* pGameCtrl,
                         ::natashapb::UserGameLogicInfo* pUser) {
  return true;
}

bool GameLogic::addGameMod(::natashapb::GAMEMODTYPE gmt, GameMod* pMod) {
  assert(gmt != ::natashapb::NULL_MOD);
  assert(m_mapGameMod.find(gmt) != m_mapGameMod.end());

  m_mapGameMod[gmt] = pMod;

  return true;
}

}  // namespace natasha