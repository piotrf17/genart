#ifndef GENART_CORE_EFFECT_VISITOR_H_
#define GENART_CORE_EFFECT_VISITOR_H_

namespace genart {
namespace image {
class Image;
}  // namespace image

namespace core {

class EffectVisitor {
 public:
  virtual void Visit(const image::Image& latest) = 0;
};

}  // namespace core
}  // namespace genart

#endif   // GENART_CORE_EFFECT_VISITOR_H_
