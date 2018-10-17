#ifndef __NATASHA_SYMBOLBLOCK_H__
#define __NATASHA_SYMBOLBLOCK_H__

#include <assert.h>
#include <map>
#include "array.h"
#include "lines.h"
#include "utils.h"

namespace natasha {

template <typename SymbolType, int Width, int Height>
class SymbolBlock {
 public:
  typedef Array2D<SymbolType, Width, Height> Array2DT;
  typedef StaticArray<Width, SymbolType> SymbolLineT;
  typedef Lines<Width, int> LinesT;
  typedef typename LinesT::LineInfoT LineInfoT;
  typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;

 public:
  SymbolBlock() {}
  ~SymbolBlock() {}

 public:
  void clear(SymbolType s) { m_arr.clear(s); }

  SymbolType get(int x, int y) const { return m_arr.get(x, y); }

  void set(int x, int y, SymbolType s) { m_arr.set(x, y, s); }

  void buildSymbolLine(SymbolLineT& sl, const LinesT& lines,
                       int indexLine) const {
    const LineInfoT& li = lines.get(indexLine);

    for (int i = 0; i < Width; ++i) {
      sl.set(i, m_arr.get(i, li.get(i)));
    }
  }

  void cloneWith(const SymbolBlockT& sb) { m_arr.cloneWith(sb.m_arr); }

 protected:
  Array2DT m_arr;
};

}  // namespace natasha

#endif  // __NATASHA_SYMBOLBLOCK_H__