#include <algorithm>
#include <cassert>
#include <iostream>
#include <cmath>
#include <cstdlib>

#include "poly/polygon.h"

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
  const double h = 0.2;
  
  // Random color.
  MutateColor();
  
  // Random, smallish triangle.
  double ox = h + (1.0 - 2 * h) * Random<double>();
  double oy = h + (1.0 - 2 * h) * Random<double>();
  for (int i = 0; i < 3; ++i) {
    Point p;
    p.x = ox + h * (Random<double>() - 0.5);
    p.y = oy + h * (Random<double>() - 0.5);
    // Assert that I did the above calculations right.
    assert(p.x > 0.0 && p.x < 1.0);
    assert(p.y > 0.0 && p.y < 1.0);
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

// Add a random point to the polygon, maintining the convexity of the polygon
// and also clamping to remain within the window.
// TODO(piotrf): clamping may result in polygons with lots of points along
// a wall, perhaps check if the side lies along a wall before adding a point.
void Polygon::AddPoint() {
  // TODO(piotrf): check for max number of points.
  if (points_.size() > 10) {
    return;
  }

  // Choose a random side and compute the max vertex angle.
  int side = Random(points_.size());
  double max_angle = std::max(
      ComputeInteriorAngle(side),
      ComputeInteriorAngle((side + 1) % points_.size()));

  // Calculate an offset vector from our chosen side midpoint based on the
  // convexity constraint.
  const Point& a = points_[side];
  const Point& b = points_[(side + 1) % (points_.size())];
  double offset = Random<double>();
  if (max_angle > M_PI / 2.0) {
    offset *= std::min(tan(M_PI - max_angle), 1.0);
  }
  double off_x = 0.5 * (b.y - a.y) * offset;
  double off_y = -0.5 * (b.x - a.x) * offset;

  // Add this new point to our list.
  Point c;
  c.x = std::min(std::max(0.5 * (a.x + b.x) + off_x, 0.0), 1.0);
  c.y = std::min(std::max(0.5 * (a.y + b.y) + off_y, 0.0), 1.0);
  points_.insert(points_.begin() + side + 1, c);
}

// Removing a random point from a convex polygon keeps it convex.
void Polygon::DeletePoint() {
  if (points_.size() <= 3) {
    return;
  }
  int i = Random(points_.size());
  points_.erase(points_.begin() + i);
}

void Polygon::MovePoint() {
  int i = Random(points_.size());
  for (int iter = 0; iter < 10; ++iter) {
    double dx = 0.05 * Random<double>();
    double dy = 0.05 * Random<double>();

    points_[i].x += dx;
    points_[i].y += dy;

    if (Convex()) {
      break;
    }

    points_[i].x -= dx;
    points_[i].y -= dy;    
  }

  // A triangle can stay convex with a point movement that flips the
  // orientation of the outside.  Ensure that we're CCW oriented.
  if (points_.size() == 3) {
    const Point& a = points_[0];
    const Point& b = points_[1];
    const Point& c = points_[2];
    double det = (a.x - b.x) * (c.y - b.y) - (a.y - b.y) * (c.x - b.x);
    if (det > 0) {
      std::swap(points_[1], points_[2]);
    }
  }
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
