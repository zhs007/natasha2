#ifndef __NATASHA_RTP_H__
#define __NATASHA_RTP_H__

#include <assert.h>
#include <map>
#include <vector>
#include "../protoc/base.pb.h"
#include "array.h"
#include "symbolblock.h"
#include "utils.h"

namespace natasha {

struct RTP {
  ::natashapb::RTP rtp;

  RTP() { clear(); }

  void clear() { rtp.Clear(); }

  void addPayout2Module(::natashapb::GameModuleRTP& gm, SymbolType s, int nums,
                        MoneyType payout) {
    if (s >= 0 && s < gm.symbols_size()) {
      auto cs = gm.symbols(s);
      if (nums > 0 && nums <= cs.lst_size()) {
        auto ccs = cs.lst(nums - 1);
        ccs.set_totalwin(ccs.totalwin() + payout);
        ccs.set_winnums(ccs.winnums() + 1);
      }
    }
  }

  void addModule(::natashapb::GAMEMODTYPE module, int maxNums, int maxSymbol) {
    auto gmname = getGameModuleName(module);
    auto gamemodules = rtp.mutable_gamemodules();
    auto gmit = gamemodules->find(gmname);
    if (gmit == gamemodules->end()) {
      auto gm = _newModule(maxNums, maxSymbol);

      ::google::protobuf::MapPair< ::std::string, ::natashapb::GameModuleRTP> p(
          gmname, *gm);

      gamemodules->insert(p);

      delete gm;
    }
  }

  void initModuleBonus(::natashapb::GAMEMODTYPE module, const char* bonusName,
                       int maxNums) {
    auto gmname = getGameModuleName(module);
    auto gamemodules = rtp.mutable_gamemodules();
    auto gmit = gamemodules->find(gmname);
    if (gmit != gamemodules->end()) {
      auto bonus = _newBonus(maxNums);

      ::google::protobuf::MapPair< ::std::string, ::natashapb::BonusRTPList> p(
          bonusName, *bonus);

      gmit->second.mutable_bonus()->insert(p);
    }
  }

  void addBonus(::natashapb::GAMEMODTYPE module, const char* bonusName,
                int bonusIndex, MoneyType payout) {
    auto gmname = getGameModuleName(module);
    auto gamemodules = rtp.mutable_gamemodules();
    auto gmit = gamemodules->find(gmname);
    if (gmit != gamemodules->end()) {
      auto bonusit = gmit->second.mutable_bonus()->find(bonusName);
      if (bonusit != gmit->second.mutable_bonus()->end()) {
        assert(bonusIndex >= 0 && bonusIndex < bonusit->second.lst_size());

        auto currtp = bonusit->second.mutable_lst(bonusIndex);

        currtp->set_winnums(currtp->winnums() + 1);
        currtp->set_totalwin(currtp->totalwin() + payout);
        currtp->set_realwin(currtp->realwin() + payout);
      }
    }
  }

  void addPayout(MoneyType payout) {
    rtp.set_totalwin(rtp.totalwin() + payout);
  }

  void addSymbolPayout(::natashapb::GAMEMODTYPE module, SymbolType s, int nums,
                       MoneyType payout) {
    auto gmname = getGameModuleName(module);
    auto gamemodules = rtp.mutable_gamemodules();
    auto gmit = gamemodules->find(gmname);
    if (gmit != gamemodules->end()) {
      addPayout2Module(gmit->second, s, nums, payout);

      gmit->second.set_totalwin(gmit->second.totalwin() + payout);
      gmit->second.set_winnums(gmit->second.winnums() + 1);
    }
  }

  void addBet(::natashapb::GAMEMODTYPE module, MoneyType bet) {
    if (bet > 0) {
      rtp.set_totalbet(rtp.totalbet() + bet);
      rtp.set_spinnums(rtp.spinnums() + 1);
    }

    auto gmname = getGameModuleName(module);
    auto gamemodules = rtp.mutable_gamemodules();
    auto gmit = gamemodules->find(gmname);
    if (gmit != gamemodules->end()) {
      gmit->second.set_spinnums(gmit->second.spinnums() + 1);
    }
  }

  void output() {
    printf("RTP is %.2f(%lld / %lld)\n",
           100.f * rtp.totalwin() / rtp.totalbet(), rtp.totalwin(),
           rtp.totalbet());

    outputGameModule(::natashapb::BASE_GAME);
    outputGameModule(::natashapb::FREE_GAME);
  }

 protected:
  ::natashapb::GameModuleRTP* _newModule(int maxNums, int maxSymbol) {
    auto gm = new ::natashapb::GameModuleRTP();

    for (int i = 0; i < maxSymbol; ++i) {
      auto ls = gm->add_symbols();
      for (int j = 0; j < maxNums; ++j) {
        ls->add_lst();
      }
    }

    return gm;
  }

  ::natashapb::BonusRTPList* _newBonus(int maxNums) {
    auto bonus = new ::natashapb::BonusRTPList();

    for (int i = 0; i < maxNums; ++i) {
      auto ls = bonus->add_lst();
    }

    return bonus;
  }

  void outputGameModule(::natashapb::GAMEMODTYPE module) {
    auto gmname = getGameModuleName(module);
    auto gamemodules = rtp.mutable_gamemodules();
    auto gmit = gamemodules->find(gmname);
    if (gmit != gamemodules->end()) {
      printf("%s RTP is %.2f(%lld / %lld)\n", gmname,
             100.f * gmit->second.totalwin() / rtp.totalbet(),
             gmit->second.totalwin(), rtp.totalbet());
    }
  }
};

}  // namespace natasha

#endif  // __NATASHA_RTP_H__