#include <stdio.h>
#include "../tlod/tlod.h"

int main() {
  natasha::TLOD tlod;

  auto c = tlod.init();
  if (c != natashapb::OK) {
    printf("init fail(%d)!\n", c);
  }

  auto pUGI = new ::natashapb::UserGameLogicInfo();
  c = tlod.userComeIn(pUGI);
  if (c != natashapb::OK) {
    printf("userComeIn fail(%d)!\n", c);
  }

  for (int i = 0; i < 100; ++i) {
    auto pGameCtrl = new ::natashapb::GameCtrl();
    auto spin = pGameCtrl->mutable_spin();

    spin->set_bet(1);
    spin->set_lines(natasha::TLOD_DEFAULT_PAY_LINES);
    spin->set_times(natasha::TLOD_DEFAULT_TIMES);

    c = tlod.gameCtrl(pGameCtrl, pUGI, i + 1);
    if (c != natashapb::OK) {
      printf("gameCtrl fail(%d)!\n", c);
    }

    delete pGameCtrl;
  }

  printf("end!\n");

  return 0;
}