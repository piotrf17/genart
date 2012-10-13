// Genome captures the polygon based genome of a drawing.

#ifndef GENART_POLY_GENOME_H
#define GENART_POLY_GENOME_H

#include <vector>

#include "poly/polygon.h"

namespace poly {

class Genome {
 public:
  Genome();
  ~Genome();

  void Randomize(int num_poly);

  void Mutate();

 private:

  std::vector<Polygon> polygons_;

};

}  // namespace poly

#endif
