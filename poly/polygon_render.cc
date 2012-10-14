#include "poly/polygon_render.h"

#include <GL/gl.h>
#include <GL/glx.h>

#include "image/image.h"

namespace poly {

struct OfflineGLWindow {
  Display* dpy;
  GLXPixmap glx_pixmap;
  GLXContext ctx;  
};

PolygonRender::PolygonRender(int width, int height)
    : width_(width),
      height_(height) {
  win_ = new OfflineGLWindow;
}

PolygonRender::~PolygonRender() {
  glXMakeCurrent(win_->dpy, None, NULL);
  glXDestroyContext(win_->dpy, win_->ctx);
  delete win_;
}

void PolygonRender::Init() {
  // Open a display connection to the X server.
  win_->dpy = XOpenDisplay(NULL);
  int screen = DefaultScreen(win_->dpy);

  // Choose a single buffered visual.
  static int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4,
                              GLX_GREEN_SIZE, 4,
                              GLX_BLUE_SIZE, 4,
                              GLX_DEPTH_SIZE, 16,
                              None};
  XVisualInfo* vi = glXChooseVisual(win_->dpy, screen, attrListSgl);

  // Create a pixmap for offline rendering.
  Pixmap pixmap = XCreatePixmap(win_->dpy,
                                RootWindow(win_->dpy, vi->screen),
                                width_, height_, vi->depth);
  win_->glx_pixmap = glXCreateGLXPixmap(win_->dpy, vi, pixmap);

  // Create a GLX context.
  win_->ctx = glXCreateContext(win_->dpy, vi, 0, GL_FALSE);
  glXMakeCurrent(win_->dpy, win_->glx_pixmap, win_->ctx);

  // Create a viewport the size of the image.
  glViewport(0, 0, width_, height_);
  // Setup an orthographic projection.
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, width_, 0, height_, -1.0, 1.0);
  // Return to model view matrix mode.
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Clear and set colors.
  glShadeModel(GL_FLAT);
  glClearColor(0.0, 0.0, 0.0, 1);
  glDisable(GL_DEPTH_TEST);
  glFlush();
  glClear(GL_COLOR_BUFFER_BIT);
}

image::Image* PolygonRender::ToImage(const Genome& genome) {
  glXMakeCurrent(win_->dpy, win_->glx_pixmap, win_->ctx);
  
  // PolygonRender the image with OpenGL.
  glBegin(GL_QUADS);
  glColor3f(0.5, 0.0, 0.0);
  glVertex2f(100, 100);
  glVertex2f(500, 100);
  glVertex2f(500, 300);
  glVertex2f(100, 300);
  glEnd();

  // Read the image bytes into a new image object.
  unsigned char* pixels = new unsigned char[3 * width_ * height_];
  glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  return new image::Image(pixels, width_, height_);
}

}  // namespace poly
