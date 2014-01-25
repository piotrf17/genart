// An image effect based on a very simple genetic algorithm.  This algorithm
// simply mutates the genome, and if the result is better according to some
// fitness function it keeps the new genome.

#ifndef GENART_CORE_GENETIC_EFFECT_H_
#define GENART_CORE_GENETIC_EFFECT_H_

#include <memory>
#include <vector>

#include "core/params.pb.h"

namespace genart {
namespace image {
class Image;
}  // namespace image
  
namespace core {

class EffectVisitor;
class Fitness;
class Genome;
class GenomeRenderer;
class GenomeFactory;

class GeneticEffect {
 public:
  GeneticEffect(const EffectParams& params,
                std::unique_ptr<GenomeRenderer> renderer,
                std::unique_ptr<GenomeFactory> genome_factory);
  virtual ~GeneticEffect();

  // Add a visitor.  The Visit function on the visitor will be called
  // every interval steps.
  void AddVisitor(int interval, EffectVisitor* visitor);

  std::unique_ptr<Genome> Render(const image::Image& image);
  std::unique_ptr<Genome> RenderFromInitial(
      const image::Image& input,
      const Genome& initial,
      double initial_fitness_threshold = 1.0);
  
 private:
  EffectParams params_;

  std::unique_ptr<GenomeRenderer> renderer_;
  std::unique_ptr<GenomeFactory> genome_factory_;

  // Visitors on intermediate output.
  struct StepVisitor {
    int interval;
    EffectVisitor* visitor;
  };
  std::vector<StepVisitor> visitors_;

  // Evaluation function for images.
  std::unique_ptr<Fitness> fitness_func_;
};

}  // namespace core
}  // namespace genart

#endif

