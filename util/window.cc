#include "window.h"

#include <iostream>

#include <GL/gl.h>
#include <GL/glx.h>

namespace util {

// A structure capturing all the state for an OpenGL window through GLX.
struct GLWindow {
  Display                *dpy;
  int                     screen;
  ::Window                win;
  GLXContext              ctx;
  XSetWindowAttributes    attr;
  unsigned int            bpp;
  int                     x,y;
  unsigned int            width, height;
};

// Create a new GLX window with the desired width and height, and return
// a GLWindow structure capturing its state.
GLWindow* CreateGLWindow(const int width, const int height,
                         const std::string& title) {
  GLWindow* gl_win = new GLWindow();
  
    // Open a display connection to the server.
  gl_win->dpy = XOpenDisplay(0);
  gl_win->screen = DefaultScreen(gl_win->dpy);

  // Declare visual attributes for single buffered mode.
  static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4,
                              GLX_GREEN_SIZE, 4,
                              GLX_BLUE_SIZE, 4,
                              GLX_DEPTH_SIZE, 16,
                              None};
  // Declare visual attributes for double buffered mode.
  static int attrListDbl[] = {GLX_RGBA, GLX_DOUBLEBUFFER,
                              GLX_RED_SIZE, 4,
                              GLX_GREEN_SIZE, 4,
                              GLX_BLUE_SIZE, 4,
                              GLX_DEPTH_SIZE, 16,
                              None};
  // Attempt to create a double-buffer visual with desired color depths.
  XVisualInfo *vi =
      glXChooseVisual(gl_win->dpy, gl_win->screen, attrListDbl);
  // If that fails, create a single-buffered visual.
  if (vi == nullptr) {
    vi = glXChooseVisual(gl_win->dpy, gl_win->screen, attrListSgl);
  }
  
  // Create a GLX graphics context, the opengl drawing machine.
  gl_win->ctx = glXCreateContext(gl_win->dpy, vi, 0, GL_TRUE);

  // TODO(piotrf): remove random text output?
  std::cout << "Window direct rendering: "
            << (glXIsDirect(gl_win->dpy, gl_win->ctx) ? "Yes" : "No")
            << std::endl;

  // Create a colormap based on all the current X settings
  Colormap cmap = XCreateColormap(gl_win->dpy,
                                  RootWindow(gl_win->dpy, vi->screen),
                                  vi->visual,
                                  AllocNone);
  gl_win->attr.colormap = cmap;
  gl_win->attr.border_pixel = 0;

  // Set up the events that we want to listen to.
  gl_win->attr.event_mask =
      ExposureMask | KeyPressMask | KeyReleaseMask |
      ButtonPressMask | StructureNotifyMask;
  
  // Create a window with our desired height and width.
  gl_win->win = XCreateWindow(gl_win->dpy,
                              RootWindow(gl_win->dpy, vi->screen),
                              0, 0, width, height,
                              0, vi->depth, InputOutput, vi->visual,
                              CWBorderPixel | CWColormap | CWEventMask,
                              &gl_win->attr);
        
  // Set a WM_Delete protocol, so that we can properly exit if our
  // window is closed.
  Atom wmDelete = XInternAtom(gl_win->dpy, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(gl_win->dpy, gl_win->win, &wmDelete, 1);

  // Set a title for our window.
  XSetStandardProperties(gl_win->dpy, gl_win->win, title.c_str(),
                         title.c_str(), None, nullptr, 0, nullptr);

  // Make our window visible
  XMapRaised(gl_win->dpy, gl_win->win);

  // Store the current window geometry into our window structure
  ::Window win_dummy;
  unsigned int border_dummy;
  XGetGeometry(gl_win->dpy, gl_win->win, &win_dummy,
               &gl_win->x, &gl_win->y,
               &gl_win->width, &gl_win->height,
               &border_dummy, &gl_win->bpp);
  
  return gl_win;
}

// Cleanup GLX resources for a given window.  This function does not clean
// up the GLWindow itself.
void DestroyGLWindow(GLWindow* gl_win) {
  // Release the drawing context.
  if (gl_win->ctx) {
    glXMakeCurrent(gl_win->dpy, None, nullptr);
    glXDestroyContext(gl_win->dpy, gl_win->ctx);
    gl_win->ctx = nullptr;
  }

  // Close the display connection.
  XCloseDisplay(gl_win->dpy);
}

Window::Window(int width, int height, const std::string& title)
    : gl_win_(CreateGLWindow(width, height, title)),
      running_(false) {
}

Window::~Window() {
  DestroyGLWindow(gl_win_.get());
}

void Window::Run() {
  // Attach the GLX context to our window.  This must be done in the thread
  // where we'll be doing our rendering.
  glXMakeCurrent(gl_win_->dpy, gl_win_->win, gl_win_->ctx);
  
  // Start by initializing any custom application context, and then ensuring
  // that the OpenGL projection is correctly set to our window size.
  if (!Init()) {
    return;
  }
  Resize(gl_win_->width, gl_win_->height);

  // Main application event loop.
  running_ = true;
  while (running_) {
    // Handle any pending events.
    while (XPending(gl_win_->dpy) > 0) {
      XEvent event;
      XNextEvent(gl_win_->dpy, &event);
      switch (event.type) {
        case ConfigureNotify:
          if ((static_cast<unsigned int>(event.xconfigure.width) !=
               gl_win_->width) ||
              (static_cast<unsigned int>(event.xconfigure.height) !=
               gl_win_->height)) {
            gl_win_->width = event.xconfigure.width;
            gl_win_->height = event.xconfigure.height;
            Resize(gl_win_->width, gl_win_->height);
          }
          break;
        case ClientMessage:
          if (*XGetAtomName(gl_win_->dpy, event.xclient.message_type) ==
              *"WM_PROTOCOLS") {
            running_ = false;
          }
          break;
        case KeyPress:
          KeySym keysym;
          XLookupString(&event.xkey, nullptr, 0, &keysym, nullptr);
          Keypress(keysym);
          break;
      }
    }

    Draw();
    glXSwapBuffers(gl_win_->dpy, gl_win_->win);
  }
}

bool Window::Init() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  return true;
}

void Window::Close() {
  running_ = false;
}

int Window::width() const {
  return gl_win_->width;
}

int Window::height() const {
  return gl_win_->height;
}

Window2d::Window2d(int width, int height, const std::string& title)
    : Window(width, height, title) {
}

void Window2d::Resize(int width, int height) {
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width, 0, height, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

}  // namespace util
