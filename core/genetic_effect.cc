#include <iostream>

#include "core/effect_visitor.h"
#include "core/fitness_l2_with_focii.h"
#include "core/genetic_effect.h"
#include "core/genome.h"
#include "core/genome_factory.h"
#include "core/genome_renderer.h"
#include "image/image.h"

namespace genart {
namespace core {

GeneticEffect::GeneticEffect(const EffectParams& params,
                             std::unique_ptr<GenomeRenderer> renderer,
                             std::unique_ptr<GenomeFactory> genome_factory)
    : params_(params),
      renderer_(std::move(renderer)),
      genome_factory_(std::move(genome_factory)),
      fitness_func_(new FitnessL2WithFocii) {
}

GeneticEffect::~GeneticEffect() {
}

void GeneticEffect::AddVisitor(int interval, EffectVisitor* visitor) {
  StepVisitor new_visitor;
  new_visitor.interval = interval;
  new_visitor.visitor = visitor;
  visitors_.push_back(new_visitor);
}

std::unique_ptr<Genome> GeneticEffect::Render(
    const image::Image& image) {
  std::unique_ptr<Genome> mother(genome_factory_->Random(
      params_.mutation_params()));
  return RenderFromInitial(image, *mother);
}

std::unique_ptr<Genome> GeneticEffect::RenderFromInitial(
    const image::Image& input,
    const Genome& initial,
    double initial_fitness_threshold) {
  std::unique_ptr<Genome> mother(genome_factory_->Copy(initial));
  std::unique_ptr<Genome> child(genome_factory_->Create());
  
  renderer_->Reset(input.width(), input.height());
  if (!renderer_->Init()) {
    // TODO(piotrf): how to handle failure?
    return mother;
  }

  // Calculate an initial fitness, based on a blank black image.
  std::unique_ptr<image::Image> image, best_image;
  image = renderer_->ToImage(*child);
  best_image.reset(new image::Image(*image));
  double last_fitness = fitness_func_->Evaluate(input, *image);
  std::cout << "Initial fitness = " << last_fitness << std::endl;
  const double start_fitness = last_fitness;

  // Check if our initial polygons pass the initial threshold, and if not
  // reset the mother to be a random genome.
  if (initial_fitness_threshold < 1.0) {
    image = renderer_->ToImage(*mother);
    double mother_fitness = fitness_func_->Evaluate(input, *image);
    if (initial_fitness_threshold * start_fitness <
        mother_fitness) {
      std::cout << "fitness frac initial = " << mother_fitness / start_fitness
                << " > threshold " << initial_fitness_threshold
                << " -> starting fresh" << std::endl;
      mother = genome_factory_->Random(params_.mutation_params());
    }
  }

  for (int i = 0; i < params_.max_generations(); ++i) {
    // Try a child mutation.
    child = genome_factory_->Copy(*mother);
    child->Mutate(params_.mutation_params());
    image = renderer_->ToImage(*child);
    double child_fitness = fitness_func_->Evaluate(input, *image);
    if (child_fitness < last_fitness) {
      mother = genome_factory_->Copy(*child);
      last_fitness = child_fitness;
      best_image.reset(new image::Image(*image));
    }
    if (i % 100 == 0) {
      std::cout << "fitness = " << last_fitness << " frac initial = "
                << last_fitness / start_fitness << std::endl;
    }

    if (params_.has_fitness_threshold() &&
        last_fitness <= start_fitness * params_.fitness_threshold()) {
      std::cout << "fitness = " << last_fitness << " frac initial = "
                << last_fitness / start_fitness
                << " which is under threshold of "
                << params_.fitness_threshold() << " -> FINISHED." << std::endl;
      break;
    }

    for (const auto& step_visitor : visitors_) {
      if (i % step_visitor.interval == 0) {
        step_visitor.visitor->Visit(*best_image);
      }
    }
  }

  return mother;
}

}  // namespace core
}  // namespace genart
