#include "poly/polygon_image.pb.h"
#include "poly/polygon.h"
#include "poly/util.h"
#include "util/svg_writer.h"

namespace poly {

namespace {

void SaveImageToSvg(const output::PolygonImage& output,
                    const std::string& filename) {
  const int width = output.width();
  const int height = output.height();
  
  util::SvgWriter writer;
  writer.Start(filename);

  // Create a background.
  writer.SetStrokeWidth(0);
  writer.SetFillColor(util::SvgWriter::Color(0, 0, 0, 255));
  writer.AddRectangle(width, height);
  
  for (const auto& poly : output.polygon()) {
    const auto& c = poly.color();
    writer.SetStrokeColor(util::SvgWriter::Color(c.r(), c.g(), c.b(), c.a()));
    writer.SetFillColor(util::SvgWriter::Color(c.r(), c.g(), c.b(), c.a()));

    // Copy points to a format the svg writer likes.
    // TODO(piotrf): clean up our types so there is no copy.
    std::vector<Point> points;
    for (const auto& point : poly.point()) {
      points.push_back(Point(width * point.x(), height * (1 - point.y())));
    }
    writer.AddPolygon(points);
  }
  
  writer.Finish();
}

}  // namespace

void PolygonProtoToVector(const output::PolygonImage& proto,
                          std::vector<Polygon>* vec) {
  std::vector<Point> points;
  RGBA color;
  vec->clear();
  for (const auto& poly : proto.polygon()) {
    points.resize(poly.point_size());
    for (int i = 0; i < poly.point_size(); ++i) {
      points[i].x = poly.point(i).x();
      points[i].y = poly.point(i).y();
    }
    color.r = poly.color().r() / 256.0;
    color.g = poly.color().g() / 256.0;
    color.b = poly.color().b() / 256.0;
    color.a = poly.color().a() / 256.0;
    vec->push_back(Polygon(points, color));
  }
}

void VectorToPolygonProto(const std::vector<Polygon>& vec,
                          output::PolygonImage* output) {
  for (auto polygon_it = vec.begin(); polygon_it != vec.end(); ++polygon_it) {
    auto* polygon = output->add_polygon();
    for (auto vertex_it = polygon_it->begin();
         vertex_it != polygon_it->end(); ++vertex_it) {
      auto* point = polygon->add_point();
      point->set_x(vertex_it->x);
      point->set_y(vertex_it->y);
    }
    auto* color = polygon->mutable_color();
    color->set_r(static_cast<int>(256 * polygon_it->color().r));
    color->set_g(static_cast<int>(256 * polygon_it->color().g));
    color->set_b(static_cast<int>(256 * polygon_it->color().b));
    color->set_a(static_cast<int>(256 * polygon_it->color().a));
  }
}

void SaveImageToFile(const output::PolygonImage& output,
                     const std::string& filename) {
  // TODO(piotrf): handle more than svg
  SaveImageToSvg(output, filename);
}

}  // namespace poly
