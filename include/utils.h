#ifndef __NATASHA_UTILS_H__
#define __NATASHA_UTILS_H__

#include <assert.h>
#include <map>
#include "../protoc/base.pb.h"

namespace natasha {

#ifdef NATASHA_SYMBOLTYPE_INT
typedef int32_t SymbolType;
#else
typedef int SymbolType;
#endif

#ifdef NATASHA_MONEYTYPE_INT64
typedef int64_t MoneyType;
#else
typedef float MoneyType;
#endif

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

// clearGameResultInfo
void clearGameResultInfo(::natashapb::GameResultInfo& gri);
// clearSpinResult
void clearSpinResult(::natashapb::SpinResult& sr);

}  // namespace natasha

#endif  // __NATASHA_UTILS_H__