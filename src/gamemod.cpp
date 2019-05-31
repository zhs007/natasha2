#include "../include/gamemod.h"
#include <fstream>
#include <streambuf>
#include <string>
#include "../include/gamelogic.h"
#include "../include/natasha.h"

namespace natasha {

// getUserGameModInfo - get user game module info
::natashapb::UserGameModInfo* GameMod::getUserGameModInfo(UserInfo* pUser) {
  assert(pUser != NULL);

  return m_logic.getUserGameModInfo(pUser, m_gmt);
}

// makeInitScenario - make a initial scenario
//                  - 产生一个初始局面，不中奖的
::natashapb::CODE SlotsGameMod::makeInitScenario(
    ::natashapb::GameCtrl* pGameCtrl, const UserInfo* pUser,
    ::natashapb::UserGameModInfo* pUGMI) {
  assert(pUser != NULL);
  assert(pUGMI != NULL);
  assert(pGameCtrl != NULL);

  //   auto pGameCtrl = new ::natashapb::GameCtrl();

  for (int i = 0; i < MAX_NUMS_MAKEINITIALSCENARIO; ++i) {
    pUGMI->clear_randomresult();
    auto code = this->randomReels(pUGMI->mutable_randomresult(), pGameCtrl,
                                  pUGMI, pUser);
    if (code != ::natashapb::OK) {
      return code;
    }

    // pUGMI->clear_spinresult();
    code = this->countSpinResult(pUGMI->mutable_spinresult(), pGameCtrl,
                                 pUGMI->mutable_randomresult(), pUGMI, pUser);
    if (code != ::natashapb::OK) {
      return code;
    }

    if (pUGMI->spinresult().lstgri_size() == 0) {
#ifdef NATASHA_DEBUG
      printf("makeInitScenario OK (%d)\n", i);
#endif  // NATASHA_DEBUG

      return ::natashapb::OK;
    }
  }

  return ::natashapb::ERR_MAKE_INITIAL_SCENARIO;
}

// clearRespinHistory
::natashapb::CODE SlotsGameMod::clearRespinHistory(
    ::natashapb::UserGameModInfo* pUser) {
  assert(pUser != NULL);

  pUser->clear_lsthistory();

  return ::natashapb::OK;
}

// addRespinHistory
::natashapb::CODE SlotsGameMod::addRespinHistory(
    ::natashapb::UserGameModInfo* pUser, int64_t realWin, int64_t win,
    int64_t mul, bool isSpecial) {
  assert(pUser != NULL);

  auto h = pUser->mutable_lsthistory()->add_lst();
  assert(h != NULL);

  h->set_realwin(realWin);
  h->set_win(win);
  h->set_mul(mul);
  h->set_isspecial(isSpecial);

  return ::natashapb::OK;
}

}  // namespace natasha