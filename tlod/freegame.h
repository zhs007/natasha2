#ifndef __NATASHA_TLOD_FREEGAME_H__
#define __NATASHA_TLOD_FREEGAME_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/gamelogic.h"
#include "game_tlod.h"

namespace natasha {

const int32_t TLOD_FG_UGMI_VER = 1;

class TLODFreeGame : public SlotsGameMod {
 public:
  TLODFreeGame(GameLogic& logic, StaticCascadingReels3X5& reels,
               Paytables3X5& paytables, Lines3X5& lines, BetList& lstBet)
      : SlotsGameMod(logic, ::natashapb::FREE_GAME),
        m_reels(reels),
        m_paytables(paytables),
        m_lines(lines),
        m_lstBet(lstBet) {}
  virtual ~TLODFreeGame() {}

 public:
  virtual ::natashapb::CODE init() { return ::natashapb::OK; }

  // start - start cur game module for user
  //    basegame does not need to handle this
  virtual ::natashapb::CODE start(::natashapb::UserGameModInfo* pUGMI,
                                  const ::natashapb::StartGameMod* pStart,
                                  const UserInfo* pUser) {
    assert(pStart->has_freegame());
    assert(pStart->has_parentctrlid());

    if (pStart->freegame().freenums() != TLOD_DEFAULT_FREENUMS) {
      return ::natashapb::INVALID_START_FREEGAME_NUMS;
    }

    if (pStart->freegame().lines() != TLOD_DEFAULT_PAY_LINES) {
      return ::natashapb::INVALID_START_LINES;
    }

    if (pStart->freegame().times() != TLOD_DEFAULT_TIMES) {
      return ::natashapb::INVALID_START_TIMES;
    }
#ifdef NATASHA_SERVER
    auto it =
        std::find(m_lstBet.begin(), m_lstBet.end(), pStart->freegame().bet());
    if (it == m_lstBet.end()) {
      return ::natashapb::INVALID_START_BET;
    }
#endif  // NATASHA_SERVER

    if (pStart->parentctrlid().ctrlid() <= 0) {
      return ::natashapb::INVALID_PARENTID;
    }

    if (pStart->parentctrlid().gamemod() != ::natashapb::BASE_GAME) {
      return ::natashapb::INVALID_PARENT_GAMEMOD;
    }

    if (this->isIn(pUGMI)) {
      return ::natashapb::ALREADY_IN_FREEGAME;
    }

    this->clearUGMI(pUGMI);

    auto freeinfo = pUGMI->mutable_freeinfo();
    freeinfo->set_curbet(pStart->freegame().bet());
    freeinfo->set_curlines(pStart->freegame().lines());
    freeinfo->set_curtimes(pStart->freegame().times());
    freeinfo->set_curnums(0);
    freeinfo->set_lastnums(pStart->freegame().freenums());
    freeinfo->set_totalwin(0);

    pUGMI->mutable_cascadinginfo()->set_isend(true);

    // printGameCtrlID("tlod start freegame", pStart->parentctrlid());

    setGameCtrlID(*pUGMI->mutable_gamectrlid(), pStart->parentctrlid(), 0,
                  ::natashapb::FREE_GAME);

    return ::natashapb::OK;
  }

  // onUserComeIn -
  virtual ::natashapb::CODE onUserComeIn(const UserInfo* pUser,
                                         ::natashapb::UserGameModInfo* pUGMI) {
    assert(pUser != NULL);
    assert(pUGMI != NULL);

    // 版本号用来区分数据版本
    // 版本号也可以用于判断数据是否已经初始化
    if (pUGMI->ver() != TLOD_FG_UGMI_VER) {
      auto code = this->clearUGMI(pUGMI);
      if (code != ::natashapb::OK) {
        return code;
      }

      pUGMI->mutable_cascadinginfo()->set_isend(true);

      ::natashapb::GameCtrl gamectrl;
      code = this->makeInitScenario(&gamectrl, pUser, pUGMI);
      if (code != ::natashapb::OK) {
        return code;
      }
    }

    return ::natashapb::OK;
  }

  // isIn - is in current game module
  virtual bool isIn(const ::natashapb::UserGameModInfo* pUser) {
    if (pUser->has_freeinfo()) {
      if (pUser->freeinfo().lastnums() > 0) {
        return true;
      }

      return !pUser->cascadinginfo().isend();
    }

    return false;
  }

  // reviewGameCtrl - check & fix gamectrl params from client
  virtual ::natashapb::CODE reviewGameCtrl(
      ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUser) {
    assert(pUser->has_cascadinginfo());
    assert(!pGameCtrl->has_spin());
    assert(pUser->has_freeinfo());

    if (!pGameCtrl->has_freespin()) {
      return ::natashapb::INVALID_GAMECTRL_GAMEMOD;
    }
#ifdef NATASHA_SERVER
    auto spinctrl = pGameCtrl->mutable_freespin();
    spinctrl->set_bet(pUser->freeinfo().curbet());
    spinctrl->set_lines(TLOD_DEFAULT_PAY_LINES);
    spinctrl->set_times(TLOD_DEFAULT_TIMES);
    spinctrl->set_totalbet(pUser->freeinfo().curbet() * TLOD_DEFAULT_PAY_LINES);
    spinctrl->set_realbet(0);
#endif  // NATASHA_SERVER

#ifdef NATASHA_SERVER
    auto it = std::find(m_lstBet.begin(), m_lstBet.end(), spinctrl->bet());
    if (it == m_lstBet.end()) {
      return ::natashapb::INVALID_BET;
    }
#endif  // NATASHA_SERVER

    return ::natashapb::OK;
  }

  // randomReels - random reels
  virtual ::natashapb::CODE randomReels(
      ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::UserGameModInfo* pUGMI, const UserInfo* pUser) {
    m_reels.random(pRandomResult, pUGMI);

    return ::natashapb::OK;
  }

  // countSpinResult - count spin result
  virtual ::natashapb::CODE countSpinResult(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult,
      const ::natashapb::UserGameModInfo* pUGMI, const UserInfo* pUser) {
    assert(pSpinResult != NULL);
    assert(pGameCtrl != NULL);
    assert(pRandomResult != NULL);
    assert(pUser != NULL);
    assert(pUGMI != NULL);

    pSpinResult->Clear();
#ifdef NATASHA_SERVER
    this->buildSpinResultSymbolBlock(pSpinResult, pUGMI, pGameCtrl,
                                     pRandomResult, pUser, NULL);
#endif  // NATASHA_SERVER

    auto sb3x5 = pRandomResult->scrr3x5().symbolblock().sb3x5();

    // First check free
    ::natashapb::GameResultInfo gri;
    TLODCountScatter(gri, sb3x5, m_paytables, TLOD_SYMBOL_S,
                     pGameCtrl->freespin().bet() * TLOD_DEFAULT_PAY_LINES);
    if (gri.typegameresult() == ::natashapb::SCATTER_LEFT) {
      auto pCurGRI = pSpinResult->add_lstgri();
      // gri.set_win(0);
      // gri.set_realwin(0);

      pCurGRI->CopyFrom(gri);

      pSpinResult->set_fgnums(TLOD_DEFAULT_FREENUMS);
      pSpinResult->set_realfgnums(TLOD_DEFAULT_FREENUMS);
    }

    // check all line payout
    TLODCountAllLine(*pSpinResult, sb3x5, m_lines, m_paytables,
                     pGameCtrl->freespin().bet());

    pSpinResult->set_awardmul(pUGMI->cascadinginfo().turnnums() + 3);
    pSpinResult->set_realwin(pSpinResult->win() * pSpinResult->awardmul());

    return ::natashapb::OK;
  }

  // procSpinResult - proc spin result
  virtual ::natashapb::CODE procSpinResult(
      ::natashapb::UserGameModInfo* pUGMI,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::RandomResult* pRandomResult, UserInfo* pUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pUGMI != NULL);

    // if need start free game
    if (pSpinResult->realfgnums() > 0) {
      auto fi = pUGMI->mutable_freeinfo();
      fi->set_lastnums(fi->lastnums() + pSpinResult->realfgnums());
    }

    // if respin
    if (pSpinResult->realwin() > 0) {
      pUGMI->mutable_cascadinginfo()->set_curbet(pGameCtrl->spin().bet());
      pUGMI->mutable_cascadinginfo()->set_turnwin(
          pUGMI->cascadinginfo().turnwin() + pSpinResult->realwin());
      pUGMI->mutable_cascadinginfo()->set_turnnums(
          pUGMI->cascadinginfo().turnnums() + 1);
      pUGMI->mutable_cascadinginfo()->set_isend(false);

      pUGMI->mutable_freeinfo()->set_totalwin(pUGMI->freeinfo().totalwin() +
                                              pSpinResult->realwin());

      // auto gamectrlid = pUser->mutable_gamectrlid();
      // if (gamectrlid->baseid() > 0) {
      //   return ::natashapb::OK;
      // }
    } else {
      pUGMI->mutable_cascadinginfo()->set_isend(true);
    }

#ifdef NATASHA_SERVER
    this->addRespinHistory(pUGMI, pSpinResult->realwin(), pSpinResult->win(),
                           pSpinResult->awardmul(), false);
#endif  // NATASHA_SERVER

    return ::natashapb::OK;
  }

  // onSpinStart - on spin start
  virtual ::natashapb::CODE onSpinStart(::natashapb::UserGameModInfo* pUGMI,
                                        const ::natashapb::GameCtrl* pGameCtrl,
                                        UserInfo* pUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pUGMI != NULL);
    assert(pUGMI->has_freeinfo());
    // assert(pUser->freeinfo().lastnums() > 0);
    assert(pUGMI->has_cascadinginfo());

    if (pUGMI->freeinfo().lastnums() == 0) {
      assert(!pUGMI->cascadinginfo().isend());
    }

    bool isrespin = true;
    if (pUGMI->cascadinginfo().isend()) {
      pUGMI->mutable_cascadinginfo()->set_turnnums(0);
      pUGMI->mutable_cascadinginfo()->set_turnwin(0);

      this->clearRespinHistory(pUGMI);

      isrespin = false;
    }

    if (!isrespin) {
      auto fi = pUGMI->mutable_freeinfo();
      fi->set_lastnums(fi->lastnums() - 1);
      fi->set_curnums(fi->curnums() + 1);
    }

    return ::natashapb::OK;
  }

  // onSpinEnd - on spin end
  virtual ::natashapb::CODE onSpinEnd(::natashapb::UserGameModInfo* pUGMI,
                                      const ::natashapb::GameCtrl* pGameCtrl,
                                      ::natashapb::SpinResult* pSpinResult,
                                      ::natashapb::RandomResult* pRandomResult,
                                      UserInfo* pUser) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pUGMI != NULL);

#ifdef NATASHA_SERVER
    pSpinResult->mutable_spin()->CopyFrom(pGameCtrl->freespin());
#endif  // NATASHA_SERVER

    if (pSpinResult->lstgri_size() > 0) {
      auto sb = pUGMI->mutable_symbolblock();
      auto sb3x5 = sb->mutable_sb3x5();

      sb3x5->CopyFrom(pRandomResult->scrr3x5().symbolblock().sb3x5());
      removeBlock3X5WithGameResult(sb3x5, pSpinResult);
      cascadeBlock3X5(sb3x5);

      // printSymbolBlock3X5("onSpinEnd", sb3x5, TLOD_SYMBOL_MAPPING);
    } else {
      auto scrr = pRandomResult->mutable_scrr3x5();
      scrr->set_reelsindex(-1);
    }

    return ::natashapb::OK;
  }

  // buildSpinResultSymbolBlock - build spin result's symbol block
  virtual ::natashapb::CODE buildSpinResultSymbolBlock(
      ::natashapb::SpinResult* pSpinResult,
      const ::natashapb::UserGameModInfo* pUGMI,
      const ::natashapb::GameCtrl* pGameCtrl,
      const ::natashapb::RandomResult* pRandomResult, const UserInfo* pUser,
      const void* pCfg) {
    assert(pUser != NULL);
    assert(pGameCtrl != NULL);
    assert(pSpinResult != NULL);
    assert(pRandomResult != NULL);
    assert(pUGMI != NULL);
#ifdef NATASHA_SERVER
    auto sb = pSpinResult->mutable_symbolblock();
    auto sb3x5 = sb->mutable_sb3x5();
    sb3x5->CopyFrom(pRandomResult->scrr3x5().symbolblock().sb3x5());
#endif  // NATASHA_SERVER
    return ::natashapb::OK;
  }

  // clearUGMI - clear UserGameModInfo
  //           - 初始化用户游戏模块数据
  virtual ::natashapb::CODE clearUGMI(::natashapb::UserGameModInfo* pUser) {
    clearUGMI_BaseCascadingInfo(*pUser->mutable_cascadinginfo(),
                                TLOD_DEFAULT_PAY_LINES, TLOD_DEFAULT_TIMES);

    clearUGMI_GameCtrlID(*pUser->mutable_gamectrlid());

    pUser->set_ver(TLOD_FG_UGMI_VER);

    return ::natashapb::OK;
  }

  // isCompeleted - isCompeleted
  //              - 游戏特殊状态是否已结束
  virtual bool isCompeleted(::natashapb::UserGameModInfo* pUser) {
    if (isIn(pUser)) {
      return false;
    }

    return true;
  }

 protected:
  StaticCascadingReels3X5& m_reels;
  Paytables3X5& m_paytables;
  Lines3X5& m_lines;
  BetList& m_lstBet;
};

}  // namespace natasha

#endif  // __NATASHA_TLOD_FREEGAME_H__