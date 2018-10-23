#ifndef __NATASHA_LOGICLINE_H__
#define __NATASHA_LOGICLINE_H__

#include <assert.h>
#include <map>
#include "../protoc/base.pb.h"
#include "array.h"
#include "gameresult.h"
#include "lines.h"
#include "paytables.h"
#include "symbolblock2.h"
#include "utils.h"

namespace natasha {

template <typename MoneyType, typename SymbolType, int Width, int Height,
          class SymbolBlockT, typename GameCfgT>
bool _countLine_Left(
    ::natashapb::GameResultInfo& gri, const StaticArray<Width, SymbolType>& sl,
    int lineIndex, const typename Lines<Width, int>::LineInfoT& li,
    const Paytables<Width, SymbolType, int, MoneyType>& paytables,
    MoneyType bet) {
  typedef GameResultInfo<MoneyType, SymbolType, int, NullType> GameResultInfoT;
  //   typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;
  typedef Lines<Width, int> LinesT;
  typedef Paytables<Width, SymbolType, int, MoneyType> PaytablesT;
  // typedef typename SymbolBlockT::SymbolLineT SymbolLineT;

  clearGameResultInfo(gri);
  // gri.Clear();
  // gri.set_typegameresult(::natashapb::NULL_);

  SymbolType s0 = sl.get(0);
  gri.add_lstsymbol(s0);
  auto pos0 = gri.add_lstpos();
  pos0->set_x(0);
  pos0->set_y(li.get(0));

  for (int i = 1; i < Width; ++i) {
    SymbolType cs = sl.get(i);
    if (GameCfgT::isSameSymbol_OnLine(s0, cs)) {
      gri.add_lstsymbol(cs);
      auto pos0 = gri.add_lstpos();
      pos0->set_x(i);
      pos0->set_y(li.get(i));
    } else {
      break;
    }
  }

  MoneyType p = paytables.getSymbolPayout(s0, gri.lstpos_size());
  if (p > 0) {
    gri.set_typegameresult(::natashapb::LINE_LEFT);
    gri.set_win(bet * p);
    gri.set_realwin(gri.win());

    return true;
  }

  return false;
}

template <typename MoneyType, typename SymbolType, int Width, int Height,
          typename SymbolBlockT, typename GameCfgT>
void countAllLine_Left(
    ::natashapb::SpinResult& sr, const SymbolBlockT& arr,
    const Lines<Width, int>& lines,
    const Paytables<Width, SymbolType, int, MoneyType>& paytables,
    MoneyType bet) {
  typedef GameResult<MoneyType, SymbolType, int, NullType, NullType>
      GameResultT;
  //   typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;
  typedef Lines<Width, int> LinesT;
  typedef Paytables<Width, SymbolType, int, MoneyType> PaytablesT;
  typedef StaticArray<Width, SymbolType> SymbolLineT;
  // typedef typename SymbolBlockT::SymbolLineT SymbolLineT;
  typedef GameResultInfo<MoneyType, SymbolType, int, NullType> GameResultInfoT;

  for (int i = 0; i < lines.getNums(); ++i) {
    SymbolLineT sl;

    buildSymbolLine<SymbolBlockT, Width, Height>(&arr, sl, lines, i);
    // arr.buildSymbolLine(sl, lines, i);

    ::natashapb::GameResultInfo gri;

    bool iswin =
        _countLine_Left<MoneyType, SymbolType, Width, Height, SymbolBlockT,
                        GameCfgT>(gri, sl, i, lines.get(i), paytables, bet);
    if (iswin) {
      auto curgri = sr.add_lstgri();
      *curgri = gri;
    }
  }
}

}  // namespace natasha

#endif  // __NATASHA_LOGICLINE_H__