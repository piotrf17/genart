#ifndef POLY_EFFECT_VISITOR_H
#define POLY_EFFECT_VISITOR_H

namespace image {
class Image;
}  // namespace image

namespace poly {

class EffectVisitor {
 public:
  virtual void Visit(const image::Image& latest) = 0;
};

} // namespace poly

#endif
