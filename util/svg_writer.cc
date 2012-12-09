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
  stroke_color_ = c;
}

void SvgWriter::SetFillColor(const Color& c) {
  fill_color_ = c;
}

void SvgWriter::SetStrokeWidth(int width) {
  stroke_width_ = width;
}

void SvgWriter::AddRectangle(int width, int height) {
  outfile_ << "  <rect width=\"" << width << "\""
           << " height=\"" << height << "\"" << std::endl;
  WriteStyle();
  outfile_ << "  />" << std::endl;
}

void SvgWriter::AddPolygon(const std::vector<Point>& points) {
  outfile_ << "  <polygon" << std::endl
           << "      points=\"";
  for (const Point& p : points) {
    outfile_ << p.x << "," << p.y << " ";
  }
  outfile_ << "\"" << std::endl;
  WriteStyle();
  outfile_ << "  />" << std::endl;
}

void SvgWriter::WriteStyle() {
  outfile_ << "      style=\"fill:";
  WriteColor(fill_color_);
  outfile_ << ";fill-opacity:" << fill_color_.a / 255.0;
  outfile_ << ";stroke:";
  WriteColor(stroke_color_);
  outfile_ << ";stroke-opacity:" << stroke_color_.a / 255.0;
  outfile_ << ";stroke-width:" << stroke_width_ << ";\"" << std::endl;
}

void SvgWriter::WriteColor(const Color& c) {
  // rgba is still unsupported by eog, convert
  outfile_ << "rgb("
           << static_cast<int>(c.r) << ","
           << static_cast<int>(c.g) << ","
           << static_cast<int>(c.b) << ")";
}

}  // namespace util
