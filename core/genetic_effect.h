// An image effect based on a very simple genetic algorithm.  This algorithm
// simply mutates the genome, and if the result is better according to some
// fitness function it keeps the new genome.

#ifndef GENART_CORE_GENETIC_EFFECT_H_
#define GENART_CORE_GENETIC_EFFECT_H_

#include <memory>
#include <vector>

#include "core/params.pb.h"
#include "image/image.h"

namespace genart {
namespace core {

class EffectVisitor;
class Fitness;
class Genome;
class GenomeFactory;

class GeneticEffect {
 public:
  GeneticEffect(image::Image input,
                EffectParams params,
                std::unique_ptr<GenomeFactory> genome_factory);
  virtual ~GeneticEffect();

  // Add a visitor.  The Visit function on the visitor will be called
  // every interval steps.
  void AddVisitor(int interval, EffectVisitor* visitor);

  std::unique_ptr<Genome> Render();
  std::unique_ptr<Genome> RenderFromInitial(
      const Genome& initial,
      double initial_fitness_threshold = 1.0);
  
 private:
  // Our copy of the reference image.
  image::Image input_;

  // Parameters controlling how the effect is rendered.
  EffectParams params_;

  // A factory for creating new blank or random genomes.
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

