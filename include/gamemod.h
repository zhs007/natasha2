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

class GameLogic;

class GameMod {
 public:
  GameMod(GameLogic& logic) : m_logic(logic) {}
  virtual ~GameMod() {}

 public:
  virtual ::natashapb::CODE init() = 0;

  // start - start cur game module for user
  virtual ::natashapb::CODE start(::natashapb::UserGameModInfo* pUser,
                                  const ::natashapb::StartGameMod* pStart,
                                  CtrlID nextCtrlID) = 0;

  // isIn - is in current game module
  virtual bool isIn(const ::natashapb::UserGameModInfo* pUser) = 0;

  // reviewGameCtrl - check & fix gamectrl params from client
  virtual ::natashapb::CODE reviewGameCtrl(
      ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUser) = 0;

  // onUserComeIn -
  virtual ::natashapb::CODE onUserComeIn(::natashapb::UserGameModInfo* pUser) {
    return ::natashapb::OK;
  }

  // updGameResult -
  virtual ::natashapb::CODE updGameResult(::natashapb::UserGameModInfo* pUser,
                                          MoneyType realbet,
                                          MoneyType realwin) {
    return ::natashapb::OK;
  }

 protected:
  GameLogic& m_logic;
};

class SlotsGameMod : public GameMod {
 public:
  SlotsGameMod(GameLogic& logic) : GameMod(logic) {}
  virtual ~SlotsGameMod() {}

 public:
  // randomReels - random reels
  virtual ::natashapb::CODE randomReels(
      ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUser) = 0;

  // countSpinResult - count spin result
  virtual ::natashapb::CODE countSpinResult(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::UserGameModInfo* pUser) = 0;

  // procSpinResult - count spin result
  virtual ::natashapb::CODE procSpinResult(
      ::natashapb::UserGameModInfo* pUser,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::RandomResult* pRandomResult, CtrlID nextCtrlID,
      ::natashapb::UserGameLogicInfo* pLogicUser) = 0;

 protected:
};

}  // namespace natasha

#endif  // __NATASHA_GAMEMOD_H__