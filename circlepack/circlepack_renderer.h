#ifndef GENART_CIRCLEPACK_CIRLEPACK_RENDERER_H_
#define GENART_CIRCLEPACK_CIRLEPACK_RENDERER_H_

#include <memory>

namespace genart {

namespace image {
class Image;
}  // namespace image

namespace util {
class OfflineGLContext;
}  // namespace util
  
namespace circlepack {

class CirclepackGenome;

class OfflineCirclepackRenderer {
 public:
  OfflineCirclepackRenderer(int width, int height);
  ~OfflineCirclepackRenderer();

  std::unique_ptr<image::Image> Render(const CirclepackGenome& genome);

 private:
  std::unique_ptr<util::OfflineGLContext> context_;
};

}  // namespcae circlepack
}  // namespace genart

#endif  // GENART_CIRCLEPACK_CIRLEPACK_RENDERER_H_
