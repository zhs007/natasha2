#include <stdio.h>
#include "../tlod/tlod.h"

int main() {
  natasha::TLOD tlod;

  printf("%ld\n", time(NULL));

  auto c = tlod.init("./csv");
  if (c != natashapb::OK) {
    printf("init fail(%d)!\n", c);
  }

  auto pUGI = new ::natashapb::UserGameLogicInfo();
  c = tlod.userComeIn(pUGI);
  if (c != natashapb::OK) {
    printf("userComeIn fail(%d)!\n", c);
  }

  int64_t totalbet = 0;
  int64_t totalpay = 0;

  auto pGameCtrl = new ::natashapb::GameCtrl();
  int64_t ctrlid = 1;

  for (int i = 0; i <= 1000000; ++ctrlid) {
    if (pUGI->nextgamemodtype() == natashapb::BASE_GAME) {
      auto spin = pGameCtrl->mutable_spin();
      spin->set_bet(1);
      spin->set_lines(natasha::TLOD_DEFAULT_PAY_LINES);
      spin->set_times(natasha::TLOD_DEFAULT_TIMES);
    } else if (pUGI->nextgamemodtype() == natashapb::FREE_GAME) {
      auto freespin = pGameCtrl->mutable_freespin();
      freespin->set_bet(1);
      freespin->set_lines(natasha::TLOD_DEFAULT_PAY_LINES);
      freespin->set_times(natasha::TLOD_DEFAULT_TIMES);
    }

    pGameCtrl->set_ctrlid(ctrlid);

    c = tlod.gameCtrl(pGameCtrl, pUGI);
    if (c != natashapb::OK) {
      printf("gameCtrl fail(%d)!\n", c);
    }

    if (pUGI->iscompleted()) {
      ++i;
    }
  }

  delete pGameCtrl;

  printf("%ld\n", time(NULL));

  printf("end!\n");

  return 0;
}