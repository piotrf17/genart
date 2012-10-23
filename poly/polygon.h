#ifndef GENART_POLY_POLYGON_H
#define GENART_POLY_POLYGON_H

#include <vector>

// I don't know what idiot is #define'ing this, but I need this line
// for my function to compile :(
#undef Convex

namespace poly {

class PolygonMutator;
  
struct Point {
  double x, y;
  Point() : x(0.0), y(0.0) {}
  Point(double ox, double oy) : x(ox), y(oy) {}
};

struct RGBA {
  double r, g, b, a;
};
  
class Polygon {
 public:
  Polygon();
  ~Polygon();

  void Randomize();

  void Mutate(const PolygonMutator& mutator);

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

  double ComputeInteriorAngle(int i) const;
  bool Convex() const;
  
 private:
  
  std::vector<Point> points_;
  RGBA color_;
};

} // namespace poly

#endif
