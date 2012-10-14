#ifndef GENART_POLY_POLYGON_H
#define GENART_POLY_POLYGON_H

#include <vector>

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

  std::vector<Point>::const_iterator begin() const {
    return points_.begin();
  }

  std::vector<Point>::const_iterator end() const {
    return points_.end();
  }

  const RGBA& color() const {
    return color_;
  }
  
 private:

  std::vector<Point> points_;
  RGBA color_;
};

} // namespace poly

#endif
