#include "poly/polygon_renderer.h"

#include <glog/logging.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "image/image.h"
//#include "poly/animated_polygon_image.h"
#include "poly/polygon_genome.h"
#include "util/offline_gl_context.h"

using genart::util::OfflineGLContext;

namespace genart {
namespace poly {

// Utility functions for tessellated polygon rendering.  These are passed
// through as callbacks to the OpenGL tesselator.
namespace tess {

void Vertex(const GLvoid* data) {
  const double* ptr = static_cast<const double*>(data);
  glVertex2f(ptr[0], ptr[1]);
}

void Error(GLenum error_code) {
  LOG(ERROR) << "GLUTess error: " << gluErrorString(error_code) << std::endl;
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
/*
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
*/

OfflinePolygonRenderer::OfflinePolygonRenderer(int width, int height)
    : context_(new OfflineGLContext(width, height)),
      width_(width), height_(height) {
}

OfflinePolygonRenderer::~OfflinePolygonRenderer() {
}

std::unique_ptr<image::Image> OfflinePolygonRenderer::Render(
    const PolygonGenome& genome) {
  context_->Acquire();
  glClear(GL_COLOR_BUFFER_BIT);

  render_.Render(genome.polygons(), width_, height_);

  return context_->GetImage();
}

}  // namespace poly
}  // namespace genart

