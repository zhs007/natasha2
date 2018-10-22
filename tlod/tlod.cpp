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

  loadStaticCascadingReels5X3(lst, m_reels);
  if (m_reels.isEmpty()) {
    return false;
  }

  loadLines5X3("./csv/game116_line.csv", m_lines);
  if (m_lines.isEmpty()) {
    return false;
  }

  loadPaytables5X3("./csv/game116_paytables.csv", m_paytables);
  if (m_paytables.isEmpty()) {
    return false;
  }

  addGameMod(::natashapb::BASE_GAME,
             new TLODBaseGame(m_reels, m_paytables, m_lines));
  addGameMod(::natashapb::FREE_GAME,
             new TLODFreeGame(m_reels, m_paytables, m_lines));

  return true;
}

}  // namespace natasha