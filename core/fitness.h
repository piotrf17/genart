// Abstract class for fitness functions.

#ifndef GENART_CORE_FITNESS_H_
#define GENART_CORE_FITNESS_H_

namespace genart {
namespace image {
  class Image;
}  // namespace image

namespace core {

class Fitness {
 public:
  Fitness() {}
  virtual ~Fitness() {}

  virtual double Evaluate(const image::Image& ref_image,
                          const image::Image& new_image) const = 0;
};

}  // namespace core
}  // namespace genart

#endif  // GENART_CORE_FITNESS_H_
