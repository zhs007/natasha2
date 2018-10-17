#ifndef __NATASHA_UTILS_H__
#define __NATASHA_UTILS_H__

#include <assert.h>
#include <map>

namespace natasha {

template <typename SrcValueType, typename DestValueType>
class Mapping {
 public:
  typedef std::map<SrcValueType, DestValueType> MapT;

 public:
  void set(SrcValueType src, DestValueType dest) { m_map[src] = dest; }

  DestValueType get(SrcValueType src) { return m_map[src]; }

  void clear() { m_map.clear(); }

 protected:
  MapT m_map;
};

template <typename PositionType>
struct Position2D {
  PositionType x, y;

  Position2D(PositionType x_, PositionType y_) {
    this->x = x_;
    this->y = y_;
  }
};

typedef struct {
} NullType;

typedef std::vector<std::string> FileNameList;

}  // namespace natasha

#endif  // __NATASHA_UTILS_H__