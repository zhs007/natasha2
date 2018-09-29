#ifndef __NATASHA_PAYTABLES_H__
#define __NATASHA_PAYTABLES_H__

#include <assert.h>
#include <map>
#include "array.h"
#include "utils.h"

namespace natasha {

template <int Width, typename SymbolType, typename MappingType,
          typename MoneyType>
class Paytables {
  typedef StaticArray<Width, MoneyType> StaticArrayTypeT;
  typedef std::map<SymbolType, StaticArrayTypeT> Map;
  typedef std::pair<SymbolType, StaticArrayTypeT> Pair;
  typedef Mapping<MappingType, int> MappingT;

 public:
  Paytables() {}

 public:
  void setSymbolPayout(SymbolType symbol, int index, MoneyType payout) {
    if (!m_map.has(symbol)) {
      Pair p;
      m_map.insert(p);
    }

    m_map[symbol].set(index, payout);
  }

  MoneyType getSymbolPayout(SymbolType symbol, int index) {
    assset(m_map.has(symbol));

    return m_map[symbol].get(index);
  }

  void setMapping(MappingType src, int index) { m_mapping.set(src, index); }

  MoneyType getSymbolPayoutWithMapping(SymbolType symbol, MappingType mapping) {
    assset(m_map.has(symbol));

    return m_map[symbol].get(m_mapping.get(mapping));
  }

 protected:
  MappingT m_mapping;
  Map m_map;
};

}  // namespace natasha

#endif  // __NATASHA_PAYTABLES_H__