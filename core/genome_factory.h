// A GenomeFactory is an interface to a factory for producing genomes.

#ifndef GENART_CORE_GENOME_FACTORY_H_
#define GENART_CORE_GENOME_FACTORY_H_

#include <memory>

namespace genart {
namespace core {

class Genome;
class MutationParams;

class GenomeFactory {
 public:

  // Create a blank genome - if rendered this should produced a completely
  // black image.
  virtual std::unique_ptr<Genome> Create() const = 0;

  // Create a new random genome based on the given mutation parameters.
  virtual std::unique_ptr<Genome> Random(
      const MutationParams& mutation_params) const = 0;

 private:
};

}  // namespace core
}  // namespace genart

#endif // GENART_CORE_GENOME_FACTORY_H_
