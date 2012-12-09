// PolygonRenderer is a class to create polygon based visualizations of
// an image.  It uses a genetic algorithm to create the visualization, based
// on a fitness function that computes a color distance metric between two
// images.

#ifndef GENART_POLY_POLYGON_EFFECT_H
#define GENART_POLY_POLYGON_EFFECT_H

#include <vector>

#include "poly/params.pb.h"

namespace image {
class Image;
}  // namespace image

namespace poly {

class EffectVisitor;
class Fitness;
namespace output {
class PolygonImage;
}  // namespace output

struct StepVisitor {
  int interval;
  EffectVisitor* visitor;
};

class PolygonEffect {
 public:
  PolygonEffect();
  ~PolygonEffect();

  void SetInput(const image::Image* input);
  void SetOutput(output::PolygonImage* output);

  void SetParams(const EffectParams& params);
  
  void AddVisitor(int interval, EffectVisitor* visitor);

  void Render();
  
 private:
  // Rendering parameters.
  EffectParams params_;

  // Input source image.
  const image::Image* input_;
  
  // Final output.
  output::PolygonImage* output_;

  // Visitors on intermediate output
  std::vector<StepVisitor> visitors_;

  Fitness* fitness_;
};

}  // namespace poly

#endif
