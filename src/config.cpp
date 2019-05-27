#include "../include/config.h"
#include "../include/fortuna.h"

namespace natasha {

int sumWeightConfig(const ::natashapb::WeightConfig& cfg) {
  int total = 0;

  for (int i = 0; i < cfg.weights_size(); ++i) {
    total += cfg.weights(i);
  }

  return total;
}

int randWeightConfig(const ::natashapb::WeightConfig& cfg) {
  auto cr = randomScale(cfg.totalweight());
  for (int i = 0; i < cfg.weights_size(); ++i) {
    if (cr < cfg.weights(i)) {
      return i;
    }

    cr -= cfg.weights(i);
  }

  return -1;
}

}  // namespace natasha