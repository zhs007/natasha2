#include "../include/game5x3.h"
#include <fstream>
#include <streambuf>
#include <string>
#include "../include/csvfile.h"

namespace natasha {

// loadPaytables5X3 - load paytables.csv
void loadPaytables5X3(const char* fn, Paytables5X3& paytables) {
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

// loadLines5X3 - load lines.csv
void loadLines5X3(const char* fn, Lines5X3& lines) {
  lines.clear();

  CSVFile csv;

  if (csv.load(fn)) {
    for (int i = 0; i < csv.getLength(); ++i) {
      int r1 = std::stoi(csv.get(i, "R1"));
      int r2 = std::stoi(csv.get(i, "R2"));
      int r3 = std::stoi(csv.get(i, "R3"));
      int r4 = std::stoi(csv.get(i, "R4"));
      int r5 = std::stoi(csv.get(i, "R5"));

      LineInfo5X3 li;

      li.set(0, r1);
      li.set(1, r2);
      li.set(2, r3);
      li.set(3, r4);
      li.set(4, r5);

      lines.addLine(li);
    }
  }
}

// loadStaticCascadingReels5X3 - StaticCascadingReels.csv
void loadStaticCascadingReels5X3(FileNameList& lstfn,
                                 StaticCascadingReels5X3& scr) {
  scr.clear();

  scr.setMaxDownNums(lstfn.size());

  for (int i = 0; i < lstfn.size(); ++i) {
    CSVFile csv;

    if (csv.load(lstfn[i].c_str())) {
      if (i > 0) {
        assert(csv.getLength() == scr.getLength());
      }

      for (int r = 0; r < csv.getLength(); ++r) {
        SymbolBlock5X3 sb;

        for (int y = 0; y < 3; ++y) {
          for (int x = 0; x < 5; ++x) {
            int s = std::stoi(csv.get(i, std::to_string(y * 5 + x).c_str()));

            sb.set(x, y, s);
          }
        }

        if (i == 0) {
          scr.newRow();
        }

        scr.newColumn(r, sb);
      }
    }
  }
}

}  // namespace natasha