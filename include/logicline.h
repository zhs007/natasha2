#ifndef __NATASHA_LOGICLINE_H__
#define __NATASHA_LOGICLINE_H__

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
bool _countLine_Left(
    GameResultInfo<MoneyType, SymbolType, int, NullType>& resultinfo,
    const typename SymbolBlock<SymbolType, Width, Height>::SymbolLineT& sl,
    int lineIndex, const typename Lines<Width, int>::LineInfoT& li,
    const Paytables<Width, SymbolType, int, MoneyType>& paytables,
    MoneyType bet) {
  typedef GameResultInfo<MoneyType, SymbolType, int, NullType> GameResultInfoT;
  typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;
  typedef Lines<Width, int> LinesT;
  typedef Paytables<Width, SymbolType, int, MoneyType> PaytablesT;
  typedef typename SymbolBlockT::SymbolLineT SymbolLineT;

  //   resultinfo.clear();

  SymbolType s0 = sl.get(0);
  resultinfo.add(0, li.get(0), s0);
  resultinfo.symbol = s0;

  for (int i = 1; i < Width; ++i) {
    SymbolType cs = sl.get(i);
    if (GameCfgT::isSameSymbol_OnLine(s0, cs)) {
      resultinfo.add(0, li.get(0), s0);
    } else {
      break;
    }
  }

  if (resultinfo.lstPos.size() < 1) {
    return false;
  }

  MoneyType p = paytables.getSymbolPayout(s0, resultinfo.lstPos.size() - 1);
  if (p > 0) {
    resultinfo.type = GRT_LINE_LEFT;
    resultinfo.win = bet * p;
    resultinfo.realWin = resultinfo.win;

    return true;
  }

  return false;
}

template <typename MoneyType, typename SymbolType, int Width, int Height,
          typename GameCfgT>
void countAllLine_Left(
    GameResult<MoneyType, SymbolType, int, NullType, NullType>& result,
    const SymbolBlock<SymbolType, Width, Height>& arr,
    const Lines<Width, int>& lines,
    const Paytables<Width, SymbolType, int, MoneyType>& paytables,
    MoneyType bet) {
  typedef GameResult<MoneyType, SymbolType, int, NullType, NullType>
      GameResultT;
  typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;
  typedef Lines<Width, int> LinesT;
  typedef Paytables<Width, SymbolType, int, MoneyType> PaytablesT;
  typedef typename SymbolBlockT::SymbolLineT SymbolLineT;
  typedef GameResultInfo<MoneyType, SymbolType, int, NullType> GameResultInfoT;

  for (int i = 0; i < lines.getNums(); ++i) {
    SymbolLineT sl;
    arr.buildSymbolLine(sl, lines, i);

    GameResultInfoT gri;

    bool iswin =
        _countLine_Left<MoneyType, SymbolType, Width, Height, GameCfgT>(
            gri, sl, i, paytables);
    if (iswin) {
      result.addResult(gri);
    }
  }
}

}  // namespace natasha

#endif  // __NATASHA_LOGICLINE_H__