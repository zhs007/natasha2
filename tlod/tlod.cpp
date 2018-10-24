#include "tlod.h"
#include "game_tlod.h"

namespace natasha {

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

}  // namespace natasha