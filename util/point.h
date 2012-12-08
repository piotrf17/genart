// A simple 2d point.

#ifndef GENART_UTIL_POINT_H
#define GENART_UTIL_POINT_H

struct Point {
  double x, y;

  Point() : x(0.0), y(0.0) {}
  Point(double ox, double oy) : x(ox), y(oy) {}
};

#endif
