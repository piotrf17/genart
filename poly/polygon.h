#ifndef GENART_POLY_POLYGON_H
#define GENART_POLY_POLYGON_H

#include <vector>

#include "util/point.h"

// I don't know what idiot is #define'ing this, but I need this line
// for my function to compile :(
#undef Convex

namespace poly {

class PolygonMutator;
  
struct RGBA {
  RGBA() {}
  RGBA(double _r, double _g, double _b, double _a)
      : r(_r), g(_g), b(_b), a(_a) {}
  double r, g, b, a;
};
  
class Polygon {
 public:
  Polygon();
  Polygon(const std::vector<Point> points, const RGBA& color);
  ~Polygon();

  // Make a random triangle with CCW orientation, with size parameter h.
  // This only sets the geometry, the color is still undefined so you must
  // call MutateColor to get a truly random initial polygon.
  void Randomize(double h);

  // Apply a mutation to the polygon.  Currently these mutations can only
  // change the geometry of the polygon.
  void Mutate(const PolygonMutator& mutator);

  // Change the color of the polygon to a random RGBA triple with alpha
  // range as specified by min_alpha and max_alpha.
  void MutateColor(double min_alpha, double max_alpha);

  // Compute the angle between sides at point i of the polygon.  This only
  // reports the correct angle if the polygon is convex.
  double ComputeInteriorAngle(int i) const;

  // Check if the polygon is convex.  This test only works for simple polygons,
  // and requires that the polygon have CCW orientation.
  bool Convex() const;

  // Accessors.
  // TODO(piotrf): these methods seem strange now, why not just expose
  // a const view of the vector?
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
  
  std::vector<Point> points_;
  RGBA color_;
};

} // namespace poly

#endif
