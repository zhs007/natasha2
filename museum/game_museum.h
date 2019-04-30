#ifndef __NATASHA_GAME_MUSEUM_H__
#define __NATASHA_GAME_MUSEUM_H__

#include <assert.h>
#include <vector>
#include "../include/natasha.h"

namespace natasha {

const int MUSEUM_WIDTH = 5;
const int MUSEUM_HEIGHT = 3;

const int MUSEUM_DEFAULT_LINES = 243;
const int MUSEUM_DEFAULT_PAY_LINES = 30;
const int MUSEUM_DEFAULT_TIMES = 1;

const int MUSEUM_DEFAULT_FREENUMS = 10;

const SymbolType MUSEUM_SYMBOL_W = 0;
const SymbolType MUSEUM_SYMBOL_S = 10;

const int MaxSymbols = 11;
const int MaxPayoutNums = 5;

const char MUSEUM_SYMBOL_MAPPING[] = " wabcdefghjs";

struct MuseumGameCfg {
  static bool isSameSymbol_OnLine(SymbolType s0, SymbolType s1) {
    if (s0 == MUSEUM_SYMBOL_S || s1 == MUSEUM_SYMBOL_S) {
      return false;
    }

    if (s0 == s1) {
      return true;
    }

    return s0 == MUSEUM_SYMBOL_W || s1 == MUSEUM_SYMBOL_W;
  }

  static bool isScatter(SymbolType s) { return s == MUSEUM_SYMBOL_S; }

  static int getMaxScstterNums(SymbolType s) { return MUSEUM_WIDTH; }
};

auto const MuseumCountScatter =
    &countScatter_Left<MoneyType, SymbolType, MUSEUM_WIDTH, MUSEUM_HEIGHT,
                       ::natashapb::SymbolBlock3X5, MuseumGameCfg>;

auto const TLODCountAllLine =
    &countAllLine_Left<MoneyType, SymbolType, MUSEUM_WIDTH, MUSEUM_HEIGHT,
                       ::natashapb::SymbolBlock3X5, MuseumGameCfg>;

}  // namespace natasha

#endif  // __NATASHA_GAME_MUSEUM_H__