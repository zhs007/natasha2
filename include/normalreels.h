#ifndef __NATASHA_NORMALREELS_H__
#define __NATASHA_NORMALREELS_H__

#include <assert.h>
#include <map>
#include "reels.h"

namespace natasha {

template <typename SymbolType>
class NormalReels : public BaseReels<SymbolType> {
    public:
    typedef BaseReels<SymbolType> BaseReelsT;
 public:
  NormalReels() {}

 public:
 protected:
};

}  // namespace natasha

#endif  // __NATASHA_NORMALREELS_H__