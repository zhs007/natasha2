#include "museum.h"
#include "game_museum.h"

namespace natasha {

// void outputReels(StaticCascadingReels3X5* pReels) {
//   for (int y = 0; y < pReels->getLength(); ++y) {
//     printf("outputReels y - %d\n", y);
//     for (int x = 0; x < pReels->getMaxDownNums(); ++x) {
//       printf("outputReels x - %d\n", x);
//       printSymbolBlock3X5(NULL, pReels->getNode(x, y),
//       MUSEUM_SYMBOL_MAPPING);
//     }
//   }
// }

::natashapb::CODE Museum::init(const char* cfgpath) {
  FileNameList lst;

  loadNormalReels3X5(pathAppend(cfgpath, "game462_payout97.csv").c_str(),
                     m_reels);
  if (m_reels.isEmpty()) {
    return ::natashapb::INVALID_REELS_CFG;
  }

  // outputReels(&m_reels);

  //   loadLines3X5(pathAppend(cfgpath, "game116_line.csv").c_str(), m_lines);
  //   if (m_lines.isEmpty()) {
  //     return ::natashapb::INVALID_LINES_CFG;
  //   }

  loadPaytables3X5(pathAppend(cfgpath, "game462_paytables.csv").c_str(),
                   m_paytables);
  if (m_paytables.isEmpty()) {
    return ::natashapb::INVALID_PAYTABLES_CFG;
  }

  m_lstBet.push_back(1);

  addGameMod(::natashapb::BASE_GAME,
             new MuseumBaseGame(*this, m_reels, m_paytables, m_lstBet));
  addGameMod(::natashapb::FREE_GAME,
             new MuseumFreeGame(*this, m_reels, m_paytables, m_lstBet));

  return GameLogic::init(cfgpath);
}

// getMainGameMod - get current main game module
GameMod* Museum::getMainGameMod(::natashapb::UserGameLogicInfo* pLogicUser,
                                bool isComeInGame) {
  auto pBG = getGameMod(::natashapb::BASE_GAME);
  assert(pBG != NULL);

  auto pFG = getGameMod(::natashapb::FREE_GAME);
  assert(pFG != NULL);

  auto pUserBG = getConstUserGameModInfo(pLogicUser, ::natashapb::BASE_GAME);
  assert(pUserBG != NULL);

  auto pUserFG = getConstUserGameModInfo(pLogicUser, ::natashapb::FREE_GAME);
  assert(pUserFG != NULL);

  if (pFG->isIn(pUserFG)) {
    // printf("TLOD FG\n");

    return pFG;
  }

  return pBG;
}

// countRTP_museum - count rtp
void countRTP_museum() {
  Museum museum;

  printf("%ld\n", time(NULL));

  auto c = museum.init("./csv");
  if (c != natashapb::OK) {
    printf("init fail(%d)!\n", c);
  }

  auto pUGI = new ::natashapb::UserGameLogicInfo();
  c = museum.userComeIn(pUGI);
  if (c != natashapb::OK) {
    printf("userComeIn fail(%d)!\n", c);
  }

  int64_t totalbet = 0;
  int64_t totalpay = 0;

  auto pGameCtrl = new ::natashapb::GameCtrl();
  int64_t ctrlid = 1;

  for (int i = 0; i <= 1000000; ++ctrlid, ++i) {
    // continue ;

    if (pUGI->nextgamemodtype() == natashapb::BASE_GAME) {
      auto spin = pGameCtrl->mutable_spin();
      spin->set_bet(1);
      spin->set_lines(natasha::MUSEUM_DEFAULT_PAY_LINES);
      spin->set_times(natasha::MUSEUM_DEFAULT_TIMES);
    } else if (pUGI->nextgamemodtype() == natashapb::FREE_GAME) {
      auto freespin = pGameCtrl->mutable_freespin();
      freespin->set_bet(1);
      freespin->set_lines(natasha::MUSEUM_DEFAULT_PAY_LINES);
      freespin->set_times(natasha::MUSEUM_DEFAULT_TIMES);
    }

    pGameCtrl->set_ctrlid(ctrlid);

    c = museum.gameCtrl(pGameCtrl, pUGI);
    if (c != natashapb::OK) {
      printf("gameCtrl fail(%d)!\n", c);
    }

    if (pUGI->iscompleted()) {
      ++i;
    }
  }

  museum.outputRTP();

  delete pGameCtrl;

  printf("%ld\n", time(NULL));

  printf("end!\n");
}

}  // namespace natasha