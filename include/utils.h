#ifndef __NATASA_UTILS_H__
#define __NATASA_UTILS_H__

#include <assert.h>
#include <map>

namespace natasa {

template <typename SrcValueType, typename DestValueType>
class Mapping {
 public:
  void set(SrcValueType src, DestValueType dest) { m_map[src] = dest; }

  DestValueType get(SrcValueType src) { return m_map[src]; }

 protected:
  std::map<SrcValueType, DestValueType> m_map;
};

}  // namespace natasa

#endif  // __NATASA_UTILS_H__