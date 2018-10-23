#ifndef __NATASHA_GAME3X5_H__
#define __NATASHA_GAME3X5_H__

#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include "../protoc/base.pb.h"
#include "gamelogic.h"
#include "lines.h"
#include "logicline2.h"
#include "logicscatter2.h"
#include "paytables.h"
#include "staticcascadingreels3x5.h"
#include "symbolblock.h"
#include "util.h"

namespace natasha {

typedef Paytables<5, SymbolType, SymbolType, MoneyType> Paytables3X5;
typedef Lines<5, int> Lines3X5;
typedef Lines3X5::LineInfoT LineInfo3X5;
typedef SymbolBlock<int, 5, 3> SymbolBlock3X5;

// loadPaytables3X5 - load paytables.csv
void loadPaytables3X5(const char* fn, Paytables3X5& paytables);

// loadLines3X5 - load lines.csv
void loadLines3X5(const char* fn, Lines3X5& lines);

// loadStaticCascadingReels - StaticCascadingReels.csv
void loadStaticCascadingReels3X5(FileNameList& lstfn,
                                 StaticCascadingReels3X5& scr);

// pb::SymbolBlock3X5 -> SymbolBlock3X5
inline void setSymbolBlock5X3FromProtoc(
    SymbolBlock3X5& dest, const natashapb::SymbolBlock3X5& sb3x5) {
  dest.set(0, 0, sb3x5.dat0_0());
  dest.set(0, 1, sb3x5.dat0_1());
  dest.set(0, 2, sb3x5.dat0_2());
  dest.set(0, 3, sb3x5.dat0_3());
  dest.set(0, 4, sb3x5.dat0_4());
  dest.set(1, 0, sb3x5.dat1_0());
  dest.set(1, 1, sb3x5.dat1_1());
  dest.set(1, 2, sb3x5.dat1_2());
  dest.set(1, 3, sb3x5.dat1_3());
  dest.set(1, 4, sb3x5.dat1_4());
  dest.set(2, 0, sb3x5.dat2_0());
  dest.set(2, 1, sb3x5.dat2_1());
  dest.set(2, 2, sb3x5.dat2_2());
  dest.set(2, 3, sb3x5.dat2_3());
  dest.set(2, 4, sb3x5.dat2_4());
}

// SymbolBlock3X5 -> pb::SymbolBlock3X5
inline void setSymbolBlock3X5ToProtoc(natashapb::SymbolBlock3X5& dest,
                                      const SymbolBlock3X5& sb3x5) {
  dest.set_dat0_0(sb3x5.get(0, 0));
  dest.set_dat0_1(sb3x5.get(0, 1));
  dest.set_dat0_2(sb3x5.get(0, 2));
  dest.set_dat0_3(sb3x5.get(0, 3));
  dest.set_dat0_4(sb3x5.get(0, 4));
  dest.set_dat1_0(sb3x5.get(1, 0));
  dest.set_dat1_1(sb3x5.get(1, 1));
  dest.set_dat1_2(sb3x5.get(1, 2));
  dest.set_dat1_3(sb3x5.get(1, 3));
  dest.set_dat1_4(sb3x5.get(1, 4));
  dest.set_dat2_0(sb3x5.get(2, 0));
  dest.set_dat2_1(sb3x5.get(2, 1));
  dest.set_dat2_2(sb3x5.get(2, 2));
  dest.set_dat2_3(sb3x5.get(2, 3));
  dest.set_dat2_4(sb3x5.get(2, 4));
}

}  // namespace natasha

#endif  // __NATASHA_GAME3X5_H__