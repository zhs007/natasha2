#include "tlod.h"
#include "game_tlod.h"

namespace natasha {

bool TLOD::init() {
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
    return false;
  }

  loadLines3X5("./csv/game116_line.csv", m_lines);
  if (m_lines.isEmpty()) {
    return false;
  }

  loadPaytables3X5("./csv/game116_paytables.csv", m_paytables);
  if (m_paytables.isEmpty()) {
    return false;
  }

  m_lstBet.push_back(1);

  addGameMod(::natashapb::BASE_GAME,
             new TLODBaseGame(m_reels, m_paytables, m_lines, m_lstBet));
  addGameMod(::natashapb::FREE_GAME,
             new TLODFreeGame(m_reels, m_paytables, m_lines, m_lstBet));

  return true;
}

}  // namespace natasha