#include "poly/polygon_render.h"

#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "image/image.h"
#include "poly/animated_polygon_image.h"
#include "poly/genome.h"

namespace poly {

// Necessary state for a GLX Pixmap based offline rendering window.
struct OfflineGLWindow {
  Display* dpy;
  GLXPixmap glx_pixmap;
  GLXContext ctx;  
};

// Utility functions for tessellated polygon rendering.  These are passed
// through as callbacks to the OpenGL tesselator.
namespace tess {

void Vertex(const GLvoid* data) {
  const double* ptr = static_cast<const double*>(data);
  glVertex2f(ptr[0], ptr[1]);
}

void Error(GLenum error_code) {
  std::cout << "GLUTess error: " << gluErrorString(error_code) << std::endl;
}

void Combine(const GLdouble new_vert[3],
             const GLdouble* neighbor_vert[4],
             const GLfloat neighbor_weight[4],
             GLdouble** out_data) {
  // TODO(piotrf): figure out a better way to deal with the ugliness
  // of memory managing combine.
  static GLdouble combine_data[64][3];
  static int combine_vertex = 0;

  combine_vertex = (combine_vertex + 1) % 64;
  combine_data[combine_vertex][0] = new_vert[0];
  combine_data[combine_vertex][1] = new_vert[1];
  combine_data[combine_vertex][2] = new_vert[2];
  
  *out_data = combine_data[combine_vertex];
}

} // namespace tess

PolygonRender::PolygonRender() {
  // Create a GL tesselator and callbacks.
  tess_ = gluNewTess();
  gluTessCallback(tess_, GLU_TESS_BEGIN, (void(*)())glBegin);
  gluTessCallback(tess_, GLU_TESS_END, glEnd);  
  gluTessCallback(tess_, GLU_TESS_VERTEX, (void(*)())tess::Vertex);
  gluTessCallback(tess_, GLU_TESS_ERROR, (void(*)())tess::Error);
  gluTessCallback(tess_, GLU_TESS_COMBINE, (void(*)())tess::Combine);
}

PolygonRender::~PolygonRender() {
  if (tess_) {
    gluDeleteTess(tess_);
  }
}

void PolygonRender::Render(const std::vector<Polygon>& polygons,
                           int width, int height) {
  for (auto polygon_it = polygons.begin();
       polygon_it != polygons.end(); ++polygon_it) {
    double* vertex_buf = new double[3 * polygon_it->num_points()];
    
    const RGBA& c = polygon_it->color();
    glColor4f(c.r, c.g, c.b, c.a);
    
    gluTessBeginPolygon(tess_, 0);
    gluTessBeginContour(tess_);
    int i = 0;
    for (auto vertex_it = polygon_it->begin();
         vertex_it != polygon_it->end(); ++vertex_it) {
      vertex_buf[3*i] = width * vertex_it->x;
      vertex_buf[3*i+1] = height * vertex_it->y;
      vertex_buf[3*i+2] = 0;
      gluTessVertex(tess_, vertex_buf + 3 * i, vertex_buf + 3 * i);
      ++i;
    }
    gluTessEndContour(tess_);
    gluTessEndPolygon(tess_);

    delete[] vertex_buf;
  }
}

void PolygonRender::Render(const AnimatedPolygonImage& image,
                           int width, int height) {
  for (auto polygon_it = image.begin();
       polygon_it != image.end(); ++polygon_it) {
    double* vertex_buf = new double[3 * polygon_it->num_points()];
    
    const RGBA& c = polygon_it->color();
    glColor4f(c.r, c.g, c.b, c.a);
    
    gluTessBeginPolygon(tess_, 0);
    gluTessBeginContour(tess_);
    int i = 0;
    for (auto vertex_it = polygon_it->begin();
         vertex_it != polygon_it->end(); ++vertex_it) {
      vertex_buf[3*i] = width * vertex_it->x;
      vertex_buf[3*i+1] = height * vertex_it->y;
      vertex_buf[3*i+2] = 0;
      gluTessVertex(tess_, vertex_buf + 3 * i, vertex_buf + 3 * i);
      ++i;
    }
    gluTessEndContour(tess_);
    gluTessEndPolygon(tess_);

    delete[] vertex_buf;
  }
}

OfflinePolygonRender::OfflinePolygonRender(int width, int height)
    : width_(width),
      height_(height) {
  win_ = new OfflineGLWindow;
}

OfflinePolygonRender::~OfflinePolygonRender() {
  glXMakeCurrent(win_->dpy, None, NULL);
  glXDestroyContext(win_->dpy, win_->ctx);
  XCloseDisplay(win_->dpy);
  delete win_;
}

bool OfflinePolygonRender::Init() {
  // Open a display connection to the X server.
  win_->dpy = XOpenDisplay(nullptr);
  if (win_->dpy == nullptr) {
    std::cerr << "ERROR: Failed to connect to the X server" << std::endl;
    return false;
  }
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
  win_->ctx = glXCreateContext(win_->dpy, vi, 0, GL_TRUE);
  if (!glXMakeCurrent(win_->dpy, win_->glx_pixmap, win_->ctx)) {
    std::cout << "Failed to make direct pixmap context" << std::endl;
    return false;
  }

  std::cout << "Pixmap direct rendering: "
            << (glXIsDirect(win_->dpy, win_->ctx) ? "Yes" : "No")
            << std::endl;
  
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

  return true;
}

image::Image* OfflinePolygonRender::ToImage(
    const std::vector<Polygon>& polygons) {
  glXMakeCurrent(win_->dpy, win_->glx_pixmap, win_->ctx);
  glClear(GL_COLOR_BUFFER_BIT);

  render_.Render(polygons, width_, height_);
  
  // Read the image bytes into a new image object.
  unsigned char* pixels = new unsigned char[3 * width_ * height_];

  // For some reason, openGL is ignoring the pack alignment parameter,
  // and insists on packing aligned to 4 bytes.  So we make a copy
  // and do the realignment here.
  if (width_ % 4 == 0) {
    glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  } else {
    const int aligned_byte_width = (3 * width_ + (4 - 1)) & -4;
    unsigned char* aligned_pixels =
        new unsigned char[aligned_byte_width * height_];
    glReadPixels(0, 0, width_, height_, GL_RGB, GL_UNSIGNED_BYTE, aligned_pixels);
    for (int i = 0; i < height_; ++i) {
      for (int j = 0; j < width_; ++j) {
        const int dealigned = 3 * (i * width_ + j);
        const int aligned = i * aligned_byte_width + 3 * j;
        pixels[dealigned + 0] = aligned_pixels[aligned + 0];
        pixels[dealigned + 1] = aligned_pixels[aligned + 1];
        pixels[dealigned + 2] = aligned_pixels[aligned + 2];
      }
    }
  }

  return new image::Image(pixels, width_, height_);
}

}  // namespace poly
