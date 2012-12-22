#include "poly/animated_polygon_image.h"

namespace poly {

//============= AnimatedPolygonImage =============//

AnimatedPolygonImage::AnimatedPolygonImage(
    const std::vector<Polygon> start,
    const std::vector<Polygon> end)
    : start_(start),
      end_(end) {
}
 
AnimatedPolygonImage::~AnimatedPolygonImage() {
}

void AnimatedPolygonImage::SetTime(double t) {
  t_ = t;
}

AnimatedPolygonImageIterator AnimatedPolygonImage::begin() const {
  return AnimatedPolygonImageIterator(
      start_.begin(), end_.begin(), t_);
}

AnimatedPolygonImageIterator AnimatedPolygonImage::end() const {
  return AnimatedPolygonImageIterator(
      start_.end(), end_.end(), t_);
}

//============= AnimatedPolygonImageIterator =============//

AnimatedPolygonImageIterator::AnimatedPolygonImageIterator(
    const std::vector<Polygon>::const_iterator& it1,
    const std::vector<Polygon>::const_iterator& it2,
    double t)
    : it1_(it1),
      it2_(it2),
      view_(t) {
}

AnimatedPolygonImageIterator::~AnimatedPolygonImageIterator() {
}

const AnimatedPolygon& AnimatedPolygonImageIterator::operator*() {
  view_.Set(&(*it1_), &(*it2_));
  return view_;
}

const AnimatedPolygon* AnimatedPolygonImageIterator::operator->() {
  return &(operator*());
}

AnimatedPolygonImageIterator& AnimatedPolygonImageIterator::operator++() {
  ++it1_;
  ++it2_;
  return *this;
}

bool AnimatedPolygonImageIterator::operator==(
    const AnimatedPolygonImageIterator& o) {
  return it1_ == o.it1_;
}

bool AnimatedPolygonImageIterator::operator!=(
    const AnimatedPolygonImageIterator& o) {
  return it1_ != o.it1_;
}

//============== AnimatedPolygon ==============//

AnimatedPolygon::AnimatedPolygon(double t)
    : t_(t) {
}

AnimatedPolygon::~AnimatedPolygon() {
}

void AnimatedPolygon::Set(const Polygon* p1,
                          const Polygon* p2) {
  p1_ = p1;
  p2_ = p2;
}

RGBA AnimatedPolygon::color() const {
  const RGBA& c1 = p1_->color();
  const RGBA& c2 = p2_->color();
  return RGBA((1 - t_) * c1.r + t_ * c2.r,
              (1 - t_) * c1.g + t_ * c2.g,
              (1 - t_) * c1.b + t_ * c2.b,
              (1 - t_) * c1.a + t_ * c2.a);
}

int AnimatedPolygon::num_points() const {
  return p1_->num_points();
}

AnimatedPolygonIterator AnimatedPolygon::begin() const {
  return AnimatedPolygonIterator(p1_->begin(), p2_->begin(), t_);
}

AnimatedPolygonIterator AnimatedPolygon::end() const {
  return AnimatedPolygonIterator(p1_->end(), p2_->end(), t_);
}

//=============== AnimatedPolygonIterator ==============//

AnimatedPolygonIterator::AnimatedPolygonIterator(
    const std::vector<Point>::const_iterator it1,
    const std::vector<Point>::const_iterator it2,
    double t)
    : it1_(it1),
      it2_(it2),
      t_(t) {
}

AnimatedPolygonIterator::~AnimatedPolygonIterator() {
}

const Point& AnimatedPolygonIterator::operator*() {
  p.x = (1 - t_) * it1_->x + t_ * it2_->x;
  p.y = (1 - t_) * it1_->y + t_ * it2_->y;
  return p;
}

const Point* AnimatedPolygonIterator::operator->() {
  return &(operator*());
}

AnimatedPolygonIterator& AnimatedPolygonIterator::operator++() {
  ++it1_;
  ++it2_;
  return *this;
}

bool AnimatedPolygonIterator::operator==(
    const AnimatedPolygonIterator& o) {
  return it1_ == o.it1_;
}

bool AnimatedPolygonIterator::operator!=(
    const AnimatedPolygonIterator& o) {
  return it1_ != o.it1_;
}

}  // namespace poly
