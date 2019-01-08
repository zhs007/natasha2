#ifndef __NATASHA_GAME_TLOD_H__
#define __NATASHA_GAME_TLOD_H__

#include <assert.h>
#include <vector>
#include "../include/natasha.h"

namespace natasha {

const int TLOD_WIDTH = 5;
const int TLOD_HEIGHT = 3;

const int TLOD_DEFAULT_LINES = 30;
const int TLOD_DEFAULT_PAY_LINES = 30;
const int TLOD_DEFAULT_TIMES = 1;

const int TLOD_DEFAULT_FREENUMS = 10;

const SymbolType TLOD_SYMBOL_W = 0;
const SymbolType TLOD_SYMBOL_S = 12;

const char TLOD_SYMBOL_MAPPING[] = " wabcdefghijks";

struct TLODGameCfg {
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

auto const TLODCountScatter =
    &countScatter_Left<MoneyType, SymbolType, TLOD_WIDTH, TLOD_HEIGHT,
                       ::natashapb::SymbolBlock3X5, TLODGameCfg>;

auto const TLODCountAllLine =
    &countAllLine_Left<MoneyType, SymbolType, TLOD_WIDTH, TLOD_HEIGHT,
                       ::natashapb::SymbolBlock3X5, TLODGameCfg>;

}  // namespace natasha

#endif  // __NATASHA_GAME_TLOD_H__