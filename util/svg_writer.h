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

  // Methods to set pen color.
  struct Color {
    Color() : r(0), g(0), b(0), a(0) {}
    Color(unsigned char ar, unsigned char ag,
          unsigned char ab, unsigned char aa) :
        r(ar), g(ag), b(ab), a(aa) {}
    unsigned char r, g, b, a;
  };
  void SetStrokeColor(const Color& c);
  void SetFillColor(const Color& c);

  // Methods for drawing shapes.
  void AddPolygon(const std::vector<Point>& points,
                  int stroke_width);
  
 private:
  void WriteColor(const Color& c);
  
  std::ofstream outfile_;

  Color stroke_color, fill_color;
};

}  // namespace util

#endif
