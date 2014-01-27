#include "poly/polygon_genome_factory.h"

#include "poly/polygon_genome.h"
#include "poly/polygon_renderer.h"

using genart::core::Genome;
using genart::core::MutationParams;

namespace genart {
namespace poly {

PolygonGenomeFactory::PolygonGenomeFactory(
    std::unique_ptr<OfflinePolygonRenderer> renderer)
    : renderer_(std::move(renderer)) {
}

std::unique_ptr<Genome> PolygonGenomeFactory::Create() const {
  return std::unique_ptr<Genome>(new PolygonGenome(renderer_.get()));
}

std::unique_ptr<Genome> PolygonGenomeFactory::Random(
    const MutationParams& mutation_params) const {
  std::unique_ptr<PolygonGenome> genome(new PolygonGenome(renderer_.get()));
  genome->Randomize(mutation_params, 1);
  return std::unique_ptr<Genome>(genome.release());
}

}  // namespace poly
}  // namespace genart
