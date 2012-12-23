#include "poly/polygon_animator.h"

#include <cassert>
#include <iostream>

#include "poly/animated_polygon_image.h"
#include "poly/polygon.h"

namespace poly {

namespace {

// Return a polygon that is the "vanishing" of the givin polygon,
// i.e. all points at the center and black color.
Polygon Vanish(const Polygon& polygon) {
  Point center(0.0, 0.0);
  for (const Point& p : polygon) {
    center.x += p.x;
    center.y += p.y;
  }
  center.x /= polygon.num_points();
  center.y /= polygon.num_points();
  return Polygon(std::vector<Point>(polygon.num_points(), center),
                 RGBA(0.0, 0.0, 0.0, 0.0));
}

// Given that source.num_points() < num_points, create a polygon
// that is like source, but has repeated points inserted so that it can
// smoothly transition to a target with more points.
Polygon ExpandPoints(const Polygon& source, int num_points) {
  assert(source.num_points() < num_points);
  
  std::vector<Point> matched_points;
  int num_to_add = num_points - source.num_points();
  int num_to_add_per = num_to_add / source.num_points() + 1;

  for (const Point& p : source.points()) {
    matched_points.push_back(p);
    for (int i = 0; i < num_to_add_per && num_to_add > 0; ++i) {
      matched_points.push_back(p);
      --num_to_add;
    }
  }

  return Polygon(matched_points, source.color());
}

}  // namespace

AnimatedPolygonImage* PolygonAnimator::Animate(
    const std::vector<Polygon>& image1,
    const std::vector<Polygon>& image2) {
  std::vector<Polygon> start, end;

  std::vector<const Polygon*> map;
  if (image1.size() > image2.size()) {
    for (unsigned int i = 0; i < image2.size(); ++i) {
      if (image1[i].num_points() == image2[i].num_points()) {
        start.push_back(image1[i]);
        end.push_back(image2[i]);
      } else if (image1[i].num_points() < image2[i].num_points()) {
        start.push_back(ExpandPoints(image1[i], image2[i].num_points()));
        end.push_back(image2[i]);
      } else {
        start.push_back(image1[i]);
        end.push_back(ExpandPoints(image2[i], image1[i].num_points()));
      }
    }
    for (unsigned int i = image2.size(); i < image1.size(); ++i) {
      start.push_back(image1[i]);
      end.push_back(Vanish(image1[i]));
    }
  } else {
    std::cout << "NOT IMPLEMENTED" << std::endl;
  }

  AnimatedPolygonImage* image = new AnimatedPolygonImage(start, end);
  return image;
}

}  // namespace poly
