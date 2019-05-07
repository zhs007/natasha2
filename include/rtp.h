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

  ::natashapb::GameModuleRTP* newModule(int maxNums, int maxSymbol) {
    auto gm = new ::natashapb::GameModuleRTP();

    for (int i = 0; i < maxSymbol; ++i) {
      auto ls = gm->add_symbols();
      for (int j = 0; j < maxNums; ++j) {
        ls->add_lst();
      }
    }

    return gm;
  }

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
      auto gm = newModule(maxNums, maxSymbol);

      ::google::protobuf::MapPair< ::std::string, ::natashapb::GameModuleRTP> p(
          gmname, *gm);

      gamemodules->insert(p);
    }
  }

  void addPayout(::natashapb::GAMEMODTYPE module, SymbolType s, int nums,
                 MoneyType payout) {
    auto gmname = getGameModuleName(module);
    auto gamemodules = rtp.mutable_gamemodules();
    auto gmit = gamemodules->find(gmname);
    if (gmit != gamemodules->end()) {
      addPayout2Module(gmit->second, s, nums, payout);
    }

    rtp.set_totalwin(rtp.totalwin() + payout);
  }

  void addBet(MoneyType bet) { rtp.set_totalbet(rtp.totalbet() + bet); }

  void output() {
    printf("RTP is %lld / %lld\n", rtp.totalwin(), rtp.totalbet());
  }
};

}  // namespace natasha

#endif  // __NATASHA_RTP_H__