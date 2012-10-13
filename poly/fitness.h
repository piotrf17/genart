// Abstract class for fitness functions.

#ifndef GENART_POLY_FITNESS_H
#define GENART_POLY_FITNESS_H

namespace image {
  class Image;
}  // namespace image

namespace poly {

class Fitness {
 public:
  Fitness() {}
  virtual ~Fitness() {}

  virtual double Evaluate(const image::Image* ref_image,
                          const image::Image* new_image) const = 0;
};

}  // namespace poly

#endif
