#include <algorithm>
#include <cstdlib>

#include "poly/polygon.h"

namespace poly {

namespace {
/*
// Returns true if the two line segments (p1, p2) and (p3, p4) intersect,
// false otherwise.
static bool LineIntersect(Point p1, Point p2, Point p3, Point p4) {
  double ua, ub;
  int det;

  det = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);
  if (det == 0) {
    return false;
  }

  ua = ((p4.x-p3.x)*(p1.y-p3.y)-(p4.y-p3.y)*(p1.x-p3.x))/((float)det);
  ub = ((p2.x-p1.x)*(p1.y-p3.y)-(p2.y-p1.y)*(p1.x-p3.x))/((float)det);
  
  return  (ua >= 0 && ua <= 1 && ub >=0 && ub <= 1);
}
*/
template <typename T>
T Random() {
  return rand() / static_cast<T>(RAND_MAX);
}

}  // namespace

Polygon::Polygon() {
}

Polygon::~Polygon() {
}

void Polygon::Randomize() {
  // TODO(piotrf): make params
  const double alpha_0 = 0.3, alpha_range = 0.3;
  const double h = 0.1;
  
  // Random color.
  color_.r = Random<float>();
  color_.g = Random<float>();
  color_.b = Random<float>();
  color_.a = alpha_0 + alpha_range * Random<float>();

  // Random, smallish triangle.
  double ox = Random<double>();
  double oy = Random<double>();
  for (int i = 0; i < 3; ++i) {
    Point p;
    p.x = ox + h * (Random<double>() - 0.5);
    p.y = oy + h * (Random<double>() - 0.5);
    // Clamp to our window.
    p.x = std::min(std::max(p.x, 0.0), 1.0);
    p.y = std::min(std::max(p.y, 0.0), 1.0);
    points_.push_back(p);
  }
}

}  // namespace poly
