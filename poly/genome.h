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

  std::vector<Polygon>::const_iterator begin() const {
    return polygons_.begin();
  }
  std::vector<Polygon>::const_iterator end() const {
    return polygons_.end();
  }

 private:

  std::vector<Polygon> polygons_;

};

}  // namespace poly

#endif
