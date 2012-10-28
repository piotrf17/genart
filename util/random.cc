#include "util/random.h"

#include <cstdlib>

namespace util {

int Random::Integer(int range) {
  return static_cast<int>(rand() / static_cast<double>(RAND_MAX) * range);
}

int Random::Integer(int a, int b) {
  return a + Integer(b - a);
}

double Random::Double(double range) {
  return range * rand() / static_cast<double>(RAND_MAX);
}

double Random::Double(double a, double b) {
  return a + Double(b - a);
}

float Random::Float(float range) {
  return range * rand() / static_cast<float>(RAND_MAX);
}

float Random::Float(float a, float b) {
  return a + Float(b - a);
}

}  // util
