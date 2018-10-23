#include "../include/fortuna.h"
#include "../libfortuna/fortuna.h"

namespace natasha {

// random - return uint32 number
uint32_t random() {
  uint32_t cr = 0;
  fortuna_get_bytes(4, (uint8_t*)&cr);
  return cr;
}

// randomScale - return [0, max)
uint32_t randomScale(uint32_t max) {
  uint32_t cr = 0;
  uint64_t MAX_RANGE = ((uint64_t)1) << 32;
  uint64_t limit = MAX_RANGE - (MAX_RANGE % max);

  do {
    fortuna_get_bytes(4, (uint8*)&cr);
  } while (cr >= limit);

  return cr % max;
}

}  // namespace natasha