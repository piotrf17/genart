#ifndef GENART_POLY_POLYGON_H
#define GENART_POLY_POLYGON_H

#include <vector>

namespace poly {

struct Point {
  int x, y;
};

struct RGBA {
  unsigned char r, g, b, a;
};
  
class Polygon {
 public:
  Polygon();
  ~Polygon();

  void Randomize();
  
 private:

  std::vector<Point> points_;
  RGBA color_;
};

} // namespace poly

#endif
