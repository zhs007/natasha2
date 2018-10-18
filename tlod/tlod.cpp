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

  return true;
}

bool TLOD::randomReels(::natashapb::RandomResult* pRandomResult,
                       BaseSymbolBlockInt* pSymbolBlock) {
  assert(pRandomResult != NULL);
  assert(pSymbolBlock != NULL);

  SymbolBlock5X3* pSB = dynamic_cast<SymbolBlock5X3*>(pSymbolBlock);

  m_reels.random(pRandomResult, *pSB);

  return true;
}

bool TLOD::countResult(::natashapb::GameResultChunk* pGameResultChunk,
                       ::natashapb::RandomResult* pRandomResult,
                       BaseSymbolBlockT* pSymbolBlock) {
  return true;
}

}  // namespace natasha