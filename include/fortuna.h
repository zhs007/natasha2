#ifndef __NATASHA_FORTUNA_H__
#define __NATASHA_FORTUNA_H__

#include <assert.h>

namespace natasha {

// random - return uint32 number
uint32_t random();

// randomScale - return [0, max)
uint32_t randomScale(uint32_t max);

}  // namespace natasha

#endif  // __NATASHA_FORTUNA_H__