#include <iostream>

#include "util/svg_writer.h"

namespace util {

SvgWriter::SvgWriter() {
}

SvgWriter::~SvgWriter() {
  // Handle if someone forgot to call Finish.
  if (outfile_.is_open()) {
    Finish();
  }
}

void SvgWriter::Start(const std::string& filename) {
  outfile_.open(filename);
  outfile_ << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"
           << std::endl;
}

void SvgWriter::Finish() {
  outfile_ << "</svg>" << std::endl;
  outfile_.close();
}

void SvgWriter::SetStrokeColor(const Color& c) {
  stroke_color = c;
}

void SvgWriter::SetFillColor(const Color& c) {
  fill_color = c;
}

void SvgWriter::AddPolygon(const std::vector<Point>& points,
                           int stroke_width = 1) {
  outfile_ << "  <polygon" << std::endl
           << "      points=\"";
  for (const Point& p : points) {
    outfile_ << p.x << "," << p.y << " ";
  }
  outfile_ << "\"" << std::endl;
  outfile_ << "      style=\"fill:";
  WriteColor(fill_color);
  outfile_ << ";stroke:";
  WriteColor(stroke_color);
  outfile_ << ";stroke-width:" << stroke_width << ";\"" << std::endl;
  outfile_ << "  />" << std::endl;
}

void SvgWriter::WriteColor(const Color& c) {
  outfile_ << "rgba(" << c.r << c.g << c.b << c.a << ")" << std::endl;
}

}  // namespace util
