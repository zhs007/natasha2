#include "../include/staticcascadingreels3x5.h"
#include "../include/fortuna.h"
#include "../include/symbolblock2.h"

namespace natasha {

void StaticCascadingReels3X5::randomNew(
    ::natashapb::StaticCascadingRandomResult3X5* pSCRR) {
  uint32_t cr = randomScale(m_lst.size());
  assert(cr >= 0 && cr < m_lst.size());

  SymbolBlockData* pSBD = m_lst[cr];
  assert(pSBD->size() > 0);

  // printSymbolBlock3X5("randomNew", pSBD->at(0), SYMBOL_MAPPING);

  pSCRR->set_reelsindex(cr);
  pSCRR->set_downnums(0);

  ::natashapb::SymbolBlock* pSB = pSCRR->mutable_symbolblock();
  ::natashapb::SymbolBlock3X5* pSB35 = pSB->mutable_sb3x5();
  pSB35->CopyFrom(*(pSBD->at(0)));
}

void StaticCascadingReels3X5::fill(
    ::natashapb::StaticCascadingRandomResult3X5* pSCRR,
    const ::natashapb::SymbolBlock3X5* pLastSB) {
  int32_t cr = pSCRR->reelsindex();
  assert(cr >= 0 && cr < m_lst.size());

  int32_t dn = pSCRR->downnums();
  SymbolBlockData* pSBD = m_lst[cr];
  assert(dn >= 0 && dn + 1 < pSBD->size());

  pSCRR->set_downnums(dn + 1);

  ::natashapb::SymbolBlock* pSB = pSCRR->mutable_symbolblock();
  ::natashapb::SymbolBlock3X5* pSB35 = pSB->mutable_sb3x5();
  pSB35->CopyFrom(*pLastSB);

  const ::natashapb::SymbolBlock3X5* pNewSB = pSBD->at(dn + 1);

  for (int y = 0; y < 3; ++y) {
    for (int x = 0; x < 5; ++x) {
      if (getSymbolBlock3X5(pSB35, x, y) == -1) {
        setSymbolBlock3X5(pSB35, x, y, getSymbolBlock3X5(pNewSB, x, y));
      }
    }
  }
}

void StaticCascadingReels3X5::random(
    ::natashapb::RandomResult* pRandomResult,
    const ::natashapb::UserGameModInfo* pUGMI) {
  assert(pRandomResult != NULL);
  // assert(pUGMI->symbolblock().has_sb3x5());

  if (!pRandomResult->has_scrr3x5()) {
    ::natashapb::StaticCascadingRandomResult3X5* pSCRR =
        pRandomResult->mutable_scrr3x5();

    randomNew(pSCRR);
  } else {
    const ::natashapb::StaticCascadingRandomResult3X5& scrr =
        pRandomResult->scrr3x5();
    if (scrr.reelsindex() < 0) {
      ::natashapb::StaticCascadingRandomResult3X5* pSCRR =
          pRandomResult->mutable_scrr3x5();

      randomNew(pSCRR);
    } else {
      ::natashapb::StaticCascadingRandomResult3X5* pSCRR =
          pRandomResult->mutable_scrr3x5();

      fill(pSCRR, &(pUGMI->symbolblock().sb3x5()));
    }
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