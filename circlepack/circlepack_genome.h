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

// TODO(piotrf): move this to util
struct RGBA {
  RGBA() {}
  RGBA(double _r, double _g, double _b, double _a)
      : r(_r), g(_g), b(_b), a(_a) {}
  double r, g, b, a;
};


class OfflineCirclepackRenderer;

class CirclepackGenome : public core::Genome {
 public:
  typedef std::pair<Point, RGBA> PointColor;
  
  explicit CirclepackGenome(OfflineCirclepackRenderer* renderer);
  CirclepackGenome(OfflineCirclepackRenderer* renderer,
                   const std::vector<PointColor> points);
  CirclepackGenome(OfflineCirclepackRenderer* renderer,
                   const core::MutationParams& params,
                   int num_points);
  virtual ~CirclepackGenome();

  virtual std::unique_ptr<core::Genome> Clone() const;
  virtual void Mutate(const core::MutationParams& params);
  virtual std::unique_ptr<image::Image> Render() const;

  std::vector<PointColor> points() const { return points_; }
  
 private:
  OfflineCirclepackRenderer* renderer_;  // Not owned.

  std::vector<PointColor> points_;
};

}  // namespace circlepack
}  // namespace genart

#endif  // GENART_CIRCLEPACK_CIRCLEPACK_GENOME_H_
