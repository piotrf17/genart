#include "poly/polygon_effect.h"

#include <iostream>

#include "image/image.h"
#include "poly/effect_visitor.h"
#include "poly/fitness.h"
#include "poly/fitness_l2_with_focii.h"
#include "poly/genome.h"
#include "poly/polygon_render.h"

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

void PolygonEffect::SetOutput(PolygonImage* output) {
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
  PolygonRender render(input_->width(), input_->height());
  render.Init();
  
  Genome mother, child;

  // Start with a 1 polygon random mother.
  mother.Randomize(1);

  // Calculate an initial fitness.
  const image::Image* mother_image = render.ToImage(mother);
  double last_fitness = fitness_->Evaluate(input_, mother_image);
  std::cout << "Initial fitness = " << last_fitness << std::endl;
  //  double start_fitness = last_fitness;
  
  for (int i = 0; i < params_.max_generations(); ++i) {
    // TODO(piotrf): may have to create output for the visitors.
    for (auto it = visitors_.begin(); it != visitors_.end(); ++it) {
      if (i % it->interval == 0j) {
        it->visitor->Visit(mother_image);
      }
    }

    /*    child = mother;
    child.Mutate(params.mutation_rates());
    double child_fitness = fitness_->Evaluate(child);
    if (child_fitness < last_fitness) {
      mother = child;
      last_fitness = child_fitness;
      }*/
  }
  
  // TODO(piotrf): may have to create final output.
}


}
