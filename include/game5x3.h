#ifndef __NATASHA_GAME5X3_H__
#define __NATASHA_GAME5X3_H__

#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include "../protoc/base.pb.h"
#include "lines.h"
#include "paytables.h"
#include "staticcascadingreels.h"
#include "symbolblock.h"
#include "util.h"

namespace natasha {

typedef Paytables<5, int, int, int64_t> Paytables5X3;
typedef Lines<5, int> Lines5X3;
typedef Lines5X3::LineInfoT LineInfo5X3;
typedef SymbolBlock<int, 5, 3> SymbolBlock5X3;
typedef StaticCascadingReels<int, 5, 3> StaticCascadingReels5X3;

// loadPaytables5X3 - load paytables.csv
void loadPaytables5X3(const char* fn, Paytables5X3& paytables);

// loadLines5X3 - load lines.csv
void loadLines5X3(const char* fn, Lines5X3& lines);

// loadStaticCascadingReels - StaticCascadingReels.csv
void loadStaticCascadingReels5X3(FileNameList& lstfn,
                                 StaticCascadingReels5X3& scr);

inline void setSymbolBlock5X3(SymbolBlock5X3& dest,
                              const natashapb::SymbolBlock3X5& sb3x5) {
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

}  // namespace natasha

#endif  // __NATASHA_GAME5X3_H__