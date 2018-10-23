#ifndef __NATASHA_TLOD_FREEGAME_H__
#define __NATASHA_TLOD_FREEGAME_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"

namespace natasha {

class TLODFreeGame : public SlotsGameMod {
 public:
  TLODFreeGame(StaticCascadingReels3X5& reels, Paytables3X5& paytables,
               Lines3X5& lines)
      : m_reels(reels), m_paytables(paytables), m_lines(lines) {}
  virtual ~TLODFreeGame() {}

 public:
  virtual bool init() { return true; }

  virtual bool randomReels(::natashapb::RandomResult* pRandomResult,
                           const ::natashapb::GameCtrl* pGameCtrl,
                           const ::natashapb::UserGameModInfo* pUser) {
    return true;
  }

  virtual bool spin(::natashapb::SpinResult* pSpinResult,
                    const ::natashapb::GameCtrl* pGameCtrl,
                    const ::natashapb::RandomResult* pRandomResult,
                    const ::natashapb::UserGameModInfo* pUser) {
    return true;
  }

  virtual bool onSpinEnd(::natashapb::UserGameModInfo* pUser,
                         const ::natashapb::GameCtrl* pGameCtrl,
                         const ::natashapb::SpinResult* pSpinResult,
                         const ::natashapb::RandomResult* pRandomResult) {
    return true;
  }

 protected:
  StaticCascadingReels3X5& m_reels;
  Paytables3X5& m_paytables;
  Lines3X5& m_lines;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_FREEGAME_H__