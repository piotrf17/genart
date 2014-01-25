#ifndef GENART_POLY_POLYGON_MUTATOR_H
#define GENART_POLY_POLYGON_MUTATOR_H

#include <vector>

#include "poly/polygon.h"

namespace genart {

namespace core {
class MutationParams;
}  // namespace core
  
namespace poly {

class PolygonMutator {
 public:
  explicit PolygonMutator(const core::MutationParams& params)
      : params_(params) {}
  virtual ~PolygonMutator() {}
  
  virtual void operator()(Polygon* polygon,
                          std::vector<Point>* points) const = 0;

 protected:
  const core::MutationParams& params_;
};

#define NEW_MUTATOR(mutator_name)                               \
  class mutator_name : public PolygonMutator {                  \
   public:                                                      \
    explicit mutator_name(const core::MutationParams& params)   \
        : PolygonMutator(params) {}                             \
    virtual ~mutator_name() {}                                  \
    virtual void operator()(Polygon* polygon,                   \
                            std::vector<Point>* points) const;  \
  };
    
NEW_MUTATOR(PolygonMutatorConvexAddPoint);
NEW_MUTATOR(PolygonMutatorConvexDeletePoint);
NEW_MUTATOR(PolygonMutatorConvexMovePoint);
NEW_MUTATOR(PolygonMutatorAddPoint);
NEW_MUTATOR(PolygonMutatorDeletePoint);
NEW_MUTATOR(PolygonMutatorMovePoint);

#undef NEW_MUTATOR

// Exposed for testing.
namespace internal {
  
bool LineIntersect(const Point& p1, const Point& p2,
                   const Point& p3, const Point& p4);

}  // namespace internal

}  // namespace poly
}  // namespace genart

#endif
