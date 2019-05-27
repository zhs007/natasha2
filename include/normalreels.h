#ifndef __NATASHA_NORMALREELS_H__
#define __NATASHA_NORMALREELS_H__

#include <assert.h>
#include <stdio.h>

namespace natasha {

template <typename SymbolType, int Width, int Height>
class NormalReels {
 public:
 public:
  NormalReels() {
    for (int i = 0; i < Width; ++i) {
      m_reels[i] = NULL;
      m_reelsLength[i] = 0;
    }
  }
  virtual ~NormalReels() { release(); }

 public:
  void clear() { release(); }

  bool isEmpty() const { return m_reelsLength[0] == 0; }

 public:
  void release() {
    for (int i = 0; i < Width; ++i) {
      if (m_reels[i] != NULL) {
        free(m_reels[i]);

        m_reels[i] = NULL;
        m_reelsLength[i] = 0;
      }
    }
  }

  void resetReelsLength(int x, int length) {
    assert(x >= 0 && x < Width);
    assert(length > 0 && length >= Height);

    if (m_reels[x] != NULL) {
      free(m_reels[x]);
    }

    m_reels[x] = (SymbolType*)malloc(sizeof(SymbolType) * length);
    m_reelsLength[x] = length;
  }

  void setReels(int x, int y, SymbolType s) {
    assert(x >= 0 && x < Width);
    assert(m_reels[x] != NULL);
    assert(y >= 0 && y < m_reelsLength[x]);

    m_reels[x][y] = s;
  }

  int getReelsLength(int x) const {
    assert(x >= 0 && x < Width);

    return m_reelsLength[x];
  }

  SymbolType getSymbol(int x, int y) const {
    assert(x >= 0 && x < Width);
    assert(m_reels[x] != NULL);
    assert(y >= -m_reelsLength[x] && y < 2 * m_reelsLength[x]);

    if (y < 0) {
      y += m_reelsLength[x];
    } else if (y >= m_reelsLength[x]) {
      y -= m_reelsLength[x];
    }

    return m_reels[x][y];
  }

  SymbolType getSymbolEx(int x, int& y) const {
    assert(x >= 0 && x < Width);
    assert(m_reels[x] != NULL);
    assert(y >= -m_reelsLength[x] && y < 2 * m_reelsLength[x]);

    if (y < 0) {
      y += m_reelsLength[x];
    } else if (y >= m_reelsLength[x]) {
      y -= m_reelsLength[x];
    }

    return m_reels[x][y];
  }

  int countSymbol(int x, SymbolType s) const {
    assert(x >= 0 && x < Width);
    assert(m_reels[x] != NULL);

    int nums = 0;
    for (int y = 0; y < m_reelsLength[x]; ++y) {
      if (m_reels[x][y] == s) {
        ++nums;
      }
    }

    return nums;
  }

  void output(const char* str, SymbolType maxSymbol) const {
    printf("%s\n", str);

    for (SymbolType s = 0; s <= maxSymbol; ++s) {
      printf("symbol %d ", s);

      for (int x = 0; x < Width; ++x) {
        printf("%d ", this->countSymbol(x, s));
      }

      printf("\n");
    }
  }

 protected:
  SymbolType* m_reels[Width];
  int m_reelsLength[Width];
};

}  // namespace natasha

#endif  // __NATASHA_NORMALREELS_H__