#ifndef __NATASHA_TLOD_BASEGAME_H__
#define __NATASHA_TLOD_BASEGAME_H__

#include <assert.h>
#include <vector>
#include "../include/game5x3.h"
#include "../include/gamelogic.h"

namespace natasha {

class TLODBaseGame : public SlotsGameMod {
 public:
  TLODBaseGame(StaticCascadingReels5X3& reels, Paytables5X3& paytables,
               Lines5X3& lines)
      : m_reels(reels), m_paytables(paytables), m_lines(lines) {}
  virtual ~TLODBaseGame() {}

 public:
  virtual bool init() {}

  virtual bool randomReels(::natashapb::RandomResult* pRandomResult,
                           const ::natashapb::UserGameModInfo* pUser) {}

  virtual bool spin(::natashapb::SpinResult* pSpinResult,
                    const ::natashapb::RandomResult* pRandomResult,
                    const ::natashapb::UserGameModInfo* pUser) {}

  virtual bool onSpinEnd(::natashapb::UserGameModInfo* pUser,
                         const ::natashapb::SpinResult* pSpinResult,
                         const ::natashapb::RandomResult* pRandomResult) {}

 protected:
  StaticCascadingReels5X3& m_reels;
  Paytables5X3& m_paytables;
  Lines5X3& m_lines;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_BASEGAME_H__