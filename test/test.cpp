#include <stdio.h>
#include "../tlod/tlod.h"

int main() {
  natasha::TLOD tlod;

  bool bInit = tlod.init();
  if (!bInit) {
    printf("init fail!\n");
  }

  printf("end!\n");

  return 0;
}