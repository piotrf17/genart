#include "poly/polygon_render.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "image/image.h"
#include "poly/genome.h"

namespace poly {

struct OfflineGLWindow {
  Display* dpy;
  GLXPixmap glx_pixmap;
  GLXContext ctx;  
};

// Tessellated polygon rendering code.
namespace tess {

void Vertex(const GLvoid* data) {
  const double* ptr = static_cast<const double*>(data);
  glVertex2f(ptr[0], ptr[1]);
}

void Error(GLenum error_code) {
  std::cout << "GLUTess error: " << gluErrorString(error_code) << std::endl;
}

void Combine(const GLdouble coords[3],
             const GLdouble* vertex_data[4],
             const GLfloat weight[4],
             GLdouble** out_data) {
}

} // namespace tess


PolygonRender::PolygonRender(int width, int height)
    : width_(width),
      height_(height) {
  win_ = new OfflineGLWindow;
}

PolygonRender::~PolygonRender() {
  glXMakeCurrent(win_->dpy, None, NULL);
  glXDestroyContext(win_->dpy, win_->ctx);
  delete win_;
  if (tess_) {
    gluDeleteTess(tess_);
  }
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
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Create a GL tesselator and callbacks.
  tess_ = gluNewTess();
  gluTessCallback(tess_, GLU_TESS_BEGIN, (void(*)())glBegin);
  gluTessCallback(tess_, GLU_TESS_END, glEnd);  
  gluTessCallback(tess_, GLU_TESS_VERTEX, (void(*)())tess::Vertex);
  gluTessCallback(tess_, GLU_TESS_ERROR, (void(*)())tess::Error);
}

image::Image* PolygonRender::ToImage(const Genome& genome) {
  glXMakeCurrent(win_->dpy, win_->glx_pixmap, win_->ctx);
  glClear(GL_COLOR_BUFFER_BIT);
  
  for (auto polygon_it = genome.begin();
       polygon_it != genome.end(); ++polygon_it) {
    double* vertex_buf = new double[3 * polygon_it->num_points()];
    
    const RGBA& c = polygon_it->color();
    glColor4f(c.r, c.g, c.b, c.a);
    
    gluTessBeginPolygon(tess_, 0);
    gluTessBeginContour(tess_);
    int i = 0;
    for (auto vertex_it = polygon_it->begin();
         vertex_it != polygon_it->end(); ++vertex_it) {
      vertex_buf[3*i] = width_ * vertex_it->x;
      vertex_buf[3*i+1] = height_ * vertex_it->y;
      vertex_buf[3*i+2] = 0;
      gluTessVertex(tess_, vertex_buf + 3 * i, vertex_buf + 3 * i);
      ++i;
    }
    gluTessEndContour(tess_);
    gluTessEndPolygon(tess_);

    delete[] vertex_buf;
  }

  // Read the image bytes into a new image object.
  unsigned char* pixels = new unsigned char[3 * width_ * height_];
  glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  return new image::Image(pixels, width_, height_);
}

}  // namespace poly
