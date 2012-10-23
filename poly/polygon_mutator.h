#ifndef GENART_POLY_POLYGON_MUTATOR_H
#define GENART_POLY_POLYGON_MUTATOR_H

#include <vector>

#include "poly/polygon.h"

namespace poly {

class PolygonMutator {
 public:
  virtual void operator()(Polygon* polygon,
                          std::vector<Point>* points) const = 0;
};

// Convex operations.
class PolygonMutatorConvexAddPoint : public PolygonMutator {
 public:
  virtual void operator()(Polygon* polygon,
                          std::vector<Point>* points) const;
};

class PolygonMutatorConvexDeletePoint : public PolygonMutator {
 public:
  virtual void operator()(Polygon* polygon,
                          std::vector<Point>* points) const;
};

class PolygonMutatorConvexMovePoint : public PolygonMutator {
 public:
  virtual void operator()(Polygon* polygon,
                          std::vector<Point>* points) const;
};

// Nonconvex operations.
class PolygonMutatorAddPoint : public PolygonMutator {
 public:
  virtual void operator()(Polygon* polygon,
                          std::vector<Point>* points) const;
};

// Exposed for testing.
namespace internal {
  
bool LineIntersect(Point p1, Point p2, Point p3, Point p4);

}  // namespace internal

}  // namespace poly    

#endif
