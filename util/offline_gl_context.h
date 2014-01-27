// OfflineGLContext encapsulates all the state required for offline OpenGL
// rendering on a 2D canvas.
#ifndef GENART_UTIL_OFFLINE_GL_CONTEXT_H_
#define GENART_UTIL_OFFLINE_GL_CONTEXT_H_

#include <memory>

namespace genart {

namespace image {
class Image;
}  // namespace image
  
namespace util {

struct OfflineGLContextState;

class OfflineGLContext {
 public:
  // Create a new context of the given width and height.
  OfflineGLContext(int width, int height);
  ~OfflineGLContext();

  // If using the context in a multithreaded environment, call Acquire
  // before calling anything in OpenGL.
  bool Acquire();

  // Returns what is currently draw as an Image.
  std::unique_ptr<image::Image> GetImage() const;

 private:
  bool Init();
  
  int width_, height_;
  std::unique_ptr<OfflineGLContextState> state_;
};

}  // namespace util
}  // namespace genart

#endif  // GENART_UTIL_OFFLINE_GL_CONTEXT_H_
