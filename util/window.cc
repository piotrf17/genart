#include "util/window.h"

#include <GL/gl.h>
#include <GL/glx.h>
#include <boost/thread.hpp>

#include "image/image.h"

namespace util {

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

Window::Window(const std::string& title, int width, int height) {
  gl_win_ = new GLWindow();
  CreateGLWindow(title, width, height);
  InitGLScene();
  ResizeGLScene();

  gui_thread_ = new boost::thread(&Window::RunUIThread, this);
}

Window::~Window() {
  running_ = false;
  gui_thread_->join();
  DestroyGLWindow();
  delete gl_win_;
}

void Window::Reset() {
  images_to_draw_.clear();
}

void Window::DrawImage(const image::Image& image, int x, int y) {
  ImageWithPosition new_image;
  new_image.image = &image;
  new_image.x = x;
  new_image.y = y;
  images_to_draw_.push_back(new_image);
}

void Window::CreateGLWindow(const std::string& title,
                            int width, int height) {
  // Open a display connection to the server
  gl_win_->dpy = XOpenDisplay(0);
  gl_win_->screen = DefaultScreen(gl_win_->dpy);

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
      glXChooseVisual(gl_win_->dpy, gl_win_->screen, attrListDbl);
  // If that fails, create a single-buffered visual
  if(NULL == vi) {
    vi = glXChooseVisual(gl_win_->dpy, gl_win_->screen, attrListSgl);
  }
  
  // Create a GLX graphics context, the opengl drawing machine
  gl_win_->ctx = glXCreateContext(gl_win_->dpy, vi, 0, GL_TRUE);

  // Create a colormap based on all the current X settings
  Colormap cmap = XCreateColormap(gl_win_->dpy,
                                  RootWindow(gl_win_->dpy, vi->screen),
                                  vi->visual,
                                  AllocNone);
  gl_win_->attr.colormap = cmap;
  gl_win_->attr.border_pixel = 0;

  // Set up the events that we want to listen to.
  gl_win_->attr.event_mask =
      ExposureMask | KeyPressMask | KeyReleaseMask |
      ButtonPressMask | StructureNotifyMask;
  
  // Create a window with our desired height and width.
  gl_win_->win = XCreateWindow(gl_win_->dpy,
                               RootWindow(gl_win_->dpy, vi->screen),
                               0, 0, width, height,
                               0, vi->depth, InputOutput, vi->visual,
                               CWBorderPixel | CWColormap | CWEventMask,
                               &gl_win_->attr);
        
  // Set a WM_Delete protocol, so that we can properly exit if our
  // window is closed.
  Atom wmDelete = XInternAtom(gl_win_->dpy, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(gl_win_->dpy, gl_win_->win, &wmDelete, 1);

  // Set a title for our window.
  XSetStandardProperties(gl_win_->dpy, gl_win_->win, title.c_str(),
                         title.c_str(), None, NULL, 0, NULL);

  // Make our window visible
  XMapRaised(gl_win_->dpy, gl_win_->win);

  // Store the current window geometry into our window structure
  ::Window winDummy;
  unsigned int borderDummy;
  XGetGeometry(gl_win_->dpy, gl_win_->win, &winDummy,
               &gl_win_->x, &gl_win_->y,
               &gl_win_->width, &gl_win_->height,
               &borderDummy, &gl_win_->bpp);
}

void Window::DestroyGLWindow() {
  // Release the drawing context.
  if (gl_win_->ctx) {
    glXMakeCurrent(gl_win_->dpy, None, NULL);
    glXDestroyContext(gl_win_->dpy, gl_win_->ctx);
    gl_win_->ctx = NULL;
  }

  // Close the display connection.
  XCloseDisplay(gl_win_->dpy);
}

void Window::InitGLScene() {
  glShadeModel(GL_FLAT);
  glClearColor(0.0, 0.0, 0.0, 1);
  glDisable(GL_DEPTH_TEST);
  glFlush();
  glClear(GL_COLOR_BUFFER_BIT);
}

void Window::ResizeGLScene() {
  // Setup a viewport to fill the window.
  glViewport(0, 0, gl_win_->width, gl_win_->height);

  // Setup an orthographic projection.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, gl_win_->width, 0, gl_win_->height, -1.0, 1.0);

  // Return to model view matrix mode.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void Window::RenderImages() {
  for (unsigned int i = 0; i < images_to_draw_.size(); ++i) {
    glRasterPos2i(images_to_draw_[i].x,
                  images_to_draw_[i].y);
    glDrawPixels(images_to_draw_[i].image->width(),
                 images_to_draw_[i].image->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 images_to_draw_[i].image->pixels());
  }
}

void Window::RunUIThread() {
  // Attach the GLX context to our window.  This must be done in the thread
  // where we'll be doing our rendering.
  glXMakeCurrent(gl_win_->dpy, gl_win_->win, gl_win_->ctx);
  
  running_ = true;
  while (running_) {
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
            ResizeGLScene();
          }
          break;
        case ClientMessage:
          if (*XGetAtomName(gl_win_->dpy, event.xclient.message_type) ==
              *"WM_PROTOCOLS") {
            // TODO(piotrf): figure out a way to shut down the window
            // from here.
            running_ = false;
          }
          break;
      }
    }

    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);

    RenderImages();
    
    glXSwapBuffers(gl_win_->dpy, gl_win_->win);
    
    // Sleep for a while so as not to spin the proc.
    usleep(100000);
  }
}

}  // namespace util
