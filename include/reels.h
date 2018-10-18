#ifndef __NATASHA_REELS_H__
#define __NATASHA_REELS_H__

#include <assert.h>
#include <map>
#include "../protoc/base.pb.h"
#include "array.h"
#include "symbolblock.h"
#include "utils.h"

namespace natasha {

template <typename SymbolType, int Width, int Height>
class BaseReels {
 public:
  typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;

 public:
  BaseReels() {}
  virtual ~BaseReels() {}

 public:
  virtual void random(::natashapb::RandomResult* pRandomResult,
                      SymbolBlockT& symbolBlock) = 0;

  virtual void clear() = 0;

  virtual bool isEmpty() const = 0;

 protected:
};

}  // namespace natasha

#endif  // __NATASHA_REELS_H__