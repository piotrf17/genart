// A genome is an abstract class representing the state of an artistic effect.

#ifndef GENART_CORE_GENOME_H_
#define GENART_CORE_GENOME_H_

#include <memory>

namespace genart {

namespace image {
class Image;
}  // namespace image
  
namespace core {

class MutationParams;

class Genome {
 public:
  virtual ~Genome() {}

  // Create and return a new copy of this genome.
  virtual std::unique_ptr<Genome> Clone() const = 0;
  
  // Apply a random mutation to this genome.  The MutationParams proto
  // can contain extensions with specific parameters for the derived class.
  virtual void Mutate(const MutationParams& mutation_params) = 0;

  // Render the genome into an image.
  virtual std::unique_ptr<image::Image> Render() const = 0;
  
 private:
};

}  // namespace core
}  // namespace genart

#endif  // GENART_CORE_GENOME_H_
