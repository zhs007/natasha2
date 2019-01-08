#ifndef __NATASHA_LOGICSCATTER2_H__
#define __NATASHA_LOGICSCATTER2_H__

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
bool countScatter_Left(
    ::natashapb::GameResultInfo& gri, const SymbolBlockT& arr,
    const Paytables<Width, SymbolType, int, MoneyType>& paytables, SymbolType s,
    MoneyType totalbet) {
  // typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;
  typedef Paytables<Width, SymbolType, int, MoneyType> PaytablesT;

  clearGameResultInfo(gri);
  // gri.Clear();
  // gri.set_typegameresult(::natashapb::NULL_);
  gri.set_symbol(s);

  for (int y = 0; y < Height; ++y) {
    for (int x = 0; x < Width; ++x) {
      SymbolType cs = getSymbolBlock<SymbolBlockT, Width, Height>(&arr, x, y);

      if (GameCfgT::isScatter(cs)) {
        auto pos = gri.add_lstpos();
        pos->set_x(x);
        pos->set_y(y);

        gri.add_lstsymbol(cs);
      }
    }
  }

  int snums = gri.lstpos_size();
  if (snums > GameCfgT::getMaxScstterNums(s)) {
    snums = GameCfgT::getMaxScstterNums(s);
  }

  if (snums < 1) {
    return false;
  }
  
  MoneyType p = paytables.getSymbolPayout(s, snums - 1);
  if (p > 0) {
    gri.set_typegameresult(::natashapb::SCATTER_LEFT);
    gri.set_win(totalbet * p);
    gri.set_realwin(gri.win());

    return true;
  }

  return false;
}

}  // namespace natasha

#endif  // __NATASHA_LOGICSCATTER2_H__