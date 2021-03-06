#ifndef __NATASHA_GAMEMOD_H__
#define __NATASHA_GAMEMOD_H__

#include <assert.h>
#include <map>
#include <vector>
#include "../protoc/base.pb.h"
#include "array.h"
#include "userinfo.h"
#include "utils.h"

namespace natasha {

class GameLogic;

class GameMod {
 public:
  GameMod(GameLogic& logic, ::natashapb::GAMEMODTYPE gmt)
      : m_logic(logic), m_gmt(gmt) {}
  virtual ~GameMod() {}

 public:
  virtual ::natashapb::CODE init() = 0;

  // start - start cur game module for user
  virtual ::natashapb::CODE start(::natashapb::UserGameModInfo* pUGMI,
                                  const ::natashapb::StartGameMod* pStart,
                                  const UserInfo* pUser) = 0;

  // isIn - is in current game module
  virtual bool isIn(const ::natashapb::UserGameModInfo* pUser) = 0;

  // reviewGameCtrl - check & fix gamectrl params from client
  virtual ::natashapb::CODE reviewGameCtrl(
      ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUser) = 0;

  // onUserComeIn -
  virtual ::natashapb::CODE onUserComeIn(const UserInfo* pUser,
                                         ::natashapb::UserGameModInfo* pUGMI) {
    return ::natashapb::ERR_NO_OVERLOADED_INTERFACE;
  }

  // onGameCtrl
  virtual ::natashapb::CODE onGameCtrl(
      const ::natashapb::GameCtrl* pGameCtrl, UserInfo* pUser,
      ::natashapb::UserGameModInfo* pMainUGMI) {
    return ::natashapb::ERR_NO_OVERLOADED_INTERFACE;
  }

  // onGameCtrlEnd
  virtual ::natashapb::CODE onGameCtrlEnd(
      const ::natashapb::GameCtrl* pGameCtrl, UserInfo* pUser,
      ::natashapb::UserGameModInfo* pMainUGMI) {
    return ::natashapb::ERR_NO_OVERLOADED_INTERFACE;
  }

  // updGameResult -
  virtual ::natashapb::CODE updGameResult(::natashapb::UserGameModInfo* pUser,
                                          MoneyType realbet,
                                          MoneyType realwin) {
    return ::natashapb::ERR_NO_OVERLOADED_INTERFACE;
  }

  // setCurGameCtrlID - set ctrlid
  //                 只在产生特殊情况下才调用，用来配置baseid和parentid
  virtual ::natashapb::CODE setCurGameCtrlID(
      ::natashapb::UserGameModInfo* pUser, CtrlID curCtrlID) {
    return ::natashapb::ERR_NO_OVERLOADED_INTERFACE;
  }

  // clearUGMI - clear UserGameModInfo
  //           - 初始化用户游戏模块数据
  virtual ::natashapb::CODE clearUGMI(::natashapb::UserGameModInfo* pUser) {
    return ::natashapb::ERR_NO_OVERLOADED_INTERFACE;
  }

  // isCompeleted - isCompeleted
  //              - 游戏特殊状态是否已结束
  virtual bool isCompeleted(::natashapb::UserGameModInfo* pUser) {
    return false;
  }

  // makeInitScenario - make a initial scenario
  //                  - 产生一个初始局面，不中奖的
  virtual ::natashapb::CODE makeInitScenario(
      ::natashapb::GameCtrl* pGameCtrl, const UserInfo* pUser,
      ::natashapb::UserGameModInfo* pUGMI) {
    return ::natashapb::ERR_NO_OVERLOADED_INTERFACE;
  }

 public:
  // getGameModType - get GAMEMODTYPE
  ::natashapb::GAMEMODTYPE getGameModType() { return m_gmt; }

  // getUserGameModInfo - get user game module info
  ::natashapb::UserGameModInfo* getUserGameModInfo(UserInfo* pUser);

  void* getUserConfig(const UserInfo* pUser) { return pUser->pCurConfig; }

 protected:
  GameLogic& m_logic;
  ::natashapb::GAMEMODTYPE m_gmt;
};

class SlotsGameMod : public GameMod {
 public:
  SlotsGameMod(GameLogic& logic, ::natashapb::GAMEMODTYPE gmt)
      : GameMod(logic, gmt) {}
  virtual ~SlotsGameMod() {}

 public:
  // onGameCtrl
  virtual ::natashapb::CODE onGameCtrl(
      const ::natashapb::GameCtrl* pGameCtrl, UserInfo* pUser,
      ::natashapb::UserGameModInfo* pMainUGMI) {
    assert(pUser != NULL);
    assert(pUser->pLogicUser != NULL);
    // assert(pMainUGMI->has_randomresult());
    // assert(pMainUGMI->has_spinresult());

    // auto pLogicUser = pUser->pLogicUser;

    auto code = this->onSpinStart(pMainUGMI, pGameCtrl, pUser);
    if (code != ::natashapb::OK) {
      return code;
    }

    code = this->randomReels(pMainUGMI->mutable_randomresult(), pGameCtrl,
                             pMainUGMI, pUser);
    if (code != ::natashapb::OK) {
      return code;
    }

    code = this->countSpinResult(pMainUGMI->mutable_spinresult(), pGameCtrl,
                                 pMainUGMI->mutable_randomresult(), pMainUGMI,
                                 pUser);
    if (code != ::natashapb::OK) {
      return code;
    }

    code = this->procSpinResult(pMainUGMI, pGameCtrl,
                                pMainUGMI->mutable_spinresult(),
                                pMainUGMI->mutable_randomresult(), pUser);
    if (code != ::natashapb::OK) {
      return code;
    }

    code =
        this->onSpinEnd(pMainUGMI, pGameCtrl, pMainUGMI->mutable_spinresult(),
                        pMainUGMI->mutable_randomresult(), pUser);
    if (code != ::natashapb::OK) {
      return code;
    }

    return ::natashapb::OK;
  }

  // setCurGameCtrlID - set ctrlid
  //                 只在产生特殊情况下才调用，用来配置baseid和parentid
  virtual ::natashapb::CODE setCurGameCtrlID(
      ::natashapb::UserGameModInfo* pUser, CtrlID curCtrlID) {
    auto curgamrctrlid = pUser->mutable_gamectrlid();

    if (curgamrctrlid->baseid() == 0) {
      curgamrctrlid->set_baseid(curCtrlID);
      curgamrctrlid->set_parentid(curCtrlID);
      curgamrctrlid->set_ctrlid(curCtrlID);
      curgamrctrlid->set_gamemod(this->m_gmt);
    } else {
      curgamrctrlid->set_parentid(curgamrctrlid->ctrlid());
      curgamrctrlid->set_ctrlid(curCtrlID);
    }
    return ::natashapb::OK;
  }

  // makeInitScenario - make a initial scenario
  //                  - 产生一个初始局面，不中奖的
  virtual ::natashapb::CODE makeInitScenario(
      ::natashapb::GameCtrl* pGameCtrl, const UserInfo* pUser,
      ::natashapb::UserGameModInfo* pUGMI);

 public:
  // randomReels - random reels
  virtual ::natashapb::CODE randomReels(
      ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUGMI, const UserInfo* pUser) = 0;

  // countSpinResult - count spin result
  virtual ::natashapb::CODE countSpinResult(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::UserGameModInfo* pUGMI, const UserInfo* pUser) = 0;

  // procSpinResult - proc spin result
  virtual ::natashapb::CODE procSpinResult(
      ::natashapb::UserGameModInfo* pUGMI,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::RandomResult* pRandomResult, UserInfo* pUser) = 0;

  // onSpinStart - on spin start
  virtual ::natashapb::CODE onSpinStart(::natashapb::UserGameModInfo* pUGMI,
                                        const ::natashapb::GameCtrl* pGameCtrl,
                                        UserInfo* pUser) = 0;

  // onSpinEnd - on spin end
  virtual ::natashapb::CODE onSpinEnd(::natashapb::UserGameModInfo* pUGMI,
                                      const ::natashapb::GameCtrl* pGameCtrl,
                                      ::natashapb::SpinResult* pSpinResult,
                                      ::natashapb::RandomResult* pRandomResult,
                                      UserInfo* pUser) = 0;

  // buildSpinResultSymbolBlock - build spin result's symbol block
  virtual ::natashapb::CODE buildSpinResultSymbolBlock(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::UserGameModInfo* pUGMI,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult, const UserInfo* pUser,
      const void* pCfg) = 0;

 public:
  // clearRespinHistory
  ::natashapb::CODE clearRespinHistory(::natashapb::UserGameModInfo* pUser);
  // addRespinHistory
  ::natashapb::CODE addRespinHistory(::natashapb::UserGameModInfo* pUser,
                                     int64_t realWin, int64_t win, int64_t mul,
                                     bool isSpecial);

 protected:
};

}  // namespace natasha

#endif  // __NATASHA_GAMEMOD_H__