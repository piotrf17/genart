// An interface for a polygon image that can be animated, with accessors
// that can see the current view of the polygon.

#ifndef GENART_POLY_ANIMATED_POLYGON_IMAGE_H
#define GENART_POLY_ANIMATED_POLYGON_IMAGE_H

#include <vector>

#include "poly/polygon.h"

namespace poly {

class AnimatedPolygonIterator {
 public:
  AnimatedPolygonIterator(
      const std::vector<Point>::const_iterator it1,
      const std::vector<Point>::const_iterator it2,
      double t);
  ~AnimatedPolygonIterator();

  const Point& operator*();
  const Point* operator->();

  AnimatedPolygonIterator& operator++();

  bool operator==(const AnimatedPolygonIterator& o);
  bool operator!=(const AnimatedPolygonIterator& o);

 private:
  std::vector<Point>::const_iterator it1_, it2_;
  Point p;
  double t_;
};
  
class AnimatedPolygon {
 public:
  AnimatedPolygon(double t);
  ~AnimatedPolygon();

  void Set(const Polygon* p1, const Polygon* p2);

  RGBA color() const;
  int num_points() const;

  AnimatedPolygonIterator begin() const;
  AnimatedPolygonIterator end() const;

 private:
  const Polygon* p1_;
  const Polygon* p2_;
  double t_;
};
  
class AnimatedPolygonImageIterator {
 public:
  AnimatedPolygonImageIterator(
      const std::vector<Polygon>::const_iterator& it1,
      const std::vector<Polygon>::const_iterator& it2,
      double t);
  ~AnimatedPolygonImageIterator();

  const AnimatedPolygon& operator*();
  const AnimatedPolygon* operator->();

  AnimatedPolygonImageIterator& operator++();

  bool operator==(const AnimatedPolygonImageIterator& o);
  bool operator!=(const AnimatedPolygonImageIterator& o);
  
 private:
  std::vector<Polygon>::const_iterator it1_, it2_;
  AnimatedPolygon view_;
};

class AnimatedPolygonImage {
 public:
  AnimatedPolygonImage(const std::vector<Polygon> start,
                       const std::vector<Polygon> end);
  ~AnimatedPolygonImage();

  void SetTime(double t);

  AnimatedPolygonImageIterator begin() const;
  AnimatedPolygonImageIterator end() const;
  
 private:
  std::vector<Polygon> start_, end_;

  double t_;
};

}  // namespace poly

#endif
