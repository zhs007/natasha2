#include "tlod.h"
#include "game_tlod.h"

namespace natasha {

::natashapb::CODE TLOD::init(const char* cfgpath) {
#ifdef NATASHA_RUNINCPP
  initConfig();
#endif  // NATASHA_RUNINCPP

  if (m_reels.isEmpty()) {
    return ::natashapb::INVALID_REELS_CFG;
  }

  if (m_lines.isEmpty()) {
    return ::natashapb::INVALID_LINES_CFG;
  }

  if (m_paytables.isEmpty()) {
    return ::natashapb::INVALID_PAYTABLES_CFG;
  }

  m_lstBet.push_back(1);

  addGameMod(::natashapb::BASE_GAME,
             new TLODBaseGame(*this, m_reels, m_paytables, m_lines, m_lstBet));
  addGameMod(::natashapb::FREE_GAME,
             new TLODFreeGame(*this, m_reels, m_paytables, m_lines, m_lstBet));

  return GameLogic::init(cfgpath);
}

// getMainGameMod - get current main game module
GameMod* TLOD::getMainGameMod(UserInfo* pUser, bool isComeInGame) {
  auto pBG = getGameMod(::natashapb::BASE_GAME);
  assert(pBG != NULL);

  auto pFG = getGameMod(::natashapb::FREE_GAME);
  assert(pFG != NULL);

  auto pUserBG = getConstUserGameModInfo(pUser, ::natashapb::BASE_GAME);
  assert(pUserBG != NULL);

  auto pUserFG = getConstUserGameModInfo(pUser, ::natashapb::FREE_GAME);
  assert(pUserFG != NULL);

  if (pFG->isIn(pUserFG)) {
    // printf("TLOD FG\n");

    return pFG;
  }

  return pBG;
}

// countRTP_tlod - count rtp
void countRTP_tlod() {
  TLOD tlod;
  auto bt = time(NULL);
  printf("%ld\n", bt);

  auto c = tlod.init("./csv");
  if (c != natashapb::OK) {
    printf("init fail(%d)!\n", c);
  }

  auto pUser = new UserInfo();
  auto pUGI = new ::natashapb::UserGameLogicInfo();
  pUser->pLogicUser = pUGI;

  c = tlod.userComeIn(pUser);
  if (c != natashapb::OK) {
    printf("userComeIn fail(%d)!\n", c);
  }

  int64_t totalbet = 0;
  int64_t totalpay = 0;

  auto pGameCtrlBG = new ::natashapb::GameCtrl();
  auto spin = pGameCtrlBG->mutable_spin();
  spin->set_bet(1);
  spin->set_lines(natasha::TLOD_DEFAULT_PAY_LINES);
  spin->set_times(natasha::TLOD_DEFAULT_TIMES);

  auto pGameCtrlFG = new ::natashapb::GameCtrl();
  auto freespin = pGameCtrlFG->mutable_freespin();
  freespin->set_bet(1);
  freespin->set_lines(natasha::TLOD_DEFAULT_PAY_LINES);
  freespin->set_times(natasha::TLOD_DEFAULT_TIMES);

  int64_t ctrlid = 1;
  bool fg = false;

  for (int i = 0; i <= 1000000; ++ctrlid) {
    // continue ;

    if (pUGI->nextgamemodtype() == natashapb::BASE_GAME) {
      pGameCtrlBG->set_ctrlid(ctrlid);

      c = tlod.gameCtrl(pGameCtrlBG, pUser);
      if (c != natashapb::OK) {
        printf("gameCtrl fail(%d)!\n", c);
      }

    } else if (pUGI->nextgamemodtype() == natashapb::FREE_GAME) {
      fg = true;

      // printf("FG\n");

      pGameCtrlFG->set_ctrlid(ctrlid);

      c = tlod.gameCtrl(pGameCtrlFG, pUser);
      if (c != natashapb::OK) {
        printf("gameCtrl fail(%d)!\n", c);
      }
    }

    if (pUGI->iscompleted()) {
      // printf("totalbet is %d %lld\n", i * 30, tlod.getRTP().rtp.totalbet());

      ++i;

      // if (fg) {
      //   break;
      // }
    }
  }
#ifdef NATASHA_COUNTRTP
  tlod.outputRTP();
#endif  // NATASHA_COUNTRTP

  delete pGameCtrlBG;
  delete pGameCtrlFG;

  auto et = time(NULL);
  printf("%ld(%ld)\n", et, et - bt);

  printf("end!\n");
}

}  // namespace natasha