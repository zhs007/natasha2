#ifndef __NATASHA_GAMELOGIC_H__
#define __NATASHA_GAMELOGIC_H__

#include <assert.h>
#include <map>
#include <vector>
#include "array.h"
#include "symbolblock.h"
#include "utils.h"

namespace natasha {

class GameLogic {
 public:
  GameLogic() {}
  virtual ~GameLogic() {}

 public:
  virtual bool onInit() = 0;

 protected:
};

}  // namespace natasha

#endif  // __NATASHA_GAMELOGIC_H__