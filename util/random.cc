#include "util/random.h"

#include <cstdlib>

namespace util {

int Random::Integer(int range) {
  return static_cast<int>(rand() / static_cast<double>(RAND_MAX) * range);
}

double Random::Double(double range) {
  return range * rand() / static_cast<double>(RAND_MAX);
}

float Random::Float(float range) {
  return range * rand() / static_cast<float>(RAND_MAX);
}

}  // util
