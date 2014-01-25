#include "core/genetic_effect.h"
#include "poly/polygon_genome_factory.h"
#include "poly/polygon_renderer.h"

using genart::core::EffectParams;
using genart::core::GeneticEffect;
using genart::core::GenomeFactory;
using genart::core::GenomeRenderer;

namespace genart {

std::unique_ptr<GeneticEffect> NewGeneticEffect(const EffectParams& params) {
  std::unique_ptr<GenomeRenderer> renderer;
  std::unique_ptr<GenomeFactory> genome_factory;

  // Only option now is poly.
  renderer.reset(new poly::OfflinePolygonRenderer());
  genome_factory.reset(new poly::PolygonGenomeFactory());
  
  return std::unique_ptr<GeneticEffect>(
      new GeneticEffect(params,
                        std::move(renderer),
                        std::move(genome_factory)));
}

}  // namespace genart
