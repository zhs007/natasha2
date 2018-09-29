#ifndef __NATASHA_GAMERESULT_H__
#define __NATASHA_GAMERESULT_H__

#include <assert.h>
#include <map>
#include <vector>
#include "array.h"
#include "symbolblock.h"
#include "utils.h"

namespace natasha {

enum GameResultType {
  GRT_NULL = 0,
  GRT_LINE_LEFT,
  GRT_LINE_RIGHT,
  GRT_SCATTER_LEFT,
  GRT_SCATTER_RIGHT,
  GRT_SCATTER_LEFT_CONSEQUENT,
  GRT_SCATTER_RIGHT_CONSEQUENT
};

template <typename MoneyType, typename SymbolType, typename PositionType,
          typename GRIExInfo>
struct GameResultInfo {
  typedef Position2D<PositionType> Position2DT;
  typedef std::vector<Position2DT> Position2DList;
  typedef std::vector<SymbolType> SymbolList;

  GameResultType type;
  SymbolType symbol;
  Position2DList lstPos;
  SymbolList lstSymbol;
  MoneyType win;
  MoneyType realWin;

  GRIExInfo griex;

  GameResultInfo() { clear(); }

  void clear() {
    type = GRT_NULL;

    lstPos.clear();
    lstSymbol.clear();

    win = 0;
    realWin = 0;

    griex.clear();
  }

  void add(PositionType x, PositionType y, SymbolType s) {
    Position2DT p(x, y);
    lstPos.push_back(p);

    lstSymbol.push_back(s);
  }
};

template <typename MoneyType, typename SymbolType, typename PositionType>
struct GameResultInfo<MoneyType, SymbolType, PositionType, NullType> {
  typedef Position2D<PositionType> Position2DT;
  typedef std::vector<Position2DT> Position2DList;
  typedef std::vector<SymbolType> SymbolList;

  GameResultType type;
  SymbolType symbol;
  Position2DList lstPos;
  SymbolList lstSymbol;
  MoneyType win;
  MoneyType realWin;

  GameResultInfo() { clear(); }

  void clear() {
    type = GRT_NULL;

    lstPos.clear();
    lstSymbol.clear();

    win = 0;
    realWin = 0;
  }

  void add(PositionType x, PositionType y, SymbolType s) {
    Position2DT p(x, y);
    lstPos.push_back(p);

    lstSymbol.push_back(s);
  }
};

template <typename MoneyType, typename SymbolType, typename PositionType,
          typename GRIExInfo, typename ExInfo>
struct GameResult {
  typedef GameResultInfo<MoneyType, SymbolType, PositionType, GRIExInfo>
      GameResultInfoT;
  typedef std::vector<GameResultInfoT> GameResultInfoList;

  GameResultInfoList lstGameResult;

  MoneyType win;
  MoneyType realWin;

  ExInfo exinfo;

  void clear() {
    lstGameResult.clear();

    win = 0;
    realWin = 0;

    exinfo.clear();
  }

  void addResult(GameResultInfoT& gri) {
    lstGameResult.push_back(gri);

    win += gri.win;
    realWin += gri.realWin;

    exinfo.onAddResult(gri);
  }
};

template <typename MoneyType, typename SymbolType, typename PositionType,
          typename GRIExInfo>
struct GameResult<MoneyType, SymbolType, PositionType, GRIExInfo, NullType> {
  typedef GameResultInfo<MoneyType, SymbolType, PositionType, GRIExInfo>
      GameResultInfoT;
  typedef std::vector<GameResultInfoT> GameResultInfoList;

  GameResultInfoList lstGameResult;

  MoneyType win;
  MoneyType realWin;

  void clear() {
    lstGameResult.clear();

    win = 0;
    realWin = 0;
  }

  void addResult(GameResultInfoT& gri) {
    lstGameResult.push_back(gri);

    win += gri.win;
    realWin += gri.realWin;
  }
};

}  // namespace natasha

#endif  // __NATASHA_GAMERESULT_H__