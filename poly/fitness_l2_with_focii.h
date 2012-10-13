#ifndef GENART_POLY_FITNESS_L2_WITH_FOCII_H
#define GENART_POLY_FITNESS_L2_WITH_FOCII_H

#include "fitness.h"

namespace image {
  class Image;
};

namespace poly {

class FitnessL2WithFocii {
public:
  FitnessL2WithFocii();
  virtual ~FitnessL2WithFocii();
  
  virtual double Evaluate(const image::Image* ref_image,
                          const image::Image* new_image) const;
};

}  // namespace poly

#endif
