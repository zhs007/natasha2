#ifndef __NATASHA_RTP_H__
#define __NATASHA_RTP_H__

#include <assert.h>
#include <map>
#include <vector>
#include "array.h"
#include "symbolblock.h"
#include "utils.h"

namespace natasha {

template <typename MoneyType, typename SymbolType>
struct SymbolPayoutStatistics {
  typedef std::vector<MoneyType> SymbolPayout;
  typedef std::vector<SymbolPayout*> SymbolPayoutList;

  SymbolPayoutList lstSymbols;

  SymbolPayoutStatistics() {}
  ~SymbolPayoutStatistics() { clear(); }

  void init(int maxNums, int maxSymbol) {
    for (int i = 0; i < maxSymbol; ++i) {
      lstSymbols.push_back(new SymbolPayout());

      for (int j = 0; j < maxNums; ++j) {
        lstSymbols[j]->push_back(0);
      }
    }
  }

  void clear() {
    for (auto it = lstSymbols.begin(); it != lstSymbols.end(); ++it) {
      delete *it;
    }

    lstSymbols.clear();
  }

  void addPayout(SymbolType s, int nums, MoneyType payout) {
    if (s >= 0 && s < lstSymbols.size()) {
      if (nums > 0 && nums <= (*lstSymbols[s]).size()) {
        (*lstSymbols[s])[nums - 1] += payout;
      }
    }
  }
};

template <typename MoneyType, typename SymbolType>
struct RTP {
  typedef SymbolPayoutStatistics<MoneyType, SymbolType> _SymbolPayoutStatistics;
  typedef std::map<::natashapb::GAMEMODTYPE, _SymbolPayoutStatistics*>
      ModuleMap;

  ModuleMap mapModule;
  MoneyType totalBet;

  RTP() { clear(); }

  void clear() {
    for (auto it = mapModule.begin(); it != mapModule.end(); ++it) {
      delete it->second;
    }

    mapModule.clear();

    totalBet = 0;
  }

  void addModule(::natashapb::GAMEMODTYPE module, int maxNums, int maxSymbol) {
    auto it = mapModule.find(module);
    if (it != mapModule.end()) {
      std::pair<::natashapb::GAMEMODTYPE, _SymbolPayoutStatistics*> p;

      p.first = module;
      p.second = new _SymbolPayoutStatistics();
      p.second->init(maxNums, maxSymbol);

      mapModule.insert(p);
    }
  }

  void addPayout(::natashapb::GAMEMODTYPE module, SymbolType s, int nums,
                 MoneyType payout) {
    auto it = mapModule.find(module);
    if (it != mapModule.end()) {
      it->second->addPayout(s, nums, payout);
    }
  }

  void addBet(MoneyType bet) { totalBet += bet; }
};

}  // namespace natasha

#endif  // __NATASHA_RTP_H__