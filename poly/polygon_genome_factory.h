#ifndef GENART_POLY_POLYGON_GENOME_FACTORY_H_
#define GENART_POLY_POLYGON_GENOME_FACTORY_H_

#include "core/genome_factory.h"

namespace genart {
namespace poly {

class PolygonGenome;
  
class PolygonGenomeFactory : public core::GenomeFactory {
 public:

  virtual std::unique_ptr<core::Genome> Create() const;

  virtual std::unique_ptr<core::Genome> Random(
      const core::MutationParams& mutation_params) const;
    
 private:
};

}  // namespace poly
}  // namespace genart

#endif  // GENART_POLY_POLYGON_GENOME_FACTORY_H_
