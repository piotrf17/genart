// Genome captures the polygon based genome of a drawing.

#ifndef GENART_POLY_POLYGON_GENOME_H_
#define GENART_POLY_POLYGON_GENOME_H_

#include <vector>

#include "core/genome.h"
#include "poly/polygon.h"

namespace genart {

namespace core {
// TODO(piotrf): use an extened poly-specific mutation params.
class MutationParams;
}  // namespace core
  
namespace poly {

class PolygonGenome : public core::Genome {
 public:
  PolygonGenome();
  explicit PolygonGenome(const std::vector<Polygon> polygons);
  virtual ~PolygonGenome();

  virtual std::unique_ptr<core::Genome> Clone() const;
  virtual void Mutate(const core::MutationParams& params);

  // TODO(piotrf): turn this into a constructor instead.
  void Randomize(const core::MutationParams& params, int num_poly);

  const std::vector<Polygon>& polygons() const {
    return polygons_;
  }
  
 private:
  std::vector<Polygon> polygons_;
};

}  // namespace poly
}  // namespace genart

#endif  // GENART_POLY_GENOME_H_
