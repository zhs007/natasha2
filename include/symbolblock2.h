#ifndef __NATASHA_SYMBOLBLOCK2_H__
#define __NATASHA_SYMBOLBLOCK2_H__

#include <assert.h>
#include <map>
#include "../protoc/base.pb.h"
#include "array.h"
#include "lines.h"
#include "utils.h"

namespace natasha {

// getSymbolBlock3X5 - getSymbol(x, y)
SymbolType getSymbolBlock3X5(const ::natashapb::SymbolBlock3X5* pSB, int x,
                             int y);

// setSymbolBlock3X5 - setSymbol(x, y, symbol)
void setSymbolBlock3X5(::natashapb::SymbolBlock3X5* pSB, int x, int y,
                       SymbolType s);

void buildSymbolLine3X5(const ::natashapb::SymbolBlock3X5* pSB,
                        StaticArray<5, SymbolType>& sl,
                        const Lines<5, int>& lines, int indexLine);

// cascadeSymbol3X5 - cascade SymbolBlock
void cascadeBlock3X5(::natashapb::SymbolBlock3X5* pSB);

// removeBlock3X5WithGameResult - remove all symbol in gameresult
bool removeBlock3X5WithGameResult(::natashapb::SymbolBlock3X5* pSB,
                                  ::natashapb::SpinResult* pSpinResult);

template <typename SymbolBlockT, int Width, int Height>
void buildSymbolLine(const SymbolBlockT* pSB,
                     StaticArray<Width, SymbolType>& sl,
                     const Lines<Width, int>& lines, int indexLine);

template <>
inline void buildSymbolLine< ::natashapb::SymbolBlock3X5, 5, 3>(
    const ::natashapb::SymbolBlock3X5* pSB, StaticArray<5, SymbolType>& sl,
    const Lines<5, int>& lines, int indexLine) {
  buildSymbolLine3X5(pSB, sl, lines, indexLine);
};

template <typename SymbolBlockT, int Width, int Height>
SymbolType getSymbolBlock(const SymbolBlockT* pSB, int x, int y);

template <>
inline SymbolType getSymbolBlock< ::natashapb::SymbolBlock3X5, 5, 3>(
    const ::natashapb::SymbolBlock3X5* pSB, int x, int y) {
  return getSymbolBlock3X5(pSB, x, y);
};

template <typename SymbolBlockT, int Width, int Height>
void setSymbolBlock(SymbolBlockT* pSB, int x, int y, SymbolType s);

template <>
inline void setSymbolBlock< ::natashapb::SymbolBlock3X5, 5, 3>(
    ::natashapb::SymbolBlock3X5* pSB, int x, int y, SymbolType s) {
  setSymbolBlock3X5(pSB, x, y, s);
};

}  // namespace natasha

#endif  // __NATASHA_SYMBOLBLOCK2_H__