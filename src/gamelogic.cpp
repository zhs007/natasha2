#include "../include/gamelogic.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace natasha {

GameLogic::GameLogic() : m_pGameConfig(NULL) {}

GameLogic::~GameLogic() {}

::natashapb::CODE GameLogic::init(const char* cfgpath) {
#ifdef NATASHA_COUNTRTP
  onInitRTP();
#endif  // NATASHA_COUNTRTP

  return ::natashapb::OK;
}

::natashapb::CODE GameLogic::gameCtrl(::natashapb::GameCtrl* pGameCtrl,
                                      UserInfo* pUser) {
  assert(pGameCtrl != NULL);
  assert(pUser != NULL);
  assert(pUser->pLogicUser != NULL);

  auto pLogicUser = pUser->pLogicUser;

  auto curmod = this->getMainGameMod(pUser, false);
  assert(curmod != NULL);

  auto curugmi = this->getUserGameModInfo(pUser, curmod->getGameModType());
  assert(curugmi != NULL);

  auto code = curmod->reviewGameCtrl(pGameCtrl, curugmi);
  if (code != ::natashapb::OK) {
    return code;
  }

  code = curmod->onGameCtrl(pGameCtrl, pUser, curugmi);
  if (code != ::natashapb::OK) {
    return code;
  }

  auto nextmod = this->getMainGameMod(pUser, false);
  assert(nextmod != NULL);

  pLogicUser->set_curgamemodtype(curmod->getGameModType());
  pLogicUser->set_nextgamemodtype(nextmod->getGameModType());

  auto nextugmi = this->getUserGameModInfo(pUser, nextmod->getGameModType());
  assert(nextugmi != NULL);

  pLogicUser->set_iscompleted(nextmod->isCompeleted(nextugmi));

  if (m_funcProcGameCtrlResult != NULL) {
    m_funcProcGameCtrlResult(pLogicUser);
  }

#ifdef NATASHA_COUNTRTP
  if (pGameCtrl->has_spin()) {
    // printf("gamectrl.spin.realbet %lld\n", pGameCtrl->spin().realbet());

    if (pGameCtrl->spin().realbet() > 0) {
      onRTPAddBet(curmod->getGameModType(), pGameCtrl->spin().realbet());
    }
  }

  if (curugmi->has_spinresult()) {
    auto spinret = curugmi->spinresult();

    for (auto i = 0; i < curugmi->spinresult().lstgri_size(); ++i) {
      auto curgri = curugmi->spinresult().lstgri(i);
      onRTPAddPayoutGRI(curmod->getGameModType(), spinret, curgri,
                        getUserGameModInfo(pUser, curmod->getGameModType()));
    }

    onRTPAddPayout(curmod->getGameModType(), spinret.realwin());
  }
#endif  // NATASHA_COUNTRTP

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
    UserInfo* pUser) {
  assert(pUser != NULL);
  assert(pUser->pLogicUser != NULL);

  auto it = m_mapGameMod.find(gmt);
  assert(it != m_mapGameMod.end());

#ifdef NATASHA_COUNTRTP
  m_rtp.addInGameModule(gmt);
#endif  // NATASHA_COUNTRTP

  auto gm = this->getUserGameModInfo(pUser, gmt);
  assert(gm != NULL);

  return it->second->start(gm, pStart, pUser);
}

// getMainGameMod - start game module for user
//   Only for gamectrl
GameMod* GameLogic::getMainGameMod(UserInfo* pUser, bool isComeInGame) {
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
    UserInfo* pUser, ::natashapb::GAMEMODTYPE gmt) {
  assert(pUser != NULL);
  assert(pUser->pLogicUser != NULL);

  auto pLogicUser = pUser->pLogicUser;

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
    const UserInfo* pUser, ::natashapb::GAMEMODTYPE gmt) const {
  assert(pUser != NULL);
  assert(pUser->pLogicUser != NULL);

  auto pLogicUser = pUser->pLogicUser;

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

::natashapb::CODE GameLogic::userComeIn(UserInfo* pUser) {
  assert(pUser != NULL);
  assert(pUser->pLogicUser != NULL);

  auto pLogicUser = pUser->pLogicUser;

  for (ConstMapGameModIter it = m_mapGameMod.begin(); it != m_mapGameMod.end();
       ++it) {
    auto pUGMI = getUserGameModInfo(pUser, it->first);
    assert(pUGMI != NULL);
    auto code = it->second->onUserComeIn(pUser, pUGMI);
    if (code != ::natashapb::OK) {
      return code;
    }
  }

  auto curmod = this->getMainGameMod(pUser, false);
  assert(curmod != NULL);

  if (pLogicUser->curgamemodtype() == ::natashapb::NULL_MOD) {
    pLogicUser->set_curgamemodtype(curmod->getGameModType());
  }

  pLogicUser->set_nextgamemodtype(curmod->getGameModType());

  return ::natashapb::OK;
}

// onGameCtrlEnd - onGameCtrlEnd
//               - 处理当前游戏模块状态，诸如isCompleted、curmod、nextmod
::natashapb::CODE GameLogic::onGameCtrlEnd(
    const ::natashapb::GameCtrl* pGameCtrl, UserInfo* pUser, GameMod* curmod,
    ::natashapb::UserGameModInfo* curugmi) {
  assert(pUser != NULL);
  assert(curmod != NULL);
  assert(curugmi != NULL);

  return curmod->onGameCtrlEnd(pGameCtrl, pUser, curugmi);
}

}  // namespace natasha