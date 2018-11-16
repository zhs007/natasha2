#include "../include/symbolblock2.h"

namespace natasha {

SymbolType getSymbolBlock3X5(const ::natashapb::SymbolBlock3X5* pSB, int x,
                             int y) {
  assert(pSB != NULL);
  assert(x >= 0 && x < 5);
  assert(y >= 0 && y < 3);

  if (y == 0) {
    switch (x) {
      case 0:
        return pSB->dat0_0();
      case 1:
        return pSB->dat0_1();
      case 2:
        return pSB->dat0_2();
      case 3:
        return pSB->dat0_3();
      case 4:
        return pSB->dat0_4();
    }
  } else if (y == 1) {
    switch (x) {
      case 0:
        return pSB->dat1_0();
      case 1:
        return pSB->dat1_1();
      case 2:
        return pSB->dat1_2();
      case 3:
        return pSB->dat1_3();
      case 4:
        return pSB->dat1_4();
    }
  }

  switch (x) {
    case 0:
      return pSB->dat2_0();
    case 1:
      return pSB->dat2_1();
    case 2:
      return pSB->dat2_2();
    case 3:
      return pSB->dat2_3();
      //   case 4:
      //     return pSB->dat2_4();
  }

  return pSB->dat2_4();
}

void setSymbolBlock3X5(::natashapb::SymbolBlock3X5* pSB, int x, int y,
                       SymbolType s) {
  assert(pSB != NULL);
  assert(x >= 0 && x < 5);
  assert(y >= 0 && y < 3);

  if (y == 0) {
    switch (x) {
      case 0:
        pSB->set_dat0_0(s);
        return;
      case 1:
        pSB->set_dat0_1(s);
        return;
      case 2:
        pSB->set_dat0_2(s);
        return;
      case 3:
        pSB->set_dat0_3(s);
        return;
      case 4:
        pSB->set_dat0_4(s);
        return;
    }
  } else if (y == 1) {
    switch (x) {
      case 0:
        pSB->set_dat1_0(s);
        return;
      case 1:
        pSB->set_dat1_1(s);
        return;
      case 2:
        pSB->set_dat1_2(s);
        return;
      case 3:
        pSB->set_dat1_3(s);
        return;
      case 4:
        pSB->set_dat1_4(s);
        return;
    }
  }

  switch (x) {
    case 0:
      pSB->set_dat2_0(s);
      return;
    case 1:
      pSB->set_dat2_1(s);
      return;
    case 2:
      pSB->set_dat2_2(s);
      return;
    case 3:
      pSB->set_dat2_3(s);
      return;
    case 4:
      pSB->set_dat2_4(s);
      return;
  }
}

void buildSymbolLine3X5(const ::natashapb::SymbolBlock3X5* pSB,
                        StaticArray<5, SymbolType>& sl,
                        const Lines<5, int>& lines, int indexLine) {
  assert(pSB != NULL);

  typedef Lines<5, int> LinesT;
  typedef typename LinesT::LineInfoT LineInfoT;

  const LineInfoT& li = lines.get(indexLine);

  for (int i = 0; i < 5; ++i) {
    sl.set(i, getSymbolBlock3X5(pSB, i, li.get(i)));
  }
}

// cascadeSymbol3X5 - cascade SymbolBlock
void cascadeBlock3X5(::natashapb::SymbolBlock3X5* pSB) {
  assert(pSB != NULL);

  for (int xx = 0; xx < 5; ++xx) {
    if (getSymbolBlock3X5(pSB, xx, 2) < 0) {
      if (getSymbolBlock3X5(pSB, xx, 1) < 0) {
        if (getSymbolBlock3X5(pSB, xx, 0) >= 0) {
          setSymbolBlock3X5(pSB, xx, 2, getSymbolBlock3X5(pSB, xx, 0));
          setSymbolBlock3X5(pSB, xx, 0, -1);
        }
      } else {
        if (getSymbolBlock3X5(pSB, xx, 0) >= 0) {
          setSymbolBlock3X5(pSB, xx, 2, getSymbolBlock3X5(pSB, xx, 1));
          setSymbolBlock3X5(pSB, xx, 1, getSymbolBlock3X5(pSB, xx, 0));
          setSymbolBlock3X5(pSB, xx, 0, -1);
        } else {
          setSymbolBlock3X5(pSB, xx, 2, getSymbolBlock3X5(pSB, xx, 1));
          setSymbolBlock3X5(pSB, xx, 1, -1);
        }
      }
    } else if (getSymbolBlock3X5(pSB, xx, 1) < 0) {
      if (getSymbolBlock3X5(pSB, xx, 0) >= 0) {
        setSymbolBlock3X5(pSB, xx, 1, getSymbolBlock3X5(pSB, xx, 0));
        setSymbolBlock3X5(pSB, xx, 0, -1);
      }
    }
  }
}

// removeBlock3X5WithGameResult - remove all symbol in gameresult
bool removeBlock3X5WithGameResult(::natashapb::SymbolBlock3X5* pSB,
                                  ::natashapb::SpinResult* pSpinResult) {
  assert(pSB != NULL);
  assert(pSpinResult != NULL);

  bool isremove = false;
  for (int i = 0; i < pSpinResult->lstgri_size(); ++i) {
    const ::natashapb::GameResultInfo& gri = pSpinResult->lstgri(i);
    for (int j = 0; j < gri.lstpos_size(); ++j) {
      const ::natashapb::Position2D& pos = gri.lstpos(j);

      setSymbolBlock3X5(pSB, pos.x(), pos.y(), -1);
      isremove = true;
    }
  }

  return isremove;
}

}  // namespace natasha