#include "poly/genome.h"

#include <algorithm>
#include <iostream>

#include "poly/params.pb.h"
#include "poly/polygon_mutator.h"

namespace poly {

namespace {
int Random(int range) {
  return static_cast<int>(rand() / static_cast<double>(RAND_MAX) * range);
}
}  // namespace

Genome::Genome() {
}

Genome::~Genome() {
}

void Genome::Randomize(int num_poly) {
  polygons_.resize(num_poly);
  for (int i = 0; i < num_poly; ++i) {
    polygons_[i].Randomize();
  }
}

void Genome::Mutate(const MutationRates& rates) {
  const int sum_of_rates =
      rates.point_add() +
      rates.point_delete() +
      rates.point_move() +
      rates.polygon_add() +
      rates.polygon_delete() +
      rates.polygon_move() +
      rates.color_change();
  int random = Random(sum_of_rates);
  if ((random -= rates.point_add()) < 0) {
    // Add a new point to a polygon.
    //    std::cout << "point add!" << std::endl;
    polygons_[Random(polygons_.size())].Mutate(
        PolygonMutatorAddPoint());
  } else if ((random -= rates.point_delete()) < 0) {
    // Delete a point from a polygon.
    //    std::cout << "point delete!" << std::endl;
    if (polygons_.size() > 1) {
      polygons_[Random(polygons_.size())].Mutate(
          PolygonMutatorDeletePoint());
    }
  } else if ((random -= rates.point_move()) < 0) {
    // Move a point on a polygon.
    //    std::cout << "point move!" << std::endl;
    polygons_[Random(polygons_.size())].Mutate(
        PolygonMutatorMovePoint());
  } else if ((random -= rates.polygon_add()) < 0) {
    // Add a new polygon.
    //    std::cout << "polygon add!" << std::endl;
    if (polygons_.size() < 125) {
      Polygon rand_poly;      rand_poly.Randomize();
      polygons_.push_back(rand_poly);
    }
  } else if ((random -= rates.polygon_delete()) < 0) {
    // Delete a polygon.
    //    std::cout << "polygon delete!" << std::endl;
    int pos = Random(polygons_.size());
    polygons_.erase(polygons_.begin() + pos);
  } else if ((random -= rates.polygon_move()) < 0) {
    // Swap 2 polygons position, affects the alpha compositing.
    //    std::cout << "polygon move!" << std::endl;
    if (polygons_.size() == 1) {
      return;
    }
    int i, j;
    i = Random(polygons_.size());
    do {
      j = Random(polygons_.size());
    } while (i == j);
    std::swap(polygons_[i], polygons_[j]);
  } else {
    // Change the color of a polygon.
    //    std::cout << "color change!" << std::endl;
    polygons_[Random(polygons_.size())].MutateColor();
  }
}

}
