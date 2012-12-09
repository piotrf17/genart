#include "poly/polygon_image.pb.h"
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

void SaveImageToFile(const output::PolygonImage& output,
                     const std::string& filename) {
  // TODO(piotrf): handle more than svg
  SaveImageToSvg(output, filename);
}

}  // namespace poly
