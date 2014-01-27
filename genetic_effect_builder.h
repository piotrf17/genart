#ifndef GENART_CORE_GENETIC_EFFECT_BUILDER_H_
#define GENART_CORE_GENETIC_EFFECT_BUILDER_H_

#include <memory>

namespace genart {

namespace core {
class EffectParams;
class GeneticEffect;
}  // namespace core

namespace image {
class Image;
}  // namespace image

std::unique_ptr<core::GeneticEffect> NewGeneticEffect(
    const image::Image& image,
    const core::EffectParams& params);

}  // namespace genart

#endif  // GENART_CORE_GENETIC_EFFECT_BUILDER_H_
