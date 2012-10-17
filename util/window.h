#ifndef GENART_UTIL_WINDOW_H
#define GENART_UTIL_WINDOW_H

#include <string>
#include <vector>

namespace boost {
  class thread;
}

namespace image {
  class Image;
}

namespace util {

struct GLWindow;

struct ImageWithPosition {
  const image::Image* image;
  int x, y;
};
  
class Window {
 public:
  Window(const std::string& title, int width, int height);
  ~Window();

  // TODO(piotrf): refactor into a base window class, and
  // a class that we can draw images on top of.
  void Reset();
  void DrawImage(const image::Image& image, int x, int y);

 private:
  // Construct or destroy an OpenGL window.
  // TODO(piotrf): these both could be factored out.
  void CreateGLWindow(const std::string& title,
                      int width, int height);
  void DestroyGLWindow();

  // Initialize the OpenGL scene.
  void InitGLScene();
  
  // Resize the OpenGL scene to fit the window size.
  void ResizeGLScene();

  // Actually draw images we have to draw.
  void RenderImages();

  // Callback for running the X UI thread.
  // TODO(piotrf): this should probably be in some singleton
  // shared among all windows.
  void RunUIThread();

  GLWindow* gl_win_;

  // TODO(piotrf): guard this behind a mutex.
  bool running_;

  boost::thread* gui_thread_;

  std::vector<ImageWithPosition> images_to_draw_;
};

}  // namespace util

#endif
