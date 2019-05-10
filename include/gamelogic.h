#ifndef __NATASHA_GAMELOGIC_H__
#define __NATASHA_GAMELOGIC_H__

#include <assert.h>
#include <functional>
#include <map>
#include <vector>
#include "../protoc/base.pb.h"
#include "array.h"
#include "gamemod.h"
// #include "symbolblock.h"
#include "rtp.h"
#include "utils.h"

namespace natasha {

typedef std::function< ::natashapb::CODE(::natashapb::UserGameLogicInfo*)>
    FuncProcGameCtrlResult;

class GameLogic {
 public:
  typedef std::map< ::natashapb::GAMEMODTYPE, GameMod*> MapGameMod;
  typedef MapGameMod::const_iterator ConstMapGameModIter;

 public:
  GameLogic();
  virtual ~GameLogic();

 public:
  virtual ::natashapb::CODE init(const char* cfgpath);

  virtual ::natashapb::CODE userComeIn(
      ::natashapb::UserGameLogicInfo* pLogicUser);

  virtual ::natashapb::CODE gameCtrl(
      ::natashapb::GameCtrl* pGameCtrl,
      ::natashapb::UserGameLogicInfo* pLogicUser);

  // getMainGameMod - get current main game module
  virtual GameMod* getMainGameMod(::natashapb::UserGameLogicInfo* pLogicUser,
                                  bool isComeInGame);

  // onGameCtrlEnd - onGameCtrlEnd
  //               - 留作后期处理结算以后事务用，目前不用
  virtual ::natashapb::CODE onGameCtrlEnd(
      const ::natashapb::GameCtrl* pGameCtrl,
      ::natashapb::UserGameLogicInfo* pLogicUser, GameMod* curmod,
      ::natashapb::UserGameModInfo* curugmi);

 public:
  // addGameMod - init game module
  //   Only for init
  ::natashapb::CODE addGameMod(::natashapb::GAMEMODTYPE gmt, GameMod* pMod);

  // getGameMod - get game module
  GameMod* getGameMod(::natashapb::GAMEMODTYPE gmt);

  // getUserGameModInfo - get user game module info
  ::natashapb::UserGameModInfo* getUserGameModInfo(
      ::natashapb::UserGameLogicInfo* pLogicUser, ::natashapb::GAMEMODTYPE gmt);

  // getConstUserGameModInfo - get user game module info
  const ::natashapb::UserGameModInfo* getConstUserGameModInfo(
      const ::natashapb::UserGameLogicInfo* pLogicUser,
      ::natashapb::GAMEMODTYPE gmt) const;

  // startGameMod - start game module for user
  //   Only for gamectrl
  ::natashapb::CODE startGameMod(::natashapb::GAMEMODTYPE gmt,
                                 const ::natashapb::StartGameMod* pStart,
                                 ::natashapb::UserGameLogicInfo* pLogicUser);

  // setFuncProcGameCtrlResult - set FuncProcGameCtrlResult
  void setFuncProcGameCtrlResult(FuncProcGameCtrlResult func) {
    m_funcProcGameCtrlResult = func;
  }

#ifdef NATASHA_COUNTRTP
 public:
  virtual void onInitRTP() = 0;

  void onRTPAddBet(::natashapb::GAMEMODTYPE module, MoneyType bet) {
    m_rtp.addBet(module, bet);
  }

  void onRTPAddPayout(::natashapb::GAMEMODTYPE module, MoneyType payout) {
    m_rtp.addPayout(module, payout);
  }

  virtual void onRTPAddPayoutGRI(::natashapb::GAMEMODTYPE module,
                                 const ::natashapb::SpinResult& spinret,
                                 const ::natashapb::GameResultInfo& gri,
                                 const ::natashapb::UserGameModInfo* pUser) {
    m_rtp.addSymbolPayout(module, gri.symbol(), gri.lstsymbol_size(),
                          gri.realwin());
  }

  void addRTPModule(::natashapb::GAMEMODTYPE module, int maxNums,
                    int maxSymbol) {
    m_rtp.addModule(module, maxNums, maxSymbol);
  }

  void addRTPModuleBonus(::natashapb::GAMEMODTYPE module, const char* bonusName,
                         int maxNums) {
    m_rtp.initModuleBonus(module, bonusName, maxNums);
  }

  void outputRTP() { m_rtp.output(); }
#endif  // NATASHA_COUNTRTP

 protected:
  MapGameMod m_mapGameMod;
  FuncProcGameCtrlResult m_funcProcGameCtrlResult;

#ifdef NATASHA_COUNTRTP
  RTP m_rtp;
#endif  // NATASHA_COUNTRTP
};

}  // namespace natasha

#endif  // __NATASHA_GAMELOGIC_H__