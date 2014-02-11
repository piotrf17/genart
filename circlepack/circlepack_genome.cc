#include "circlepack/circlepack_genome.h"

#include "core/params.pb.h"
#include "image/image.h"
#include "circlepack/circlepack_renderer.h"
#include "util/random.h"

using genart::core::Genome;
using genart::core::MutationParams;
using genart::core::MutationRates;
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
  const MutationRates& rates = params.rates();
  const int sum_of_rates =
      rates.point_move() +
      rates.color_change();
  int random = Random::Integer(sum_of_rates);
  int i = Random::Integer(points_.size());
  if ((random -= rates.point_move()) < 0) {
    // Move a point.
    double dx = Random::Double(-params.max_move(), params.max_move());
    double dy = Random::Double(-params.max_move(), params.max_move());
    
    points_[i].first.x += dx;
    points_[i].first.y += dy;
  } else {
    // Change the color of a point.
    points_[i].second.r = Random::Float();
    points_[i].second.g = Random::Float();
    points_[i].second.b = Random::Float();
  }
}

std::unique_ptr<image::Image> CirclepackGenome::Render() const {
  return renderer_->Render(*this);
}

}  // namespace circlepack
}  // namespace genart
