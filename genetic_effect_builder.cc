#include "core/genetic_effect.h"
#include "poly/polygon_genome_factory.h"
#include "poly/polygon_renderer.h"

using genart::core::EffectParams;
using genart::core::GeneticEffect;
using genart::core::GenomeFactory;

namespace genart {

std::unique_ptr<GeneticEffect> NewGeneticEffect(const image::Image& image,
                                                const EffectParams& params) {
  std::unique_ptr<GenomeFactory> genome_factory;

  // Only option now is poly.
  std::unique_ptr<poly::OfflinePolygonRenderer> renderer(
      new poly::OfflinePolygonRenderer(image.width(), image.height()));
  renderer->Init();
  genome_factory.reset(new poly::PolygonGenomeFactory(std::move(renderer)));
  
  return std::unique_ptr<GeneticEffect>(
      new GeneticEffect(image, params,
                        std::move(genome_factory)));
}

}  // namespace genart
