#ifndef __NATASHA_GAMELOGIC_H__
#define __NATASHA_GAMELOGIC_H__

#include <assert.h>
#include <map>
#include <vector>
#include "../protoc/base.pb.h"
#include "array.h"
#include "symbolblock.h"
#include "utils.h"

namespace natasha {

template <typename SymbolType>
class GameLogic {
 public:
  typedef BaseSymbolBlock<SymbolType> BaseSymbolBlockT;

 public:
  GameLogic() {}
  virtual ~GameLogic() {}

 public:
  virtual bool init() = 0;

  virtual bool randomReels(::natashapb::RandomResult* pRandomResult,
                           BaseSymbolBlockT* pSymbolBlock) = 0;

  virtual bool countResult(::natashapb::GameResultChunk* pGameResultChunk,
                           ::natashapb::RandomResult* pRandomResult,
                           BaseSymbolBlockT* pSymbolBlock) = 0;

 protected:
};

typedef GameLogic<int> GameLogicInt;

}  // namespace natasha

#endif  // __NATASHA_GAMELOGIC_H__