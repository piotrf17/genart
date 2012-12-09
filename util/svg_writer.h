// Class for writing to an svg file.

#ifndef GENART_UTIL_SVG_WRITER_H
#define GENART_UTIL_SVG_WRITER_H

#include <fstream>
#include <vector>

#include "util/point.h"

namespace util {

class SvgWriter {
 public:
  SvgWriter();
  ~SvgWriter();

  // Starting and finishing writing.
  void Start(const std::string& filename);
  void Finish();

  // Methods to set pen type.
  struct Color {
    Color() : r(0), g(0), b(0), a(0) {}
    Color(unsigned char ar, unsigned char ag,
          unsigned char ab, unsigned char aa) :
        r(ar), g(ag), b(ab), a(aa) {}
    unsigned char r, g, b, a;
  };
  void SetStrokeColor(const Color& c);
  void SetFillColor(const Color& c);
  void SetStrokeWidth(int width);

  // Methods for drawing shapes.
  void AddRectangle(int width, int height);
  void AddPolygon(const std::vector<Point>& points);
  
 private:
  void WriteStyle();
  void WriteColor(const Color& c);
  
  std::ofstream outfile_;

  // Pen settings.
  int stroke_width_;
  Color stroke_color_;

  // Fill settings
  Color fill_color_;
};

}  // namespace util

#endif
