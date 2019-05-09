#ifndef __NATASHA_LOGICWAYS_H__
#define __NATASHA_LOGICWAYS_H__

#include <assert.h>
#include <map>
#include "../protoc/base.pb.h"
#include "array.h"
#include "gameresult.h"
#include "lines.h"
#include "paytables.h"
#include "symbolblock2.h"
#include "utils.h"

namespace natasha {

template <typename MoneyType, typename SymbolType, int Height,
          class SymbolBlockT, typename GameCfgT>
bool _countFullWays5_Left(
    ::natashapb::SpinResult& sr, SymbolType s, int y0, const SymbolBlockT& arr,
    const Paytables<5, SymbolType, int, MoneyType>& paytables, MoneyType bet) {
  typedef GameResultInfo<MoneyType, SymbolType, int, NullType> GameResultInfoT;
  typedef Paytables<5, SymbolType, int, MoneyType> PaytablesT;

  // printf("_countFullWays5_Left %d-%d\n", y0, s);

  int winnums = 0;
  int len = 1;
  for (int y1 = 0; y1 < Height; ++y1) {
    // printf("_countFullWays5_Left y1:%d-%d\n", y1,
    //        getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 1, y1));

    if (GameCfgT::isSameSymbol_OnLine(
            getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 1, y1), s)) {
      len = 2;

      for (int y2 = 0; y2 < Height; ++y2) {
        if (GameCfgT::isSameSymbol_OnLine(
                getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 2, y2), s)) {
          len = 3;

          for (int y3 = 0; y3 < Height; ++y3) {
            if (GameCfgT::isSameSymbol_OnLine(
                    getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 3, y3), s)) {
              len = 4;

              for (int y4 = 0; y4 < Height; ++y4) {
                if (GameCfgT::isSameSymbol_OnLine(
                        getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 4, y4),
                        s)) {
                  len = 5;

                  MoneyType p = paytables.getSymbolPayout(s, 5 - 1);
                  if (p > 0) {
                    auto curgri = sr.add_lstgri();
                    winnums++;

                    curgri->set_mul(p);
                    curgri->set_symbol(s);
                    curgri->set_typegameresult(::natashapb::WAY_LEFT);
                    curgri->set_win(bet * p);
                    curgri->set_realwin(curgri->win());

                    sr.set_win(sr.win() + curgri->win());

                    auto pos0 = curgri->add_lstpos();
                    pos0->set_x(0);
                    pos0->set_y(y0);

                    auto pos1 = curgri->add_lstpos();
                    pos1->set_x(1);
                    pos1->set_y(y1);

                    auto pos2 = curgri->add_lstpos();
                    pos2->set_x(2);
                    pos2->set_y(y2);

                    auto pos3 = curgri->add_lstpos();
                    pos3->set_x(3);
                    pos3->set_y(y3);

                    auto pos4 = curgri->add_lstpos();
                    pos4->set_x(4);
                    pos4->set_y(y4);

                    curgri->add_lstsymbol(s);
                    curgri->add_lstsymbol(
                        getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 1, y1));
                    curgri->add_lstsymbol(
                        getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 2, y2));
                    curgri->add_lstsymbol(
                        getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 3, y3));
                    curgri->add_lstsymbol(
                        getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 4, y4));
                  }  // if s[5] can payout
                }    // if s5 == s
              }      // for y4

              if (len == 4) {
                MoneyType p = paytables.getSymbolPayout(s, 4 - 1);
                if (p > 0) {
                  auto curgri = sr.add_lstgri();
                  winnums++;

                  curgri->set_mul(p);
                  curgri->set_symbol(s);
                  curgri->set_typegameresult(::natashapb::WAY_LEFT);
                  curgri->set_win(bet * p);
                  curgri->set_realwin(curgri->win());

                  sr.set_win(sr.win() + curgri->win());

                  auto pos0 = curgri->add_lstpos();
                  pos0->set_x(0);
                  pos0->set_y(y0);

                  auto pos1 = curgri->add_lstpos();
                  pos1->set_x(1);
                  pos1->set_y(y1);

                  auto pos2 = curgri->add_lstpos();
                  pos2->set_x(2);
                  pos2->set_y(y2);

                  auto pos3 = curgri->add_lstpos();
                  pos3->set_x(3);
                  pos3->set_y(y3);

                  curgri->add_lstsymbol(s);
                  curgri->add_lstsymbol(
                      getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 1, y1));
                  curgri->add_lstsymbol(
                      getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 2, y2));
                  curgri->add_lstsymbol(
                      getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 3, y3));
                }  // if s[4] can payout
              }    // if len == 4
            }      // if s3 == s

          }  // for y3

          if (len == 3) {
            MoneyType p = paytables.getSymbolPayout(s, 3 - 1);
            if (p > 0) {
              auto curgri = sr.add_lstgri();
              winnums++;

              curgri->set_mul(p);
              curgri->set_symbol(s);
              curgri->set_typegameresult(::natashapb::WAY_LEFT);
              curgri->set_win(bet * p);
              curgri->set_realwin(curgri->win());

              sr.set_win(sr.win() + curgri->win());

              auto pos0 = curgri->add_lstpos();
              pos0->set_x(0);
              pos0->set_y(y0);

              auto pos1 = curgri->add_lstpos();
              pos1->set_x(1);
              pos1->set_y(y1);

              auto pos2 = curgri->add_lstpos();
              pos2->set_x(2);
              pos2->set_y(y2);

              curgri->add_lstsymbol(s);
              curgri->add_lstsymbol(
                  getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 1, y1));
              curgri->add_lstsymbol(
                  getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 2, y2));
            }  // if s[3] can payout
          }    // if len == 3
        }      // if s2 == s
      }        // for y2

      if (len == 2) {
        MoneyType p = paytables.getSymbolPayout(s, 2 - 1);
        if (p > 0) {
          auto curgri = sr.add_lstgri();
          winnums++;

          curgri->set_mul(p);
          curgri->set_symbol(s);
          curgri->set_typegameresult(::natashapb::WAY_LEFT);
          curgri->set_win(bet * p);
          curgri->set_realwin(curgri->win());

          sr.set_win(sr.win() + curgri->win());

          auto pos0 = curgri->add_lstpos();
          pos0->set_x(0);
          pos0->set_y(y0);

          auto pos1 = curgri->add_lstpos();
          pos1->set_x(1);
          pos1->set_y(y1);

          curgri->add_lstsymbol(s);
          curgri->add_lstsymbol(
              getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 1, y1));
        }  // if s[2] can payout
      }    // len == 2
    }      // if s1 == s
  }        // for y1

  return winnums > 0;
}

template <typename MoneyType, typename SymbolType, int Height,
          typename SymbolBlockT, typename GameCfgT>
void countFullWays5_Left(
    ::natashapb::SpinResult& sr, const SymbolBlockT& arr,
    const Paytables<5, SymbolType, int, MoneyType>& paytables, MoneyType bet) {
  typedef GameResult<MoneyType, SymbolType, int, NullType, NullType>
      GameResultT;
  typedef Paytables<5, SymbolType, int, MoneyType> PaytablesT;
  typedef GameResultInfo<MoneyType, SymbolType, int, NullType> GameResultInfoT;

  for (int i = 0; i < Height; ++i) {
    bool iswin = _countFullWays5_Left<MoneyType, SymbolType, Height,
                                      SymbolBlockT, GameCfgT>(
        sr, getSymbolBlock<SymbolBlockT, 5, Height>(&arr, 0, i), i, arr,
        paytables, bet);

    // printf("countFullWays5_Left %lld %d\n", sr.win(), sr.lstgri_size());
  }
}

}  // namespace natasha

#endif  // __NATASHA_LOGICWAYS_H__