#ifndef __NATASHA_GAME_TLOD_H__
#define __NATASHA_GAME_TLOD_H__

#include <assert.h>
#include <vector>
#include "../include/array.h"
#include "../include/gamecfg.h"
#include "../include/gamecode.h"
#include "../include/utils.h"

namespace natasha {

const int TLOD_WIDTH = 5;
const int TLOD_HEIGHT = 3;

const int TLOD_SYMBOL_W = 0;
const int TLOD_SYMBOL_S = 12;

template <typename SymbolType>
struct GameCfg<SymbolType, GAMECODE_TLOD> {
  static bool isSameSymbol_OnLine(SymbolType s0, SymbolType s1) {
    if (s0 == TLOD_SYMBOL_S || s1 == TLOD_SYMBOL_S) {
      return false;
    }

    if (s0 == s1) {
      return true;
    }

    return s0 == TLOD_SYMBOL_W || s1 == TLOD_SYMBOL_W;
  }

  static bool isScatter(SymbolType s) { return s == TLOD_SYMBOL_S; }

  static int getMaxScstterNums(SymbolType s) { return TLOD_WIDTH; }
};

}  // namespace natasha

#endif  // __NATASHA_GAME_TLOD_H__