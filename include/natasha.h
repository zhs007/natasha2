#ifndef __NATASHA_NATASA_H__
#define __NATASHA_NATASA_H__

#include <assert.h>
#include <map>
#include <string>
#include <vector>
#include "../protoc/base.pb.h"
#include "symbolblock.h"

namespace natasha {

typedef SymbolBlock<int, 5, 3> SymbolBlockInt5X3;

inline void setSymbolBlockInt5X3(SymbolBlockInt5X3& dest,
                                 const natashabasepb::SymbolBlock3X5& sb3x5) {
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

#endif  // __NATASA_NATASHA_H__