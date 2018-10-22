#ifndef __NATASHA_TLOD_H__
#define __NATASHA_TLOD_H__

#include <assert.h>
#include <vector>
#include "../include/game5x3.h"
#include "../include/gamelogic.h"
#include "basegame.h"
#include "freegame.h"

namespace natasha {

class TLOD : public GameLogic {
 public:
  TLOD() {}
  virtual ~TLOD() {}

 public:
  virtual bool init();

 protected:
  StaticCascadingReels5X3 m_reels;
  Paytables5X3 m_paytables;
  Lines5X3 m_lines;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_H__