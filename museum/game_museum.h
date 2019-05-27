#ifndef __NATASHA_GAME_MUSEUM_H__
#define __NATASHA_GAME_MUSEUM_H__

#include <assert.h>
#include <vector>
#include "../include/game3x5.h"
#include "../include/natasha.h"

namespace natasha {

const int MUSEUM_WIDTH = 5;
const int MUSEUM_HEIGHT = 3;

const int MUSEUM_DEFAULT_LINES = 243;
const int MUSEUM_DEFAULT_PAY_LINES = 30;
const int MUSEUM_DEFAULT_TIMES = 1;

// const int MUSEUM_DEFAULT_FREENUMS = 10;

const SymbolType MUSEUM_SYMBOL_W = 0;
const SymbolType MUSEUM_SYMBOL_S = 10;

const int MeseumMaxSymbols = 11;
const int MeseumMaxPayoutNums = 5;

const char MUSEUM_SYMBOL_MAPPING[] = " wabcdefghjs";

struct MuseumGameCfg {
  static bool isSameSymbol_OnLine(SymbolType s0, SymbolType s1) {
    if (s0 == MUSEUM_SYMBOL_S || s1 == MUSEUM_SYMBOL_S) {
      return false;
    }

    if (s0 == s1) {
      return true;
    }

    return s0 == MUSEUM_SYMBOL_W || s1 == MUSEUM_SYMBOL_W;
  }

  static bool isSameSymbol_wild_OnLine(SymbolType s, SymbolType& curws) {
    if (curws < 0) {
      if (s == MUSEUM_SYMBOL_W) {
        return true;
      }

      curws = s;

      return true;
    }

    return s == curws || s == MUSEUM_SYMBOL_W;
  }

  static bool isScatter(SymbolType s) { return s == MUSEUM_SYMBOL_S; }

  static bool isWild(SymbolType s) { return s == MUSEUM_SYMBOL_W; }

  static int getMaxScstterNums(SymbolType s) { return MUSEUM_WIDTH; }
};

auto const MuseumCountScatter =
    &countScatter_Left<MoneyType, SymbolType, MUSEUM_WIDTH, MUSEUM_HEIGHT,
                       ::natashapb::SymbolBlock3X5, MuseumGameCfg>;

auto const MuseumCountWays =
    &countFullWays5_Left<MoneyType, SymbolType, MUSEUM_HEIGHT,
                         ::natashapb::SymbolBlock3X5, MuseumGameCfg>;

// callback function in fill
static SymbolType museum_onfill(int x, int y, SymbolType s,
                                const ::natashapb::WeightConfig& weightcfg) {
  if (x == 0 || s == MUSEUM_SYMBOL_S) {
    return s;
  }

  auto cr = randWeightConfig(weightcfg);
  if (cr == 0) {
    return MUSEUM_SYMBOL_W;
  }

  return s;
}

template <::natashapb::GAMEMODTYPE GameModType>
class MuseumConfig {
 public:
  static int getBonusPrize(const ::natashapb::MuseumRTPConfig cfg,
                           int turnnums);

  static const ::natashapb::WeightConfig& getMysteryWild(
      const ::natashapb::MuseumRTPConfig cfg, int index);
};

template <>
class MuseumConfig<::natashapb::BASE_GAME> {
 public:
  static int getBonusPrize(const ::natashapb::MuseumRTPConfig cfg,
                           int turnnums) {
    if (turnnums >= cfg.bgbonusprize_size()) {
      turnnums = cfg.bgbonusprize_size() - 1;
    }

    return cfg.bgbonusprize(turnnums);
  }

  static const ::natashapb::WeightConfig& getMysteryWild(
      const ::natashapb::MuseumRTPConfig cfg, int index) {
    return cfg.bgmysterywild(index);
  }
};

template <>
class MuseumConfig<::natashapb::FREE_GAME> {
 public:
  static int getBonusPrize(const ::natashapb::MuseumRTPConfig cfg,
                           int turnnums) {
    if (turnnums >= cfg.fgbonusprize_size()) {
      turnnums = cfg.fgbonusprize_size() - 1;
    }

    return cfg.fgbonusprize(turnnums);
  }

  static const ::natashapb::WeightConfig& getMysteryWild(
      const ::natashapb::MuseumRTPConfig cfg, int index) {
    return cfg.fgmysterywild(index);
  }
};

// bomb
static void museum_bomb(::natashapb::SymbolBlock3X5& tmp,
                        const ::natashapb::SymbolBlock3X5& sb3x5, int x, int y,
                        ::natashapb::GameResultInfo* pGRI) {
  // printf("bomb %d %d\n", x, y);
  for (int cy = y - 1; cy <= y + 1; ++cy) {
    if (cy >= 0 && cy < MUSEUM_HEIGHT) {
      for (int cx = x - 1; cx <= x + 1; ++cx) {
        if (cx >= 0 && cx < MUSEUM_WIDTH) {
          auto ctmps = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                      MUSEUM_HEIGHT>(&tmp, cx, cy);
          if (ctmps == 0) {
            if (cx == x && cy == y) {
              auto cp = pGRI->add_lstpos();
              cp->set_x(cx);
              cp->set_y(cy);

              setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                             MUSEUM_HEIGHT>(&tmp, cx, cy, -1);
            } else {
              auto cs =
                  getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                 MUSEUM_HEIGHT>(&sb3x5, cx, cy);

              if (cs != MUSEUM_SYMBOL_W && cs != MUSEUM_SYMBOL_S) {
                auto cp = pGRI->add_lstpos();
                cp->set_x(cx);
                cp->set_y(cy);

                setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                               MUSEUM_HEIGHT>(&tmp, cx, cy, -1);
              }
            }
          }
        }
      }
    }
  }
}

template <::natashapb::GAMEMODTYPE GameModType>
MoneyType museum_procWildBomb(MoneyType bet,
                              const ::natashapb::MuseumRTPConfig& cfg,
                              const ::natashapb::UserGameModInfo* pUser,
                              ::natashapb::SpinResult* pSpinResult) {
  if (pSpinResult->specialtriggered() > 0) {
    ::natashapb::SymbolBlock3X5 tmp;
    auto sb3x5 = pSpinResult->symbolblock().sb3x5();
    auto pGRI = pSpinResult->add_lstgri();

    removeBlock3X5WithGameResult(&tmp, pSpinResult);

    for (int y = 0; y < MUSEUM_HEIGHT; ++y) {
      for (int x = 1; x < MUSEUM_WIDTH; ++x) {
        auto ctmps = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                    MUSEUM_HEIGHT>(&tmp, x, y);
        auto cs = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                                 MUSEUM_HEIGHT>(&sb3x5, x, y);
        if (cs == MUSEUM_SYMBOL_W) {
          museum_bomb(tmp, sb3x5, x, y, pGRI);
        }
      }
    }

    if (pGRI->lstpos_size() > 0) {
      auto turnnums = pUser->cascadinginfo().turnnums();

      pGRI->set_mul(MuseumConfig<GameModType>::getBonusPrize(cfg, turnnums));
      pGRI->set_typegameresult(::natashapb::SPECIAL);
      pGRI->set_win(bet * pGRI->mul() * pGRI->lstpos_size());
      pGRI->set_realwin(pGRI->win());

      return pGRI->realwin();
    } else {
      pSpinResult->mutable_lstgri()->RemoveLast();
    }
  }

  return 0;
};

template <::natashapb::GAMEMODTYPE GameModType>
int museum_randWArr(const ::natashapb::MuseumRTPConfig& cfg,
                    const ::natashapb::SymbolBlock3X5& srcsb3x5,
                    ::natashapb::SymbolBlock3X5* sb3x5) {
  int nums = 0;

  auto mwweight = MuseumConfig<GameModType>::getMysteryWild(cfg, 0);

  for (int y = 0; y < MUSEUM_HEIGHT; ++y) {
    setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH, MUSEUM_HEIGHT>(
        sb3x5, 0, y,
        getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                       MUSEUM_HEIGHT>(&srcsb3x5, 0, y));

    for (int x = 1; x < MUSEUM_WIDTH; ++x) {
      auto cs = getSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                               MUSEUM_HEIGHT>(&srcsb3x5, x, y);
      if (cs != MUSEUM_SYMBOL_S) {
        auto ci = randWeightConfig(mwweight);
        if (ci == 0) {
          setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH,
                         MUSEUM_HEIGHT>(sb3x5, x, y, MUSEUM_SYMBOL_W);

          nums++;

          continue;
        }
      }

      setSymbolBlock<::natashapb::SymbolBlock3X5, MUSEUM_WIDTH, MUSEUM_HEIGHT>(
          sb3x5, x, y, cs);
    }
  }

  return nums;
}

}  // namespace natasha

#endif  // __NATASHA_GAME_MUSEUM_H__