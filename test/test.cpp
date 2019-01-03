#include <stdio.h>
#include "../tlod/tlod.h"

int main() {
  natasha::TLOD tlod;

  bool bInit = tlod.init();
  if (!bInit) {
    printf("init fail!\n");
  }

  auto pUGI = new ::natashapb::UserGameLogicInfo();
  tlod.userComeIn(pUGI);

  for (int i = 0; i < 100; ++i) {
    auto pGameCtrl = new ::natashapb::GameCtrl();
    auto spin = pGameCtrl->mutable_spin();

    tlod.gameCtrl(pGameCtrl, pUGI);

    delete pGameCtrl;
  }

  printf("end!\n");

  return 0;
}