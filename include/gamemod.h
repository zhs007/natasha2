#ifndef __NATASHA_GAMEMOD_H__
#define __NATASHA_GAMEMOD_H__

#include <assert.h>
#include <map>
#include <vector>
#include "../protoc/base.pb.h"
#include "array.h"
// #include "symbolblock.h"
#include "utils.h"

namespace natasha {

class GameMod {
 public:
  GameMod() {}
  virtual ~GameMod() {}

 public:
  virtual bool init() = 0;

  virtual bool reviewGameCtrl(::natashapb::GameCtrl* pGameCtrl,
                              const ::natashapb::UserGameModInfo* pUser) = 0;

 protected:
};

class SlotsGameMod : public GameMod {
 public:
  SlotsGameMod() {}
  virtual ~SlotsGameMod() {}

 public:
  virtual bool randomReels(::natashapb::RandomResult* pRandomResult,
                           const ::natashapb::GameCtrl* pGameCtrl,
                           const ::natashapb::UserGameModInfo* pUser) = 0;

  virtual bool spin(::natashapb::SpinResult* pSpinResult,
                    const ::natashapb::GameCtrl* pGameCtrl,
                    const ::natashapb::RandomResult* pRandomResult,
                    const ::natashapb::UserGameModInfo* pUser) = 0;

  virtual bool onSpinEnd(::natashapb::UserGameModInfo* pUser,
                         const ::natashapb::GameCtrl* pGameCtrl,
                         const ::natashapb::SpinResult* pSpinResult,
                         const ::natashapb::RandomResult* pRandomResult) = 0;

 protected:
};

}  // namespace natasha

#endif  // __NATASHA_GAMEMOD_H__