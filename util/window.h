// Some basic window classes that simplify creating an application with an
// OpenGL window.  These classes setup reasonable defaults, and simply leave
// a few functions to be overridden:
//   Init(): called before entering the main application loop.
//   Resize(): called after a resize event (easiest to use defaults)
//   Keypress(): called whenever a key is _pressed_ (not released)
//   Draw(): called after handling all events
// 
// The classes provided in this file are:
// Window - A bare bones abstract class that handles an event loop.
// Window2d - Sets up an orthographic projection with pixel coordinates
//   in the rectangle (0, 0) - (width, height)
//
// To use, inherit from one of these classes, and implement any missing virtual
// functions.  Then, simply create your window and call run:
// MyWindow window(800, 600, "A program");
// window.Run();

#ifndef GENART_UTIL_WINDOW_H
#define GENART_UTIL_WINDOW_H

#include <memory>
#include <string>

#include <X11/keysym.h>

namespace util {

struct GLWindow;

class Window {
 public:
  // Create a new window with the given width and height in pixels, and
  // a window title string given by title.
  Window(int width, int height, const std::string& title);
  virtual ~Window();

  // Start running the application.  Starts off by calling Init(), Resize(),
  // and then enters an event handling loop, each loop calling Draw().
  // The event handler finishes either when Close() is called, or when the
  // window is closed externally.
  void Run();

 protected:
  // For initializing any application state or OpenGL stuff.
  virtual bool Init();

  // Handle resizing the window to a new width and height.
  virtual void Resize(int width, int height) = 0;

  // Handle the initial press of a key.  The key code is an X11 KeySym.
  virtual void Keypress(unsigned int key) = 0;

  // Handle redrawing the frame.
  virtual void Draw() = 0;

  // Request that the window is closed, also ends the event loop in Run().
  // Note that depending on where you call this, Draw() may be called 1
  // more time.  Also note that the window may be externally closed.
  void Close();

  int width() const;
  int height() const;
  
 private:
  std::unique_ptr<GLWindow> gl_win_;
  bool running_;
};

class Window2d : public Window {
 public:
  Window2d(int width, int height, const std::string& title);

 protected:
  virtual void Resize(int width, int height);
};

}  // namespace util

#endif
