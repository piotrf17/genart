#include <gtest/gtest.h>

#include "polygon_mutator.h"

namespace poly {

TEST(LineIntersect, Parallel) {
  ASSERT_FALSE(internal::LineIntersect(
      Point(0.0, 0.0), Point(1.0, 0.0),
      Point(0.0, 2.0), Point(1.0, 2.0)));
}

TEST(LineIntersect, NotInInterval) {
  ASSERT_FALSE(internal::LineIntersect(
      Point(0.0, 0.0), Point(2.0, 0.0),
      Point(1.0, 1.0), Point(1.0, 3.0)));
}

TEST(LineIntersect, Intersecting) {
  ASSERT_TRUE(internal::LineIntersect(
      Point(0.0, 0.0), Point(2.0, 0.0),
      Point(1.0, 1.0), Point(1.0, -1.0)));
  ASSERT_TRUE(internal::LineIntersect(
      Point(0.60789, 0.756653), Point(0.393576, 0.801077),
      Point(0.451292, 0.6017), Point(0.554099, 0.852681)));
}

}
