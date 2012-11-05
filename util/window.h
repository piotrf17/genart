// Abstract base class for an OpenGL window.  Handles initialization
// via GLX and spins off a new thread that calls the Draw() function
// at a set interval.  Also handles resizing of the window.

#ifndef GENART_UTIL_WINDOW_H
#define GENART_UTIL_WINDOW_H

#include <string>
#include <vector>

namespace boost {
  class thread;
}

namespace util {

struct GLWindow;
  
class Window {
 public:
  Window(const std::string& title, int width, int height);
  virtual ~Window();

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

  // Callback for running the X UI thread.
  // TODO(piotrf): this should probably be in some singleton
  // shared among all windows.
  void RunUIThread();

  // Subclasses must overload this method to draw what they want.
  virtual void Draw() = 0;

  GLWindow* gl_win_;

  // TODO(piotrf): guard this behind a mutex.
  bool running_;

  boost::thread* gui_thread_;
};

}  // namespace util

#endif
