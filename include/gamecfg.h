#ifndef __NATASHA_GAMECFG_H__
#define __NATASHA_GAMECFG_H__

#include <assert.h>
#include <vector>
#include "array.h"
#include "utils.h"
#include "gamecode.h"

namespace natasha {

template <typename SymbolType, GameCode gamecode>
struct GameCfg {
  static bool isSameSymbol_OnLine(SymbolType s0, SymbolType s1);

  static bool isScatter(SymbolType s);

  static int getMaxScstterNums(SymbolType s);
};

}  // namespace natasha

#endif  // __NATASHA_GAMECFG_H__