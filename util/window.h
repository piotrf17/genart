// Abstract base class for an OpenGL window.  Handles initialization
// via GLX and spins off a new thread that calls the HandleDraw() function
// at a set interval.  Also handles resizing of the window.

#ifndef GENART_UTIL_WINDOW_H
#define GENART_UTIL_WINDOW_H

#include <atomic>
#include <string>
#include <thread>
#include <vector>

namespace util {

struct GLWindow;
  
class Window {
 public:
  Window(const std::string& title, int width, int height);
  virtual ~Window();
  
 protected:
  // Methods to be overloaded by subclasses.
  virtual void HandleKey(unsigned int state, unsigned int keycode) = 0;
  virtual void HandleDraw() = 0;
  virtual void HandleClose() = 0;
  
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

  GLWindow* gl_win_;

  // TODO(piotrf): guard this behind a mutex.
  std::atomic<bool> running_;

  std::thread* gui_thread_;
};

}  // namespace util

#endif
