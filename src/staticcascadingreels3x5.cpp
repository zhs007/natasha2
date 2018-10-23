#include "../include/staticcascadingreels3x5.h"
#include "../include/fortuna.h"

namespace natasha {

void StaticCascadingReels3X5::randomNew(
    ::natashapb::StaticCascadingRandomResult3X5* pSCRR) {
  pSCRR->set_reelsindex(randomScale(m_lst.size()));
}

void StaticCascadingReels3X5::random(::natashapb::RandomResult* pRandomResult) {
  assert(pRandomResult != NULL);

  if (!pRandomResult->has_retstaticcascading3x5()) {
    ::natashapb::StaticCascadingRandomResult3X5* pSCRR =
        pRandomResult->mutable_retstaticcascading3x5();

    randomNew(pSCRR);
  } else {
    const ::natashapb::StaticCascadingRandomResult3X5& scrr =
        pRandomResult->retstaticcascading3x5();
    if (scrr.reelsindex() < 0) {
      ::natashapb::StaticCascadingRandomResult3X5* pSCRR =
          pRandomResult->mutable_retstaticcascading3x5();

      randomNew(pSCRR);
    } else {
    }
  }

  ::natashapb::StaticCascadingRandomResult3X5* pSCRR =
      pRandomResult->mutable_retstaticcascading3x5();

  ::natashapb::StaticCascadingRandomResult3X5 msg;
  pRandomResult->info().UnpackTo(&msg);

  auto ri = msg.reelsindex();
  auto dn = msg.downnums();

  if (ri < 0) {
  } else {
    assert(dn < m_maxDownNums);
  }
}

void StaticCascadingReels3X5::clear() {
  for (int i = 0; i < m_lst.size(); ++i) {
    SymbolBlockData* pSBD = m_lst[i];
    for (int j = 0; j < pSBD->size(); ++j) {
      ::natashapb::SymbolBlock3X5* pSB = (*pSBD)[j];
      delete pSB;
    }

    pSBD->clear();

    delete pSBD;
  }

  m_lst.clear();
  m_maxDownNums = -1;
}

void StaticCascadingReels3X5::newColumn(int row,
                                        const ::natashapb::SymbolBlock3X5& sb) {
  assert(row >= 0 && row < m_lst.size());

  SymbolBlockData* pSBD = m_lst[row];
  assert(pSBD != NULL);
  assert(pSBD->size() < m_maxDownNums);

  ::natashapb::SymbolBlock3X5* pSB = new ::natashapb::SymbolBlock3X5();
  pSB->CopyFrom(sb);

  pSBD->push_back(pSB);
}

}  // namespace natasha