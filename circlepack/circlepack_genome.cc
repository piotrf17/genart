#include "circlepack/circlepack_genome.h"

#include "core/params.pb.h"
#include "image/image.h"
#include "circlepack/circlepack_renderer.h"
#include "util/random.h"

using genart::core::Genome;
using genart::core::MutationParams;
using util::Random;

namespace genart {
namespace circlepack {

CirclepackGenome::CirclepackGenome(OfflineCirclepackRenderer* renderer)
    : renderer_(renderer) {
}

CirclepackGenome::CirclepackGenome(OfflineCirclepackRenderer* renderer,
                                   const std::vector<PointColor> points)
    : renderer_(renderer),
      points_(points) {
}

CirclepackGenome::CirclepackGenome(OfflineCirclepackRenderer* renderer,
                                   const MutationParams& params,
                                   int num_points)
    : renderer_(renderer) {
  for (int i = 0; i < num_points; ++i) {
    RGBA color;
    color.r = Random::Float();
    color.g = Random::Float();
    color.b = Random::Float();
    color.a = 1.0;
    points_.push_back(
        std::make_pair(Point(Random::Double(), Random::Double()), color));
  }
}

CirclepackGenome::~CirclepackGenome() {
}

std::unique_ptr<Genome> CirclepackGenome::Clone() const {
  return std::unique_ptr<Genome>(new CirclepackGenome(renderer_, points_));
}

void CirclepackGenome::Mutate(const MutationParams& params) {
  // TODO(piotrf): fill me in
}

std::unique_ptr<image::Image> CirclepackGenome::Render() const {
  return renderer_->Render(*this);
}

}  // namespace circlepack
}  // namespace genart
