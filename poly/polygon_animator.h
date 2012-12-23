// A class for creating interesting animations between two different
// polygon images.

#ifndef GENART_POLY_POLYGON_ANIMATOR_H
#define GENART_POLY_POLYGON_ANIMATOR_H

#include <vector>

namespace poly {

class AnimatedPolygonImage;
class Polygon;

class PolygonAnimator {
 public:

  static AnimatedPolygonImage* Animate(
      const std::vector<Polygon>& image1,
      const std::vector<Polygon>& image2);

};

}  // namespace poly

#endif
