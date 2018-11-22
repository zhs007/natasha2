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
  // tlod.

  printf("end!\n");

  return 0;
}