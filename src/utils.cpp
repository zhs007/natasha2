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
  assert(pSB->dat0_0() + 1 >= 0 && pSB->dat0_0() + 1 < strlen(strMapping));
  assert(pSB->dat0_1() + 1 >= 0 && pSB->dat0_1() + 1 < strlen(strMapping));
  assert(pSB->dat0_2() + 1 >= 0 && pSB->dat0_2() + 1 < strlen(strMapping));
  assert(pSB->dat0_3() + 1 >= 0 && pSB->dat0_3() + 1 < strlen(strMapping));
  assert(pSB->dat0_4() + 1 >= 0 && pSB->dat0_4() + 1 < strlen(strMapping));
  assert(pSB->dat1_0() + 1 >= 0 && pSB->dat1_0() + 1 < strlen(strMapping));
  assert(pSB->dat1_1() + 1 >= 0 && pSB->dat1_1() + 1 < strlen(strMapping));
  assert(pSB->dat1_2() + 1 >= 0 && pSB->dat1_2() + 1 < strlen(strMapping));
  assert(pSB->dat1_3() + 1 >= 0 && pSB->dat1_3() + 1 < strlen(strMapping));
  assert(pSB->dat1_4() + 1 >= 0 && pSB->dat1_4() + 1 < strlen(strMapping));
  assert(pSB->dat2_0() + 1 >= 0 && pSB->dat2_0() + 1 < strlen(strMapping));
  assert(pSB->dat2_1() + 1 >= 0 && pSB->dat2_1() + 1 < strlen(strMapping));
  assert(pSB->dat2_2() + 1 >= 0 && pSB->dat2_2() + 1 < strlen(strMapping));
  assert(pSB->dat2_3() + 1 >= 0 && pSB->dat2_3() + 1 < strlen(strMapping));
  assert(pSB->dat2_4() + 1 >= 0 && pSB->dat2_4() + 1 < strlen(strMapping));

  if (str != NULL) {
    printf("%s\n", str);
  }

  printf("%c %c %c %c %c\n", strMapping[pSB->dat0_0() + 1],
         strMapping[pSB->dat0_1() + 1], strMapping[pSB->dat0_2() + 1],
         strMapping[pSB->dat0_3() + 1], strMapping[pSB->dat0_4() + 1]);
  printf("%c %c %c %c %c\n", strMapping[pSB->dat1_0() + 1],
         strMapping[pSB->dat1_1() + 1], strMapping[pSB->dat1_2() + 1],
         strMapping[pSB->dat1_3() + 1], strMapping[pSB->dat1_4()] + 1);
  printf("%c %c %c %c %c\n", strMapping[pSB->dat2_0() + 1],
         strMapping[pSB->dat2_1() + 1], strMapping[pSB->dat2_2() + 1],
         strMapping[pSB->dat2_3() + 1], strMapping[pSB->dat2_4() + 1]);
}

// printRandomResult
void printRandomResult(const char* str,
                       const ::natashapb::RandomResult* pRandomResult,
                       const char* strMapping) {
  assert(pRandomResult != NULL);
  assert(strMapping != NULL);

  if (pRandomResult->has_scrr3x5()) {
    auto rsc = pRandomResult->scrr3x5();
    printf("printRandomResult.scrr3x5 %d %d\n", rsc.reelsindex(),
           rsc.downnums());

    if (rsc.has_symbolblock()) {
      auto sb = rsc.symbolblock();
      if (sb.has_sb3x5()) {
        auto sb3x5 = sb.sb3x5();

        printSymbolBlock3X5(str, &sb3x5, strMapping);
      }
    }
  } else if (pRandomResult->has_nrrr3x5()) {
    auto rsc = pRandomResult->nrrr3x5();
    if (rsc.has_symbolblock()) {
      auto sb = rsc.symbolblock();
      if (sb.has_sb3x5()) {
        auto sb3x5 = sb.sb3x5();

        printSymbolBlock3X5(str, &sb3x5, strMapping);
      }
    }
  }
}

// printSpinResult
void printSpinResult(const char* str,
                     const ::natashapb::SpinResult* pSpinResult,
                     const char* strMapping) {
  assert(pSpinResult != NULL);
  assert(strMapping != NULL);

  if (str != NULL) {
    printf("%s\n", str);
  }

  printf("symbol win is %lld, real win is %lld, win nums is %d\n",
         pSpinResult->win(), pSpinResult->realwin(),
         pSpinResult->lstgri_size());

  for (int i = 0; i < pSpinResult->lstgri_size(); ++i) {
    auto gri = pSpinResult->lstgri(i);
    printGameResultInfo(&gri, strMapping);
  }
}

// printGameResultInfo
void printGameResultInfo(const ::natashapb::GameResultInfo* pGameResultInfo,
                         const char* strMapping) {
  assert(pGameResultInfo != NULL);
  assert(strMapping != NULL);

  printf("%d symbol %d payout is %lld(%lld) line is %d\n",
         pGameResultInfo->typegameresult(), pGameResultInfo->symbol(),
         pGameResultInfo->realwin(), pGameResultInfo->mul(),
         pGameResultInfo->lineindex());

  for (int i = 0; i < pGameResultInfo->lstsymbol_size(); ++i) {
    printf("%c ", strMapping[pGameResultInfo->lstsymbol(i) + 1]);
  }

  printf("\n");
}

// clearUGMI_GameCtrlID
void clearUGMI_GameCtrlID(::natashapb::GameCtrlID& gcid) {
  gcid.set_baseid(0);
  gcid.set_parentid(0);
  gcid.set_ctrlid(0);
  gcid.set_gamemod(natashapb::NULL_MOD);
}

// clearUGMI_BaseCascadingInfo
void clearUGMI_BaseCascadingInfo(::natashapb::BaseCascadingInfo& bci, int lines,
                                 int times) {
  bci.set_curbet(-1);
  bci.set_curlines(lines);
  bci.set_curtimes(times);

  bci.set_turnnums(0);
  bci.set_turnwin(0);
  bci.set_freestate(::natashapb::NO_FREEGAME);
}

// printGameCtrlID
void printGameCtrlID(const char* str, const ::natashapb::GameCtrlID& gcid) {
  if (str != NULL) {
    printf("%s\n", str);
  }

  printf("gamectrlid baseid - %lld, parentid - %lld, ctrlid - %lld, gmt - %d\n",
         gcid.baseid(), gcid.parentid(), gcid.ctrlid(), gcid.gamemod());
}

// pathAppend
std::string pathAppend(const std::string& p1, const std::string& p2) {
  char sep = '/';
  std::string tmp = p1;

  if (p1[p1.length()] != sep) {
    tmp += sep;
    return (tmp + p2);
  }

  return (p1 + p2);
}

// getGameModuleName
const char* getGameModuleName(::natashapb::GAMEMODTYPE module) {
  const char* name[] = {
      "null", "basegame", "freegame", "bonus", "jackpot", "commonjackpot",
  };

  return name[(int)module];
}

// printSymbolBlock
void printSymbolBlock(const char* str, const ::natashapb::SymbolBlock* pSB,
                      const char* strMapping) {
  if (pSB->has_sb3x5()) {
    auto sb3x5 = pSB->sb3x5();

    printSymbolBlock3X5(str, &sb3x5, strMapping);
  }
}

// printBaseCascadingInfo
void printBaseCascadingInfo(const char* str,
                            const ::natashapb::BaseCascadingInfo* pBCI,
                            const char* strMapping) {
  if (str != NULL) {
    printf("%s\n", str);
  }

  printf("bet = %lld, line = %d, times = %d, turnnums = %d, turnwin = %lld\n",
         pBCI->curbet(), pBCI->curlines(), pBCI->curtimes(), pBCI->turnnums(),
         pBCI->turnwin());

  printf("freestate = %d, isend = %d\n", pBCI->freestate(),
         pBCI->isend() ? 1 : 0);

  if (pBCI->has_symbolblock()) {
    printSymbolBlock(NULL, &(pBCI->symbolblock()), strMapping);
  }
}

// printUserGameModInfo
void printUserGameModInfo(const char* str,
                          const ::natashapb::UserGameModInfo* pUser,
                          const char* strMapping) {
  if (str != NULL) {
    printf("%s\n", str);
  }

  printf("ver %d\n", pUser->ver());

  if (pUser->has_cascadinginfo()) {
    printBaseCascadingInfo(NULL, &(pUser->cascadinginfo()), strMapping);
  }

  if (pUser->has_spinresult()) {
    printSpinResult(NULL, &(pUser->spinresult()), strMapping);
  }
}

}  // namespace natasha