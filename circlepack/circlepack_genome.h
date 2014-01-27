#ifndef GENART_CIRCLEPACK_CIRCLEPACK_GENOME_H_
#define GENART_CIRCLEPACK_CIRCLEPACK_GENOME_H_

#include <vector>

#include "core/genome.h"
#include "util/point.h"

namespace genart {

namespace core {
class MutationParams;
}  // namespace core

namespace circlepack {

class OfflineCirclepackRenderer;

class CirclepackGenome : public core::Genome {
 public:
  explicit CirclepackGenome(OfflineCirclepackRenderer* renderer);
  CirclepackGenome(OfflineCirclepackRenderer* renderer,
                   const std::vector<Point> points);
  CirclepackGenome(OfflineCirclepackRenderer* renderer,
                   const core::MutationParams& params,
                   int num_points);
  virtual ~CirclepackGenome();

  virtual std::unique_ptr<core::Genome> Clone() const;
  virtual void Mutate(const core::MutationParams& params);
  virtual std::unique_ptr<image::Image> Render() const;

 private:
  OfflineCirclepackRenderer* renderer_;  // Not owned.

  std::vector<Point> points_;
};

}  // namespace circlepack
}  // namespace genart

#endif  // GENART_CIRCLEPACK_CIRCLEPACK_GENOME_H_
