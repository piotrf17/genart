#include <glog/logging.h>

#include "circlepack/circlepack_genome_factory.h"
#include "circlepack/circlepack_params.pb.h"
#include "circlepack/circlepack_renderer.h"
#include "core/genetic_effect.h"
#include "poly/polygon_genome_factory.h"
#include "poly/poly_params.pb.h"
#include "poly/polygon_renderer.h"

using genart::core::EffectParams;
using genart::core::GeneticEffect;
using genart::core::GenomeFactory;

namespace genart {

std::unique_ptr<GeneticEffect> NewGeneticEffect(const image::Image& image,
                                                const EffectParams& params) {
  std::unique_ptr<GenomeFactory> genome_factory;

  // We figure out what type of effect to create based on the params.
  if (params.mutation_params().HasExtension(
          genart::poly::mutation_params_ext)) {
    std::unique_ptr<poly::OfflinePolygonRenderer> renderer(
        new poly::OfflinePolygonRenderer(
            image.width(), image.height()));
    genome_factory.reset(
        new poly::PolygonGenomeFactory(std::move(renderer)));
  }
  if (params.mutation_params().HasExtension(
          genart::circlepack::mutation_params_ext)) {
    CHECK(genome_factory.get() == nullptr);
    std::unique_ptr<circlepack::OfflineCirclepackRenderer> renderer(
        new circlepack::OfflineCirclepackRenderer(
            image.width(), image.height()));
    genome_factory.reset(
        new circlepack::CirclepackGenomeFactory(std::move(renderer)));
  }
  
  return std::unique_ptr<GeneticEffect>(
      new GeneticEffect(image, params,
                        std::move(genome_factory)));
}

}  // namespace genart
