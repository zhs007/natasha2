#include "../include/gamelogic.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace natasha {

GameLogic::GameLogic() {}

GameLogic::~GameLogic() {}

bool GameLogic::init() {}

bool GameLogic::gameCtrl(const ::natashapb::UserGameLogicInfo* pUser) {}

bool GameLogic::addGameMod(::natashapb::GAMEMODTYPE gmt, GameMod* pMod) {
  assert(gmt != ::natashapb::NULL_MOD);
  assert(m_mapGameMod.find(gmt) != m_mapGameMod.end());

  m_mapGameMod[gmt] = pMod;
}

}  // namespace natasha