#ifndef GENART_POLY_POLYGON_H
#define GENART_POLY_POLYGON_H

#include <vector>

// I don't know what idiot is #define'ing this, but I need this line
// for my function to compile :(
#undef Convex

namespace poly {

struct Point {
  double x, y;
};

struct RGBA {
  double r, g, b, a;
};
  
class Polygon {
 public:
  Polygon();
  ~Polygon();

  void Randomize();

  // Possible mutations.
  void AddPoint();
  void DeletePoint();
  void MovePoint();
  void MutateColor();
  
  std::vector<Point>::const_iterator begin() const {
    return points_.begin();
  }
  std::vector<Point>::const_iterator end() const {
    return points_.end();
  }
  int num_points() const {
    return points_.size();
  }
  const RGBA& color() const {
    return color_;
  }
  
 private:
  double ComputeInteriorAngle(int i) const;
  bool Convex() const;
  
  std::vector<Point> points_;
  RGBA color_;
};

} // namespace poly

#endif
