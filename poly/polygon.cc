#include "poly/polygon.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include "poly/polygon_mutator.h"

namespace poly {

namespace {

template <typename T>
T Random() {
  return rand() / static_cast<T>(RAND_MAX);
}

int Random(int range) {
  return static_cast<int>(rand() / static_cast<double>(RAND_MAX) * range);
}

}  // namespace

Polygon::Polygon() {
}

Polygon::~Polygon() {
}

void Polygon::Randomize() {
  // TODO(piotrf): make params
  const double h = 0.4;
  
  // Random color.
  MutateColor();
  
  // Random, smallish triangle.
  double ox = 0.5 * h + (1.0 - h) * Random<double>();
  double oy = 0.5 * h + (1.0 - h) * Random<double>();
  for (int i = 0; i < 3; ++i) {
    Point p;
    p.x = ox + h * (Random<double>() - 0.5);
    p.y = oy + h * (Random<double>() - 0.5);
    // Assert that I did the above calculations right.
    assert(p.x >= 0.0 && p.x <= 1.0);
    assert(p.y >= 0.0 && p.y <= 1.0);
    points_.push_back(p);
  }

  // Require that the triangle has CCW orientation, flipping
  // 2 points if necessary.
  const Point& a = points_[0];
  const Point& b = points_[1];
  const Point& c = points_[2];
  double det = (a.x - b.x) * (c.y - b.y) - (a.y - b.y) * (c.x - b.x);
  if (det > 0) {
    std::swap(points_[1], points_[2]);
  }
}

void Polygon::Mutate(const PolygonMutator& mutator) {
  mutator(this, &this->points_);
}

void Polygon::MutateColor() {
  const double alpha_0 = 0.3, alpha_range = 0.3;
  
  // Random color.
  color_.r = Random<float>();
  color_.g = Random<float>();
  color_.b = Random<float>();
  color_.a = alpha_0 + alpha_range * Random<float>();
}

// Compute the angle between sides at point i of the polygon.  This only
// reports the correct angle if the polygon is convex.
double Polygon::ComputeInteriorAngle(int i) const {
  int next_i = (i + 1) % (points_.size());
  int last_i = (i - 1 + points_.size()) % (points_.size());
  const Point& a = points_[last_i];
  const Point& b = points_[i];
  const Point& c = points_[next_i];
  double dot = (a.x - b.x) * (c.x - b.x) + (a.y - b.y) * (c.y - b.y);
  double mag_ab = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
  double mag_bc = sqrt((c.x - b.x) * (c.x - b.x) + (c.y - b.y) * (c.y - b.y));
  return acos(dot / (mag_ab * mag_bc));
}

// A polygon is convex if all interior angles < 180 degrees.
bool Polygon::Convex() const {
  for (unsigned int i = 0; i < points_.size(); ++i) {
    int next_i = (i + 1) % (points_.size());
    int last_i = (i - 1 + points_.size()) % (points_.size());
    const Point& a = points_[last_i];
    const Point& b = points_[i];
    const Point& c = points_[next_i];
    // Check the turning direction of the side.  Since we have CCW orientation,
    // each subsequent point should be turning CCW, i.e.
    // (b - a) x (c - b) > 0
    double det = (b.x - a.x) * (c.y - b.y) - (b.y - a.y) * (c.x - b.x);
    if (det < 0) {
      return false;
    }
  }
  return true;
}

}  // namespace poly
