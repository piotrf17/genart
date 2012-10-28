#include "poly/genome.h"

#include <algorithm>
#include <iostream>

#include "poly/params.pb.h"
#include "poly/polygon_mutator.h"
#include "util/random.h"

using util::Random;

namespace poly {

Genome::Genome() {
}

Genome::~Genome() {
}

void Genome::Randomize(const MutationParams& params,
                       int num_poly) {
  polygons_.resize(num_poly);
  for (int i = 0; i < num_poly; ++i) {
    polygons_[i].Randomize(params.initial_size());
    polygons_[i].MutateColor(params.min_alpha(), params.max_alpha());
  }
}

void Genome::Mutate(const MutationParams& params) {
  const MutationRates& rates = params.rates();
  const int sum_of_rates =
      rates.point_add() +
      rates.point_delete() +
      rates.point_move() +
      rates.polygon_add() +
      rates.polygon_delete() +
      rates.polygon_move() +
      rates.color_change();
  int random = Random::Integer(sum_of_rates);
  if ((random -= rates.point_add()) < 0) {
    // Add a new point to a polygon.
    polygons_[Random::Integer(polygons_.size())].Mutate(
        PolygonMutatorAddPoint(params));
  } else if ((random -= rates.point_delete()) < 0) {
    // Delete a point from a polygon.
    if (polygons_.size() > 1) {
      polygons_[Random::Integer(polygons_.size())].Mutate(
          PolygonMutatorDeletePoint(params));
    }
  } else if ((random -= rates.point_move()) < 0) {
    // Move a point on a polygon.
    polygons_[Random::Integer(polygons_.size())].Mutate(
        PolygonMutatorMovePoint(params));
  } else if ((random -= rates.polygon_add()) < 0) {
    // Add a new polygon.
    if (static_cast<int>(polygons_.size()) < params.max_polygons()) {
      Polygon rand_poly;
      rand_poly.Randomize(params.initial_size());
      rand_poly.MutateColor(params.min_alpha(), params.max_alpha());
      polygons_.push_back(rand_poly);
    }
  } else if ((random -= rates.polygon_delete()) < 0) {
    // Delete a polygon.
    int pos = Random::Integer(polygons_.size());
    polygons_.erase(polygons_.begin() + pos);
  } else if ((random -= rates.polygon_move()) < 0) {
    // Swap 2 polygons position, affects the alpha compositing.
    if (polygons_.size() == 1) {
      return;
    }
    int i, j;
    i = Random::Integer(polygons_.size());
    do {
      j = Random::Integer(polygons_.size());
    } while (i == j);
    std::swap(polygons_[i], polygons_[j]);
  } else {
    // Change the color of a polygon.
    polygons_[Random::Integer(polygons_.size())].MutateColor(
        params.min_alpha(),
        params.max_alpha());
  }
}

}
