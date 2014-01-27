#include "poly/polygon_genome_factory.h"

#include "poly/polygon_genome.h"

using genart::core::Genome;
using genart::core::MutationParams;

namespace genart {
namespace poly {

std::unique_ptr<Genome> PolygonGenomeFactory::Create() const {
  return std::unique_ptr<Genome>(new PolygonGenome());
}

std::unique_ptr<Genome> PolygonGenomeFactory::Random(
    const MutationParams& mutation_params) const {
  std::unique_ptr<PolygonGenome> genome(new PolygonGenome());
  genome->Randomize(mutation_params, 1);
  return std::unique_ptr<Genome>(genome.release());
}

}  // namespace poly
}  // namespace genart
