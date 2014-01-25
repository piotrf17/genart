#ifndef GENART_CORE_GENETIC_EFFECT_BUILDER_H_
#define GENART_CORE_GENETIC_EFFECT_BUILDER_H_

#include <memory>

namespace genart {

namespace core {
class EffectParams;
class GeneticEffect;
}  // namespace core

std::unique_ptr<core::GeneticEffect> NewGeneticEffect(
    const core::EffectParams& params);

}  // namespace genart

#endif  // GENART_CORE_GENETIC_EFFECT_BUILDER_H_
