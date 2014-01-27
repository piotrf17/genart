#include "circlepack/circlepack_renderer.h"

#include <cmath>

#include <glog/logging.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <GL/gl.h>

#include "image/image.h"
#include "circlepack/circlepack_genome.h"
#include "util/offline_gl_context.h"

using genart::circlepack::RGBA;
using genart::image::Image;
using genart::util::OfflineGLContext;

// Configure CGAL Delaunay triangulation, and allow us to pass an RGBA color
// through as extra information on each vertex.
typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Triangulation_vertex_base_with_info_2<RGBA, K> Vb;
typedef CGAL::Triangulation_data_structure_2<Vb>             Tds;
typedef CGAL::Delaunay_triangulation_2<K, Tds>               Delaunay;    

namespace genart {
namespace circlepack {

namespace {

void GLCircle(float x, float y, float r, int segments) {
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(x, y);
  for( int n = 0; n <= segments; ++n ) {
    float const t = 2*M_PI*(float)n/(float)segments;
    glVertex2f(x + sin(t)*r, y + cos(t)*r);
  }
  glEnd();
}

}  // namespace

OfflineCirclepackRenderer::OfflineCirclepackRenderer(int width, int height)
    : context_(new OfflineGLContext(width, height)),
      width_(width), height_(height) {
}

OfflineCirclepackRenderer::~OfflineCirclepackRenderer() {
}

std::unique_ptr<Image> OfflineCirclepackRenderer::Render(
    const CirclepackGenome& genome) {
  CHECK(context_->Acquire());
  glClear(GL_COLOR_BUFFER_BIT);

  std::vector<std::pair<K::Point_2, RGBA>> scaled_points;
  for (const auto& point_color : genome.points()) {
    const Point& point = point_color.first;
    scaled_points.push_back(
        std::make_pair(K::Point_2(point.x * width_, point.y * height_),
                       point_color.second));
  }
  
  Delaunay dt;
  dt.insert(scaled_points.begin(), scaled_points.end());
  if (!dt.is_valid()) {
    LOG(ERROR) << "Invalid triangulation.";
    return context_->GetImage();
  }

  for (auto it = dt.finite_vertices_begin();
       it != dt.finite_vertices_end(); ++it) {
    const auto& p = it->point();
    auto vc = dt.incident_vertices(it), done(vc);
    double min_dist = 100000000.0;
    if (vc != 0) {
      do {
        const auto& q = vc->point();
        double dist = sqrt((p.x() - q.x()) * (p.x() - q.x()) +
                           (p.y() - q.y()) * (p.y() - q.y()));
        min_dist = std::min(dist, min_dist);
      } while (++vc != done);
    }
    const RGBA& color = it->info();
    glColor3f(color.r, color.g, color.b);
    GLCircle(p.x(), p.y(), 0.5 * min_dist, 24);
  }

  return context_->GetImage();
}

}  // namespace circlepack
}  // namespace genart
