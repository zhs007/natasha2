#ifndef __NATASHA_CONFIG_H__
#define __NATASHA_CONFIG_H__

#include <assert.h>
#include "../protoc/base.pb.h"

namespace natasha {

int sumWeightConfig(const ::natashapb::WeightConfig& cfg);

int randWeightConfig(const ::natashapb::WeightConfig& cfg);

}  // namespace natasha

#endif  // __NATASHA_CONFIG_H__