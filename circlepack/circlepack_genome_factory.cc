#include "circlepack/circlepack_genome_factory.h"

#include "circlepack/circlepack_genome.h"
#include "circlepack/circlepack_renderer.h"

using genart::core::Genome;
using genart::core::MutationParams;

namespace genart {
namespace circlepack {

CirclepackGenomeFactory::CirclepackGenomeFactory(
    std::unique_ptr<OfflineCirclepackRenderer> renderer)
    : renderer_(std::move(renderer)) {
}

std::unique_ptr<Genome> CirclepackGenomeFactory::Create() const {
  return std::unique_ptr<Genome>(new CirclepackGenome(renderer_.get()));
}

std::unique_ptr<Genome> CirclepackGenomeFactory::Random(
    const MutationParams& mutation_params) const {
  return std::unique_ptr<Genome>(new CirclepackGenome(renderer_.get(),
                                                      mutation_params,
                                                      1));
}

}  // namespace circlepack
}  // namespace genart
