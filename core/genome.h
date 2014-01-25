#ifndef GENART_CORE_GENOME_H_
#define GENART_CORE_GENOME_H_

namespace genart {
namespace core {

class MutationParams;

class Genome {
 public:
  virtual ~Genome() {}

  virtual void Mutate(const MutationParams& mutation_params) = 0;
  
 private:
};

}  // namespace core
}  // namespace genart

#endif  // GENART_CORE_GENOME_H_
