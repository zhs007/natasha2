#include "tlod.h"
#include "game_tlod.h"

namespace natasha {

void outputReels(StaticCascadingReels3X5* pReels) {
  for (int y = 0; y < pReels->getLength(); ++y) {
    printf("outputReels y - %d\n", y);
    for (int x = 0; x < pReels->getMaxDownNums(); ++x) {
      printf("outputReels x - %d\n", x);
      printSymbolBlock3X5(NULL, pReels->getNode(x, y), TLOD_SYMBOL_MAPPING);
    }
  }
}

::natashapb::CODE TLOD::init() {
  FileNameList lst;

  lst.push_back("./csv/game116e_payout95_0.csv");
  lst.push_back("./csv/game116e_payout95_1.csv");
  lst.push_back("./csv/game116e_payout95_2.csv");
  lst.push_back("./csv/game116e_payout95_3.csv");
  lst.push_back("./csv/game116e_payout95_4.csv");
  lst.push_back("./csv/game116e_payout95_5.csv");
  lst.push_back("./csv/game116e_payout95_6.csv");

  loadStaticCascadingReels3X5(lst, m_reels);
  if (m_reels.isEmpty()) {
    return ::natashapb::INVALID_REELS_CFG;
  }

  // outputReels(&m_reels);

  loadLines3X5("./csv/game116_line.csv", m_lines);
  if (m_lines.isEmpty()) {
    return ::natashapb::INVALID_LINES_CFG;
  }

  loadPaytables3X5("./csv/game116_paytables.csv", m_paytables);
  if (m_paytables.isEmpty()) {
    return ::natashapb::INVALID_PAYTABLES_CFG;
  }

  m_lstBet.push_back(1);

  addGameMod(::natashapb::BASE_GAME,
             new TLODBaseGame(*this, m_reels, m_paytables, m_lines, m_lstBet));
  addGameMod(::natashapb::FREE_GAME,
             new TLODFreeGame(*this, m_reels, m_paytables, m_lines, m_lstBet));

  return ::natashapb::OK;
}

// getMainGameMod - get current main game module
GameMod* TLOD::getMainGameMod(::natashapb::UserGameLogicInfo* pLogicUser,
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

}  // namespace natasha