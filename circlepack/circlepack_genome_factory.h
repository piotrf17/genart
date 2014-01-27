#ifndef CIRCLEPACK_GENOME_FACTORY_H_
#define CIRCLEPACK_GENOME_FACTORY_H_

#include "core/genome_factory.h"

namespace genart {
namespace circlepack {

class OfflineCirclepackRenderer;

class CirclepackGenomeFactory : public core::GenomeFactory {
 public:
  explicit CirclepackGenomeFactory(
      std::unique_ptr<OfflineCirclepackRenderer> renderer);
  
  virtual std::unique_ptr<core::Genome> Create() const;

  virtual std::unique_ptr<core::Genome> Random(
      const core::MutationParams& mutation_params) const;
    
 private:
  std::unique_ptr<OfflineCirclepackRenderer> renderer_;
};
  
}  // namespace circlepack
}  // namespace genart

#endif  // CIRCLEPACK_GENOME_FACTORY_H_
