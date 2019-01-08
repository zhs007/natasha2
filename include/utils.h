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

typedef int64_t CtrlID;

const char SYMBOL_MAPPING[] = "wabcdefghijklmnopqrstuvxyz";

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

// setGameCtrlID
void setGameCtrlID(::natashapb::GameCtrlID& dest,
                   const ::natashapb::GameCtrlID& parent, CtrlID curCtrlID,
                   ::natashapb::GAMEMODTYPE curGameMod);

// printSymbolBlock3X5
void printSymbolBlock3X5(const char* str,
                         const ::natashapb::SymbolBlock3X5* pSB,
                         const char* strMapping);

// printRandomResult
void printRandomResult(const char* str,
                       const ::natashapb::RandomResult* pRandomResult,
                       const char* strMapping);

}  // namespace natasha

#endif  // __NATASHA_UTILS_H__