#include "image/image.h"
#include "poly/genome.h"
#include "poly/polygon_effect.h"

namespace poly {

PolygonEffect::PolygonEffect() {
}

PolygonEffect::~PolygonEffect() {
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
  Genome mother, child;

  // Start with a 1 polygon random mother.
  mother.Randomize(1);

  /*  const image::Image* RenderImage(mother);
  double last_fitness = fitness_->Evaluate(input_, );
  double start_fitness = last_fitness;
  
  for (int i = 0; i < params.max_generations(); ++i) {
    // TODO(piotrf): may have to create output for the visitors.
    for (visitor : visitors_) {
      if (i % visitor.interval) {
        visitor.visitor->Visit(output_);
      }
    }

    child = mother;
    child.Mutate(params.mutation_rates());
    double child_fitness = fitness_->Evaluate(child);
    if (child_fitness < last_fitness) {
      mother = child;
      last_fitness = child_fitness;
    }
  }
  */
  // TODO(piotrf): may have to create final output.
}


}
