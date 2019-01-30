#include "../include/gamemod.h"
#include <fstream>
#include <streambuf>
#include <string>
#include "../include/gamelogic.h"
#include "../include/natasha.h"

namespace natasha {

// getUserGameModInfo - get user game module info
::natashapb::UserGameModInfo* GameMod::getUserGameModInfo(
    ::natashapb::UserGameLogicInfo* pLogicUser) {
  assert(pLogicUser != NULL);

  return m_logic.getUserGameModInfo(pLogicUser, m_gmt);
}

// makeInitScenario - make a initial scenario
//                  - 产生一个初始局面，不中奖的
::natashapb::CODE SlotsGameMod::makeInitScenario(
    ::natashapb::GameCtrl* pGameCtrl,
    const ::natashapb::UserGameLogicInfo* pLogicUser,
    ::natashapb::UserGameModInfo* pUGMI) {
  assert(pLogicUser != NULL);
  assert(pUGMI != NULL);
  assert(pGameCtrl != NULL);

  //   auto pGameCtrl = new ::natashapb::GameCtrl();

  for (int i = 0; i < MAX_NUMS_MAKEINITIALSCENARIO; ++i) {
    auto code =
        this->randomReels(pUGMI->mutable_randomresult(), pGameCtrl, pUGMI);
    if (code != ::natashapb::OK) {
      return code;
    }

    code =
        this->countSpinResult(pUGMI->mutable_spinresult(), pGameCtrl,
                              pUGMI->mutable_randomresult(), pUGMI, pLogicUser);
    if (code != ::natashapb::OK) {
      return code;
    }

    if (pUGMI->spinresult().realwin() == 0 && pUGMI->spinresult().fgnums() == 0) {
        return ::natashapb::OK;
    }
  }

  return ::natashapb::ERR_MAKE_INITIAL_SCENARIO;
}

}  // namespace natasha