#include "poly/genome.h"

namespace poly {

Genome::Genome() {
}

Genome::~Genome() {
}

void Genome::Randomize(int num_poly) {
  polygons_.resize(num_poly);
  for (int i = 0; i < num_poly; ++i) {
    polygons_[i].Randomize();
  }
}

void Genome::Mutate() {
}

}
