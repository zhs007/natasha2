#ifndef __NATASHA_STATICCASCADINGREELS_H__
#define __NATASHA_STATICCASCADINGREELS_H__

#include <assert.h>
#include <map>
#include "../protoc/base.pb.h"
#include "reels.h"

namespace natasha {

template <typename SymbolType, int Width, int Height>
class StaticCascadingReels : public BaseReels<SymbolType, Width, Height> {
 public:
  typedef SymbolBlock<SymbolType, Width, Height> SymbolBlockT;
  typedef BaseReels<SymbolType, Width, Height> BaseReelsT;
  typedef std::vector<SymbolBlockT*> SymbolBlockData;
  typedef std::vector<SymbolBlockData*> SymbolBlockDataList;

 public:
  StaticCascadingReels() : m_maxDownNums(-1) {}
  virtual ~StaticCascadingReels() {}

 public:
  virtual void random(::natashapb::RandomResult* pRandomResult,
                      SymbolBlockT& symbolBlock) {
    assert(pRandomResult != NULL);

    ::natashapb::StaticCascadingRandomResult msg;
    pRandomResult->info().UnpackTo(&msg);

    auto ri = msg.reelsindex();
    auto dn = msg.downnums();

    if (ri < 0) {
    } else {
      assert(dn < m_maxDownNums);
    }
  }

  virtual void clear() {
    for (int i = 0; i < m_lst.size(); ++i) {
      SymbolBlockData* pSBD = m_lst[i];
      for (int j = 0; j < pSBD->size(); ++j) {
        SymbolBlockT* pSB = (*pSBD)[j];
        delete pSB;
      }

      pSBD->clear();

      delete pSBD;
    }

    m_lst.clear();
    m_maxDownNums = -1;
  }

  virtual bool isEmpty() const { return m_lst.empty(); }

 public:
  void newRow() { m_lst.push_back(new SymbolBlockData()); }

  void newColumn(int row, const SymbolBlockT& sb) {
    assert(row >= 0 && row < m_lst.size());

    SymbolBlockData* pSBD = m_lst[row];
    assert(pSBD != NULL);
    assert(pSBD->size() < m_maxDownNums);

    SymbolBlockT* pSB = new SymbolBlockT();
    pSB->cloneWith(sb);

    pSBD->push_back(pSB);
  }

  int getLength() const { return m_lst.size(); }

  void setMaxDownNums(int maxDownNums) { m_maxDownNums = maxDownNums; }

 protected:
  int m_maxDownNums;
  SymbolBlockDataList m_lst;
};

}  // namespace natasha

#endif  // __NATASHA_STATICCASCADINGREELS_H__