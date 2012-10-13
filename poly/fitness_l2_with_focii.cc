#include "fitness_l2_with_focii.h"

#include <cassert>
#include <cmath>

#include "image/image.h"

namespace poly {

FitnessL2WithFocii::FitnessL2WithFocii() {
}

FitnessL2WithFocii::~FitnessL2WithFocii() {
}

double FitnessL2WithFocii::Evaluate(
    const image::Image* ref_image,
    const image::Image* new_image) const {
  assert(ref_image->width() == new_image->width());
  assert(ref_image->height() == new_image->height());

  double sum = 0;
  for (int i = 0; i < 3 * ref_image->width() * ref_image->height(); i += 3) {
    sum += (ref_image->pixels()[i + 0] - new_image->pixels()[i + 0]) *
        (ref_image->pixels()[i + 0] - new_image->pixels()[i + 0]);
    sum += (ref_image->pixels()[i + 1] - new_image->pixels()[i + 1]) *
        (ref_image->pixels()[i + 1] - new_image->pixels()[i + 1]);
    sum += (ref_image->pixels()[i + 2] - new_image->pixels()[i + 2]) *
        (ref_image->pixels()[i + 2] - new_image->pixels()[i + 2]);
  }
  return sqrt(sum);
}

}
