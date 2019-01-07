#include "../include/utils.h"
#include <fstream>
#include <streambuf>
#include <string>

namespace natasha {

// clearGameResultInfo
void clearGameResultInfo(::natashapb::GameResultInfo& gri) {
  gri.Clear();

  // gri.set_typegameresult(::natashapb::NO_WIN);
  gri.set_mul(1);
}

// clearSpinResult
void clearSpinResult(::natashapb::SpinResult& sr) { sr.Clear(); }

// setGameCtrlID
void setGameCtrlID(::natashapb::GameCtrlID& dest,
                   const ::natashapb::GameCtrlID& parent, CtrlID curCtrlID,
                   ::natashapb::GAMEMODTYPE curGameMod) {
  assert(parent.ctrlid() > 0);
  assert(parent.gamemod() != ::natashapb::NULL_MOD);

  if (parent.baseid() > 0) {
    dest.set_baseid(parent.baseid());
    dest.set_parentid(parent.ctrlid());
  } else {
    dest.set_baseid(parent.ctrlid());
    dest.set_parentid(parent.ctrlid());
  }

  dest.set_ctrlid(curCtrlID);
  dest.set_gamemod(curGameMod);
}

// printSymbolBlock3X5
void printSymbolBlock3X5(const ::natashapb::SymbolBlock3X5* pSB,
                         const char* str) {
  assert(pSB != NULL);
  assert(str != NULL);

  printf("%c %c %c %c %c\n", str[pSB->dat0_0()], str[pSB->dat0_1()], str[pSB->dat0_2()], str[pSB->dat0_3()], str[pSB->dat0_4()]);
  printf("%c %c %c %c %c\n", str[pSB->dat1_0()], str[pSB->dat1_1()], str[pSB->dat1_2()], str[pSB->dat1_3()], str[pSB->dat1_4()]);
  printf("%c %c %c %c %c\n", str[pSB->dat2_0()], str[pSB->dat2_1()], str[pSB->dat2_2()], str[pSB->dat2_3()], str[pSB->dat2_4()]);
}

}  // namespace natasha