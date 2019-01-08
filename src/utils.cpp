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
void printSymbolBlock3X5(const char* str,
                         const ::natashapb::SymbolBlock3X5* pSB,
                         const char* strMapping) {
  assert(pSB != NULL);
  assert(strMapping != NULL);

  if (str != NULL) {
    printf("%s\n", str);
  }

  printf("%c %c %c %c %c\n", strMapping[pSB->dat0_0()],
         strMapping[pSB->dat0_1()], strMapping[pSB->dat0_2()],
         strMapping[pSB->dat0_3()], strMapping[pSB->dat0_4()]);
  printf("%c %c %c %c %c\n", strMapping[pSB->dat1_0()],
         strMapping[pSB->dat1_1()], strMapping[pSB->dat1_2()],
         strMapping[pSB->dat1_3()], strMapping[pSB->dat1_4()]);
  printf("%c %c %c %c %c\n", strMapping[pSB->dat2_0()],
         strMapping[pSB->dat2_1()], strMapping[pSB->dat2_2()],
         strMapping[pSB->dat2_3()], strMapping[pSB->dat2_4()]);
}

// printRandomResult
void printRandomResult(const char* str,
                       const ::natashapb::RandomResult* pRandomResult,
                       const char* strMapping) {
  assert(pRandomResult != NULL);
  assert(str != NULL);

  if (pRandomResult->has_scrr3x5()) {
    auto rsc = pRandomResult->scrr3x5();
    if (rsc.has_symbolblock()) {
      auto sb = rsc.symbolblock();
      if (sb.has_sb3x5()) {
        auto sb3x5 = sb.sb3x5();

        printSymbolBlock3X5(str, &sb3x5, strMapping);
      }
    }
  }
}

}  // namespace natasha