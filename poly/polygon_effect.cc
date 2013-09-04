#include "poly/polygon_effect.h"

#include <iostream>
#include <memory>

#include "image/image.h"
#include "poly/effect_visitor.h"
#include "poly/fitness.h"
#include "poly/fitness_l2_with_focii.h"
#include "poly/genome.h"
#include "poly/polygon_image.pb.h"
#include "poly/polygon_render.h"
#include "poly/util.h"

namespace poly {

PolygonEffect::PolygonEffect() {
  // TODO(piotrf): Use a factory method.
  fitness_ = new FitnessL2WithFocii();
}

PolygonEffect::~PolygonEffect() {
  delete fitness_;
}

void PolygonEffect::SetInput(const image::Image* input) {
  input_ = input;
}

void PolygonEffect::SetOutput(output::PolygonImage* output) {
  output_ = output;
}

void PolygonEffect::SetParams(const EffectParams& params) {
  params_ = params;
}

void PolygonEffect::AddVisitor(int interval, EffectVisitor* visitor) {
  StepVisitor new_visitor;
  new_visitor.interval = interval;
  new_visitor.visitor = visitor;
  visitors_.push_back(new_visitor);
}

void PolygonEffect::Render() {
  Genome mother;
  mother.Randomize(params_.mutation_params(), 1);
  output::PolygonImage initial_polygons;
  VectorToPolygonProto(mother.polygons(), &initial_polygons);
  RenderFromInitial(initial_polygons);
}

void PolygonEffect::RenderFromInitial(
    const output::PolygonImage& initial_polygons,
    double initial_fitness_threshold) {
  OfflinePolygonRender render(input_->width(), input_->height());
  if (!render.Init()) {
    return;
  }

  std::vector<Polygon> initial_vector;
  PolygonProtoToVector(initial_polygons, &initial_vector);
  Genome mother(initial_vector), child;

  // Calculate an initial fitness, based on a blank black image.
  std::unique_ptr<image::Image> image;
  std::unique_ptr<image::Image> best_image;
  image.reset(render.ToImage(child.polygons()));
  best_image.reset(new image::Image(*image));
  double last_fitness = fitness_->Evaluate(input_, image.get());
  std::cout << "Initial fitness = " << last_fitness << std::endl;
  double start_fitness = last_fitness;

  // Check if our initial polygons pass the initial threshold, and if not
  // reset the mother to be a random polygon.
  if (initial_fitness_threshold < 1.0) {
    image.reset(render.ToImage(mother.polygons()));
    double mother_fitness = fitness_->Evaluate(input_, image.get());
    if (initial_fitness_threshold * start_fitness <
        mother_fitness) {
      std::cout << "fitness frac initial = " << mother_fitness / start_fitness
                << " > threshold " << initial_fitness_threshold
                << " -> starting fresh" << std::endl;
      mother.Randomize(params_.mutation_params(), 1);
    }
  }

  for (int i = 0; i < params_.max_generations(); ++i) {
    if (i % 100 == 0) {
      std::cout << "**Generation " << i << std::endl;
    }

    // Try a child mutation
    child = mother;
    child.Mutate(params_.mutation_params());
    image.reset(render.ToImage(child.polygons()));
    double child_fitness = fitness_->Evaluate(input_, image.get());
    if (child_fitness < last_fitness) {
      mother = child;
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

    for (auto it = visitors_.begin(); it != visitors_.end(); ++it) {
      if (i % it->interval == 0) {
        it->visitor->Visit(*best_image);
      }
    }
  }
  
  // Create the final output in a proto structure.
  VectorToPolygonProto(mother.polygons(), output_);
  output_->set_height(input_->height());
  output_->set_width(input_->width());
}

}
