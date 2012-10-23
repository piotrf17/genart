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

class PolygonMutatorDeletePoint : public PolygonMutator {
 public:
  virtual void operator()(Polygon* polygon,
                          std::vector<Point>* points) const;
};

class PolygonMutatorMovePoint : public PolygonMutator {
 public:
  virtual void operator()(Polygon* polygon,
                          std::vector<Point>* points) const;
};

// Exposed for testing.
namespace internal {
  
bool LineIntersect(const Point& p1, const Point& p2,
                   const Point& p3, const Point& p4);

}  // namespace internal

}  // namespace poly    

#endif
