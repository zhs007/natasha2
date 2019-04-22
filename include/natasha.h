#ifndef __NATASHA_NATASA_H__
#define __NATASHA_NATASA_H__

#define NATASHA_MONEYTYPE_INT64
#define NATASHA_SYMBOLTYPE_INT

#include <vector>
#include "array.h"
#include "gamecfg.h"
#include "gamecode.h"
#include "logicline2.h"
#include "logicscatter2.h"
#include "utils.h"

// #define NATASHA_SERVER
// #define NATASHA_COUNTRTP

namespace natasha {

typedef std::vector<MoneyType> BetList;

// max nums for makeInitScenario
const int MAX_NUMS_MAKEINITIALSCENARIO = 200;

}  // namespace natasha

#endif  // __NATASA_NATASHA_H__