#include "../include/gamelogic.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace natasha {

GameLogic::GameLogic() {}

GameLogic::~GameLogic() {}

::natashapb::CODE GameLogic::init() { return ::natashapb::OK; }

::natashapb::CODE GameLogic::gameCtrl(
    ::natashapb::GameCtrl* pGameCtrl,
    ::natashapb::UserGameLogicInfo* pLogicUser) {
  assert(pGameCtrl != NULL);
  assert(pLogicUser != NULL);

  auto curmod = this->getMainGameMod(pLogicUser, false);
  assert(curmod != NULL);

  auto curugmi = this->getUserGameModInfo(pLogicUser, curmod->getGameModType());
  assert(curugmi != NULL);

  auto code = curmod->reviewGameCtrl(pGameCtrl, curugmi);
  if (code != ::natashapb::OK) {
    return code;
  }

  code = curmod->onGameCtrl(pGameCtrl, pLogicUser, curugmi);
  if (code != ::natashapb::OK) {
    return code;
  }

  auto nextmod = this->getMainGameMod(pLogicUser, false);
  assert(nextmod != NULL);

  pLogicUser->set_curgamemodtype(curmod->getGameModType());
  pLogicUser->set_nextgamemodtype(nextmod->getGameModType());

  auto nextugmi =
      this->getUserGameModInfo(pLogicUser, nextmod->getGameModType());
  assert(nextugmi != NULL);

  pLogicUser->set_iscompleted(nextmod->isCompeleted(nextugmi));

  if (m_funcProcGameCtrlResult != NULL) {
    m_funcProcGameCtrlResult(pLogicUser);
  }

  // code = onGameCtrlEnd(pGameCtrl, pLogicUser, curmod, curugmi);
  // if (code != ::natashapb::OK) {
  //   return code;
  // }

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
    ::natashapb::UserGameLogicInfo* pLogicUser) {
  auto it = m_mapGameMod.find(gmt);
  assert(it != m_mapGameMod.end());

  return it->second->start(pLogicUser->mutable_freegame(), pStart);
}

// getMainGameMod - start game module for user
//   Only for gamectrl
GameMod* GameLogic::getMainGameMod(::natashapb::UserGameLogicInfo* pLogicUser,
                                   bool isComeInGame) {
  return NULL;
}

// getGameMod - get game module
GameMod* GameLogic::getGameMod(::natashapb::GAMEMODTYPE gmt) {
  auto it = m_mapGameMod.find(gmt);
  if (it != m_mapGameMod.end()) {
    return it->second;
  }

  return NULL;
}

// getUserGameModInfo - get user game module info
::natashapb::UserGameModInfo* GameLogic::getUserGameModInfo(
    ::natashapb::UserGameLogicInfo* pLogicUser, ::natashapb::GAMEMODTYPE gmt) {
  assert(pLogicUser != NULL);

  switch (gmt) {
    case ::natashapb::BASE_GAME:
      return pLogicUser->mutable_basegame();
    case ::natashapb::FREE_GAME:
      return pLogicUser->mutable_freegame();
    case ::natashapb::BONUS_GAME:
      return pLogicUser->mutable_bonus();
    case ::natashapb::JACKPOT_GAME:
      return pLogicUser->mutable_jackpot();
    case ::natashapb::COMMON_JACKPOT_GAME:
      return pLogicUser->mutable_commonjackpot();
    case ::natashapb::NULL_MOD:
      return NULL;
    default:
      return NULL;
  }

  return NULL;
}

// getConstUserGameModInfo - get user game module info
const ::natashapb::UserGameModInfo* GameLogic::getConstUserGameModInfo(
    const ::natashapb::UserGameLogicInfo* pLogicUser,
    ::natashapb::GAMEMODTYPE gmt) const {
  assert(pLogicUser != NULL);

  switch (gmt) {
    case ::natashapb::BASE_GAME:
      return &(pLogicUser->basegame());
    case ::natashapb::FREE_GAME:
      return &(pLogicUser->freegame());
    case ::natashapb::BONUS_GAME:
      return &(pLogicUser->bonus());
    case ::natashapb::JACKPOT_GAME:
      return &(pLogicUser->jackpot());
    case ::natashapb::COMMON_JACKPOT_GAME:
      return &(pLogicUser->commonjackpot());
    case ::natashapb::NULL_MOD:
      return NULL;
    default:
      return NULL;
  }

  return NULL;
}

::natashapb::CODE GameLogic::userComeIn(
    ::natashapb::UserGameLogicInfo* pLogicUser) {
  assert(pLogicUser != NULL);

  for (ConstMapGameModIter it = m_mapGameMod.begin(); it != m_mapGameMod.end();
       ++it) {
    auto pUGMI = getUserGameModInfo(pLogicUser, it->first);
    assert(pUGMI != NULL);
    auto code = it->second->onUserComeIn(pUGMI);
    if (code != ::natashapb::OK) {
      return code;
    }
  }

  auto curmod = this->getMainGameMod(pLogicUser, false);
  assert(curmod != NULL);

  pLogicUser->set_nextgamemodtype(curmod->getGameModType());

  return ::natashapb::OK;
}

// onGameCtrlEnd - onGameCtrlEnd
//               - 处理当前游戏模块状态，诸如isCompleted、curmod、nextmod
::natashapb::CODE GameLogic::onGameCtrlEnd(
    const ::natashapb::GameCtrl* pGameCtrl,
    ::natashapb::UserGameLogicInfo* pLogicUser, GameMod* curmod,
    ::natashapb::UserGameModInfo* curugmi) {
  assert(pLogicUser != NULL);
  assert(curmod != NULL);
  assert(curugmi != NULL);

  return curmod->onGameCtrlEnd(pGameCtrl, pLogicUser, curugmi);
}

}  // namespace natasha