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
  typedef typename Map::iterator MapIter;
  typedef std::pair<SymbolType, StaticArrayTypeT> Pair;
  typedef Mapping<MappingType, int> MappingT;

 public:
  Paytables() {}

 public:
  void setSymbolPayout(SymbolType symbol, int index, MoneyType payout) {
    MapIter it = m_map.find(symbol);
    if (it == m_map.end()) {
      Pair p;
      m_map.insert(p);
    }

    m_map[symbol].set(index, payout);
  }

  MoneyType getSymbolPayout(SymbolType symbol, int index) const {
    auto it = m_map.find(symbol);
    assert(it != m_map.end());

    return it->second.get(index);
  }

  void setMapping(MappingType src, int index) { m_mapping.set(src, index); }

  MoneyType getSymbolPayoutWithMapping(SymbolType symbol, MappingType mapping) {
    assert(m_map.has(symbol));

    return m_map[symbol].get(m_mapping.get(mapping));
  }

  void clear() {
    m_mapping.clear();
    m_map.clear();
  }

  bool isEmpty() const { return m_map.empty(); }

 protected:
  MappingT m_mapping;
  Map m_map;
};

}  // namespace natasha

#endif  // __NATASHA_PAYTABLES_H__