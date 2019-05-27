#include "../include/game3x5.h"
#include <fstream>
#include <streambuf>
#include <string>
#include "../include/csvfile.h"
#include "../include/fortuna.h"
#include "../include/symbolblock2.h"

namespace natasha {

// loadPaytables3X5 - load paytables.csv
void loadPaytables3X5(const char* fn, Paytables3X5& paytables) {
  paytables.clear();

  CSVFile csv;

  if (csv.load(fn)) {
    for (int i = 0; i < csv.getLength(); ++i) {
      int symbol = std::stoi(csv.get(i, "Code"));
      int p1 = std::stoi(csv.get(i, "X1"));
      int p2 = std::stoi(csv.get(i, "X2"));
      int p3 = std::stoi(csv.get(i, "X3"));
      int p4 = std::stoi(csv.get(i, "X4"));
      int p5 = std::stoi(csv.get(i, "X5"));

      paytables.setSymbolPayout(symbol, 0, p1);
      paytables.setSymbolPayout(symbol, 1, p2);
      paytables.setSymbolPayout(symbol, 2, p3);
      paytables.setSymbolPayout(symbol, 3, p4);
      paytables.setSymbolPayout(symbol, 4, p5);
    }
  }
}

// loadLines3X5 - load lines.csv
void loadLines3X5(const char* fn, Lines3X5& lines) {
  lines.clear();

  CSVFile csv;

  if (csv.load(fn)) {
    for (int i = 0; i < csv.getLength(); ++i) {
      int r1 = std::stoi(csv.get(i, "R1"));
      int r2 = std::stoi(csv.get(i, "R2"));
      int r3 = std::stoi(csv.get(i, "R3"));
      int r4 = std::stoi(csv.get(i, "R4"));
      int r5 = std::stoi(csv.get(i, "R5"));

      LineInfo3X5 li;

      li.set(0, r1);
      li.set(1, r2);
      li.set(2, r3);
      li.set(3, r4);
      li.set(4, r5);

      lines.addLine(li);
    }
  }
}

// loadStaticCascadingReels3X5 - StaticCascadingReels.csv
void loadStaticCascadingReels3X5(FileNameList& lstfn,
                                 StaticCascadingReels3X5& scr) {
  scr.clear();

  scr.setMaxDownNums(lstfn.size());

  for (int i = 0; i < lstfn.size(); ++i) {
    CSVFile csv;

    if (csv.load(lstfn[i].c_str())) {
      if (i > 0) {
        assert(csv.getLength() == scr.getLength());
      }

      for (int r = 0; r < csv.getLength(); ++r) {
        ::natashapb::SymbolBlock3X5 sb;

        for (int y = 0; y < 3; ++y) {
          for (int x = 0; x < 5; ++x) {
            int s = std::stoi(csv.get(r, std::to_string(y * 5 + x).c_str()));

            setSymbolBlock3X5(&sb, x, y, s);
          }
        }

        // printSymbolBlock3X5(&sb, SYMBOL_MAPPING);

        if (i == 0) {
          scr.newRow();
        }

        scr.newColumn(r, sb);
      }
    }
  }
}

// loadNormalReels - reelstrips.csv
void loadNormalReels3X5(const char* fn, NormalReels3X5& scr) {
  scr.clear();

  CSVFile csv;

  if (csv.load(fn)) {
#ifdef NATASHA_DEBUG
    printf("loadNormalReels3X5 %d\n", csv.getLength());
#endif  // NATASHA_DEBUG

    int len[] = {0, 0, 0, 0, 0};

    for (int i = 0; i < csv.getLength(); ++i) {
      int p1 = std::stoi(csv.get(i, "R1"));
      int p2 = std::stoi(csv.get(i, "R2"));
      int p3 = std::stoi(csv.get(i, "R3"));
      int p4 = std::stoi(csv.get(i, "R4"));
      int p5 = std::stoi(csv.get(i, "R5"));

      if (p1 < 0 && len[0] == 0) {
        len[0] = i;
      }

      if (p2 < 0 && len[1] == 0) {
        len[1] = i;
      }

      if (p3 < 0 && len[2] == 0) {
        len[2] = i;
      }

      if (p4 < 0 && len[3] == 0) {
        len[3] = i;
      }

      if (p5 < 0 && len[4] == 0) {
        len[4] = i;
      }
    }

    for (int i = 0; i < 5; ++i) {
      if (len[i] == 0) {
        len[i] = csv.getLength();
      }

#ifdef NATASHA_DEBUG
      printf("loadNormalReels3X5 reel%d is %d\n", i, len[i]);
#endif  // NATASHA_DEBUG

      scr.resetReelsLength(i, len[i]);
    }

    for (int i = 0; i < csv.getLength(); ++i) {
      int p1 = std::stoi(csv.get(i, "R1"));
      int p2 = std::stoi(csv.get(i, "R2"));
      int p3 = std::stoi(csv.get(i, "R3"));
      int p4 = std::stoi(csv.get(i, "R4"));
      int p5 = std::stoi(csv.get(i, "R5"));

      if (p1 >= 0 && i < len[0]) {
        scr.setReels(0, i, p1);
      }

      if (p2 >= 0 && i < len[1]) {
        scr.setReels(1, i, p2);
      }

      if (p3 >= 0 && i < len[2]) {
        scr.setReels(2, i, p3);
      }

      if (p4 >= 0 && i < len[3]) {
        scr.setReels(3, i, p4);
      }

      if (p5 >= 0 && i < len[4]) {
        scr.setReels(4, i, p5);
      }
    }
  }
}

// _randomNewReels3x5 - random with NormalReels
void _randomNewReels3x5(const NormalReels3X5& reels,
                        ::natashapb::NormalReelsRandomResult3X5* pNRRR) {
  pNRRR->clear_reelsindex();
  auto sb = pNRRR->mutable_symbolblock();
  auto sb3x5 = sb->mutable_sb3x5();

  for (int x = 0; x < 5; ++x) {
    // uint32_t cr = rand() % reels.getReelsLength(
    //                            x);  // randomScale(reels.getReelsLength(x));
    uint32_t cr = randomScale(reels.getReelsLength(x));
    pNRRR->add_reelsindex(cr);
  }

#ifdef NATASHA_DEBUG
  printf("_randomNewReels3x5 index: \n %d %d %d %d %d\n", pNRRR->reelsindex(0),
         pNRRR->reelsindex(1), pNRRR->reelsindex(2), pNRRR->reelsindex(3),
         pNRRR->reelsindex(4));
#endif  // NATASHA_DEBUG

  sb3x5->set_dat0_0(reels.getSymbol(0, pNRRR->reelsindex(0)));
  sb3x5->set_dat0_1(reels.getSymbol(1, pNRRR->reelsindex(1)));
  sb3x5->set_dat0_2(reels.getSymbol(2, pNRRR->reelsindex(2)));
  sb3x5->set_dat0_3(reels.getSymbol(3, pNRRR->reelsindex(3)));
  sb3x5->set_dat0_4(reels.getSymbol(4, pNRRR->reelsindex(4)));

  sb3x5->set_dat1_0(reels.getSymbol(0, pNRRR->reelsindex(0) + 1));
  sb3x5->set_dat1_1(reels.getSymbol(1, pNRRR->reelsindex(1) + 1));
  sb3x5->set_dat1_2(reels.getSymbol(2, pNRRR->reelsindex(2) + 1));
  sb3x5->set_dat1_3(reels.getSymbol(3, pNRRR->reelsindex(3) + 1));
  sb3x5->set_dat1_4(reels.getSymbol(4, pNRRR->reelsindex(4) + 1));

  sb3x5->set_dat2_0(reels.getSymbol(0, pNRRR->reelsindex(0) + 2));
  sb3x5->set_dat2_1(reels.getSymbol(1, pNRRR->reelsindex(1) + 2));
  sb3x5->set_dat2_2(reels.getSymbol(2, pNRRR->reelsindex(2) + 2));
  sb3x5->set_dat2_3(reels.getSymbol(3, pNRRR->reelsindex(3) + 2));
  sb3x5->set_dat2_4(reels.getSymbol(4, pNRRR->reelsindex(4) + 2));
}

// _fillReels3x5 - fill with NormalReels
void _fillReels3x5(const NormalReels3X5& reels,
                   const ::natashapb::SymbolBlock3X5& last3x5,
                   ::natashapb::NormalReelsRandomResult3X5* pNRRR,
                   FuncOnFillReels onfillreels) {
  assert(pNRRR->reelsindex_size() == 5);

  ::natashapb::SymbolBlock* pSB = pNRRR->mutable_symbolblock();
  ::natashapb::SymbolBlock3X5* pSB35 = pSB->mutable_sb3x5();

#ifdef NATASHA_DEBUG
  printf("_fillReels3x5 start index: \n %d %d %d %d %d\n", pNRRR->reelsindex(0),
         pNRRR->reelsindex(1), pNRRR->reelsindex(2), pNRRR->reelsindex(3),
         pNRRR->reelsindex(4));
#endif  // NATASHA_DEBUG

  for (int y = 2; y >= 0; --y) {
    for (int x = 0; x < 5; ++x) {
      auto curs = getSymbolBlock3X5(&last3x5, x, y);
      if (curs == -1) {
        auto ci = pNRRR->reelsindex(x) - 1;
        auto cs = reels.getSymbolEx(x, ci);

        cs = onfillreels(x, y, cs);

        setSymbolBlock3X5(pSB35, x, y, cs);

        pNRRR->set_reelsindex(x, ci);
      } else {
        setSymbolBlock3X5(pSB35, x, y, curs);
      }
    }
  }

#ifdef NATASHA_DEBUG
  printf("_fillReels3x5 start index: \n %d %d %d %d %d\n", pNRRR->reelsindex(0),
         pNRRR->reelsindex(1), pNRRR->reelsindex(2), pNRRR->reelsindex(3),
         pNRRR->reelsindex(4));
#endif  // NATASHA_DEBUG
}

// randomReels3x5 - random normal reels
void randomReels3x5(const NormalReels3X5& reels,
                    ::natashapb::RandomResult* pRandomResult,
                    const ::natashapb::UserGameModInfo* pUGMI,
                    FuncOnFillReels onfillreels) {
  assert(pRandomResult != NULL);

  if (pUGMI->cascadinginfo().isend()) {
    ::natashapb::NormalReelsRandomResult3X5* pNRRR =
        pRandomResult->mutable_nrrr3x5();

    _randomNewReels3x5(reels, pNRRR);

    return;
  }

  if (!pRandomResult->has_nrrr3x5()) {
    ::natashapb::NormalReelsRandomResult3X5* pNRRR =
        pRandomResult->mutable_nrrr3x5();

    _randomNewReels3x5(reels, pNRRR);
  } else {
    const ::natashapb::NormalReelsRandomResult3X5& nrrr =
        pRandomResult->nrrr3x5();
    if (nrrr.reelsindex_size() != 5) {
      ::natashapb::NormalReelsRandomResult3X5* pNRRR =
          pRandomResult->mutable_nrrr3x5();

      _randomNewReels3x5(reels, pNRRR);
    } else {
      ::natashapb::NormalReelsRandomResult3X5* pNRRR =
          pRandomResult->mutable_nrrr3x5();

      _fillReels3x5(reels, pUGMI->symbolblock().sb3x5(), pNRRR, onfillreels);
    }
  }
}

}  // namespace natasha