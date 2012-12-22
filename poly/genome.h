// Genome captures the polygon based genome of a drawing.

#ifndef GENART_POLY_GENOME_H
#define GENART_POLY_GENOME_H

#include <vector>

#include "poly/polygon.h"

namespace poly {

class MutationParams;
  
class Genome {
 public:
  Genome();
  ~Genome();

  void Randomize(const MutationParams& params,
                 int num_poly);

  void Mutate(const MutationParams& params);

  const std::vector<Polygon>& polygons() const {
    return polygons_;
  }
  
 private:

  std::vector<Polygon> polygons_;

};

}  // namespace poly

#endif
