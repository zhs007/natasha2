#ifndef __NATASHA_LOGICSCATTER_H__
#define __NATASHA_LOGICSCATTER_H__

#include <assert.h>
#include <map>
#include "array.h"
#include "gameresult.h"
#include "lines.h"
#include "paytables.h"
#include "symbolblock.h"
#include "utils.h"

namespace natasha {

template <typename MoneyType, typename SymbolType, int Width, int Height,
          typename GameCfgT>
bool countScatter_Left(
    GameResultInfo<MoneyType, SymbolType, int, NullType>& gri,
    const SymbolBlock<SymbolType, Width, Height>& arr,
    const Paytables<Width, SymbolType, int, MoneyType>& paytables, SymbolType s,
    MoneyType totalbet) {
  typedef GameResult<MoneyType, SymbolType, int, NullType, NullType>
      GameResultT;
  typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;
  typedef Paytables<Width, SymbolType, int, MoneyType> PaytablesT;
  typedef GameResultInfo<MoneyType, SymbolType, int, NullType> GameResultInfoT;

  //   GameResultInfoT gri;

  gri.clear();
  //   gri.type = GRT_SCATTER_LEFT;
  gri.symbol = s;

  //   int snums = 0;
  for (int y = 0; y < Height; ++y) {
    for (int x = 0; x < Width; ++x) {
      SymbolType cs = arr.get(x, y);

      if (GameCfgT::isScatter(cs)) {
        // ++snums;

        gri.add(x, y, cs);
      }
    }
  }

  int snums = gri.lstPos.size();
  if (snums > Width) {
    snums = Width;
  }

  if (snums < 1) {
    return false;
  }

  MoneyType p = paytables.getSymbolPayout(s, snums - 1);
  if (p > 0) {
    gri.type = GRT_LINE_LEFT;
    gri.win = totalbet * p;
    gri.realWin = gri.win;

    // result.addResult(gri);

    return true;
  }

  return false;
}

}  // namespace natasha

#endif  // __NATASHA_LOGICSCATTER_H__