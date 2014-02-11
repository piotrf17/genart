#include "poly/polygon_mutator.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "poly/poly_params.pb.h"
#include "poly/polygon.h"
#include "util/random.h"

using util::Random;

namespace genart {
namespace poly {

namespace internal {

bool LineIntersect(const Point& p1, const Point& p2,
                   const Point& p3, const Point& p4) {
  double det = (p4.y - p3.y) * (p2.x - p1.x) - (p4.x - p3.x) * (p2.y - p1.y);
  // TODO(piotrf): Use a more sensible check here.
  if (fabs(det) < 0.001) {
    return false;
  }
  double ua =
      ((p4.x - p3.x) * (p1.y - p3.y) - (p4.y - p3.y) * (p1.x - p3.x)) / det;
  double ub =
      ((p2.x - p1.x) * (p1.y - p3.y) - (p2.y - p1.y) * (p1.x - p3.x)) / det;
  return (ua > 0 && ua < 1 && ub > 0 && ub < 1);
}

}  // namespace internal

//=================== Convex Polygon Mutations ===================//

// Add a random point to the polygon, maintining the convexity of the polygon
// and also clamping to remain within the window.
// TODO(piotrf): clamping may result in polygons with lots of points along
// a wall, perhaps check if the side lies along a wall before adding a point.
void PolygonMutatorConvexAddPoint::operator()(
    Polygon* polygon,
    std::vector<Point>* points) const {
  if (static_cast<int>(points->size()) >= params_.max_points_per_polygon()) {
    return;
  }

  // Choose a random side and compute the max vertex angle.
  int side = Random::Integer(points->size());
  double max_angle = std::max(
      polygon->ComputeInteriorAngle(side),
      polygon->ComputeInteriorAngle((side + 1) % points->size()));

  // Calculate an offset vector from our chosen side midpoint based on the
  // convexity constraint.
  const Point& a = (*points)[side];
  const Point& b = (*points)[(side + 1) % (points->size())];
  double offset = Random::Double();
  if (max_angle > M_PI / 2.0) {
    offset *= std::min(tan(M_PI - max_angle), 1.0);
  }
  double off_x = 0.5 * (b.y - a.y) * offset;
  double off_y = -0.5 * (b.x - a.x) * offset;

  // Add this new point to our list.
  Point c;
  c.x = std::min(std::max(0.5 * (a.x + b.x) + off_x, 0.0), 1.0);
  c.y = std::min(std::max(0.5 * (a.y + b.y) + off_y, 0.0), 1.0);
  points->insert(points->begin() + side + 1, c);
}

// Removing a random point from a convex polygon keeps it convex.
void PolygonMutatorConvexDeletePoint::operator()(
    Polygon* polygon,
    std::vector<Point>* points) const {
  if (points->size() <= 3) {
    return;
  }
  int i = Random::Integer(points->size());
  points->erase(points->begin() + i);
}

void PolygonMutatorConvexMovePoint::operator()(
    Polygon* polygon,
    std::vector<Point>* points) const {
  int i = Random::Integer(points->size());
  for (int iter = 0; iter < 10; ++iter) {
    double dx = Random::Double(-params_.max_move(), params_.max_move());
    double dy = Random::Double(-params_.max_move(), params_.max_move());

    (*points)[i].x += dx;
    (*points)[i].y += dy;

    if (polygon->Convex()) {
      break;
    }

    (*points)[i].x -= dx;
    (*points)[i].y -= dy;    
  }

  // A triangle can stay convex with a point movement that flips the
  // orientation of the outside.  Ensure that we're CCW oriented.
  if (points->size() == 3) {
    const Point& a = (*points)[0];
    const Point& b = (*points)[1];
    const Point& c = (*points)[2];
    double det = (a.x - b.x) * (c.y - b.y) - (a.y - b.y) * (c.x - b.x);
    if (det > 0) {
      std::swap((*points)[1], (*points)[2]);
    }
  }  
}

//=================== General Polygon Mutations ===================//

void PolygonMutatorAddPoint::operator()(
    Polygon* polygon,
    std::vector<Point>* points) const {
  if (static_cast<int>(points->size()) >= params_.max_points_per_polygon()) {
    return;
  }

  int side = Random::Integer(points->size());
  const Point& a = (*points)[side];
  const Point& c = (*points)[(side + 1) % points->size()];
  Point b;
  bool intersecting = false;

  const double max_n = params_.non_convex_add_max_normal_distance();
  const double max_t = params_.non_convex_add_max_tangential_distance();
  
  for (int iter = 0; iter < 10; ++iter) {
    // Try a point somewhere in the half-space formed by this side.
    double un = Random::Double(max_n);
    double ut = Random::Double(-max_t, max_t);
    double off_x = (b.y - a.y) * un + (b.x - a.x) * ut;
    double off_y = -(b.x - a.x) * un + (b.y - a.y) * ut;
    b.x = std::min(std::max(0.5 * (a.x + c.x) + off_x, 0.0), 1.0);
    b.y = std::min(std::max(0.5 * (a.y + c.y) + off_y, 0.0), 1.0);
    
    intersecting = false;
    // Check sides before the one we're expanding.
    for (int i = 0; i < side; ++i) {
      if (internal::LineIntersect(
          a, b,
          (*points)[i], (*points)[i + 1])) {
        intersecting = true;
        break;
      } else if (internal::LineIntersect(
          b, c,
          (*points)[i], (*points)[i + 1])) {
        intersecting = true;
        break;
      }
    }
    if (intersecting) {
      continue;
    }
    // Check sides after the one we're expanding.
    for (int i = side + 1; i < static_cast<int>(points->size()); ++i) {
      if (internal::LineIntersect(
          a, b,
          (*points)[i], (*points)[(i + 1) % points->size()])) {
        intersecting = true;
        break;
      } else if (internal::LineIntersect(
          b, c,
          (*points)[i], (*points)[(i + 1) % points->size()])) {
        intersecting = true;
        break;
      }      
    }
    if (!intersecting) {
      points->insert(points->begin() + side + 1, b);
      return;
    }
  }
}

void PolygonMutatorDeletePoint::operator()(
    Polygon* polygon,
    std::vector<Point>* points) const {
  if (points->size() <= 3) {
    return;
  }
  for (int iter = 0; iter < 10; ++iter) {
    int i = Random::Integer(points->size());
    const Point& a = (*points)[(i - 1 + points->size()) % points->size()];
    const Point& b = (*points)[(i + 1) % points->size()];
    bool intersecting = false;
    for (int j = 0; j < i; ++j) {
      if (internal::LineIntersect(
          a, b,
          (*points)[j], (*points)[j + 1])) {
        intersecting = true;
        break;
      }
    }
    if (intersecting) {
      continue;
    }
    for (int j = i + 1; j < static_cast<int>(points->size()); ++j) {
      if (internal::LineIntersect(
          a, b,
          (*points)[j], (*points)[(j + 1) % points->size()])) {
        intersecting = true;
        break;
      }
    }
    if (!intersecting) {
      points->erase(points->begin() + i);
      return;
    }
  }
}

void PolygonMutatorMovePoint::operator()(
    Polygon* polygon,
    std::vector<Point>* points) const {
  int i = Random::Integer(points->size());

  const Point& a = (*points)[(i - 1 + points->size()) % points->size()];
  const Point& c = (*points)[(i + 1) % points->size()];
  
  for (int iter = 0; iter < 10; ++iter) {
    Point b = (*points)[i];
    double dx = Random::Double(-params_.max_move(), params_.max_move());
    double dy = Random::Double(-params_.max_move(), params_.max_move());

    b.x += dx;
    b.y += dy;
    bool intersecting = false;
    for (int j = 0; j < i - 1; ++j) {
      if (internal::LineIntersect(
          a, b,
          (*points)[j], (*points)[j + 1])) {
        intersecting = true;
        break;
      } else if (internal::LineIntersect(
          b, c,
          (*points)[j], (*points)[j + 1])) {
        intersecting = true;
        break;
      }
    }
    if (intersecting) {
      continue;
    }
    for (int j = i + 1; j < static_cast<int>(points->size()); ++j) {
      if (internal::LineIntersect(
          a, b,
          (*points)[j], (*points)[(j + 1) % points->size()])) {
        intersecting = true;
        break;
      } else if (internal::LineIntersect(
          b, c,
          (*points)[j], (*points)[(j + 1) % points->size()])) {
        intersecting = true;
        break;
      }

    }
    if (!intersecting) {
      (*points)[i].x += dx;
      (*points)[i].y += dy;
      
      // A triangle can stay convex with a point movement that flips the
      // orientation of the outside.  Ensure that we're CCW oriented.
      if (points->size() == 3) {
        const Point& a = (*points)[0];
        const Point& b = (*points)[1];
        const Point& c = (*points)[2];
        double det = (a.x - b.x) * (c.y - b.y) - (a.y - b.y) * (c.x - b.x);
        if (det > 0) {
          std::swap((*points)[1], (*points)[2]);
        }
      }

      return;
    }
    
  }
}

}  // namespace poly
}  // namespace genart
