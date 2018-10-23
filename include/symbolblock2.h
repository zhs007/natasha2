#ifndef __NATASHA_SYMBOLBLOCK2_H__
#define __NATASHA_SYMBOLBLOCK2_H__

#include <assert.h>
#include <map>
#include "../protoc/base.pb.h"
#include "array.h"
#include "lines.h"
#include "utils.h"

namespace natasha {

int32_t getSymbolBlock3X5(const ::natashapb::SymbolBlock3X5* pSB, int x, int y);

void setSymbolBlock3X5(::natashapb::SymbolBlock3X5* pSB, int x, int y,
                       int32_t s);

void buildSymbolLine3X5(const ::natashapb::SymbolBlock3X5* pSB,
                        StaticArray<5, int32_t>& sl, const Lines<5, int>& lines,
                        int indexLine);

}  // namespace natasha

#endif  // __NATASHA_SYMBOLBLOCK2_H__