// TODO(piotrf): move this to image subdirectory

#ifndef GENART_CORE_FITNESS_L2_WITH_FOCII_H_
#define GENART_CORE_FITNESS_L2_WITH_FOCII_H_

#include "fitness.h"

namespace genart {
  
namespace image {
class Image;
}  // namespace image
  
namespace core {

class FitnessL2WithFocii : public Fitness {
public:
  FitnessL2WithFocii();
  virtual ~FitnessL2WithFocii();
  
  virtual double Evaluate(const image::Image& ref_image,
                          const image::Image& new_image) const;
};

}  // namespace core
}  // namespace genart

#endif  // GENART_POLY_FITNESS_L2_WITH_FOCII_H_
