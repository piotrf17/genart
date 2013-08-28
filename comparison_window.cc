#include "comparison_window.h"

#include <GL/gl.h>

#include "image/image.h"

namespace genart {

namespace {

// Get the best scaling for an image of the given width and height into
// a window with the given width and height.
float GetBestScaling(int window_width, int window_height,
                     int image_width, int image_height) {
  float window_aspect = static_cast<float>(window_width) / window_height;
  float image_aspect = static_cast<float>(image_width) / image_height;
  if (window_aspect > image_aspect) {
    return static_cast<float>(window_height) / image_height;
  } else {
    return static_cast<float>(window_width) / image_width;
  }
}

// Get the percent of the window filled by an image of the given aspect ratio.
float GetWindowFill(float window_aspect, float image_aspect) {
  if (window_aspect > image_aspect) {
    return image_aspect / window_aspect;
  } else {
    return window_aspect / image_aspect;
  }
}

}

ComparisonWindow::ComparisonWindow()
    : util::Window2d(640, 480, "GenPic") {
  source_image_.reset(NULL);
  effect_image_.reset(NULL);
}

ComparisonWindow::~ComparisonWindow() {
}

void ComparisonWindow::SetSourceImage(const image::Image* image) {
  image_mutex_.lock();
  source_image_.reset(image);
  image_mutex_.unlock();
}

void ComparisonWindow::SetEffectImage(const image::Image* image) {
  image_mutex_.lock();
  effect_image_.reset(image);
  image_mutex_.unlock();
}

void ComparisonWindow::Keypress(unsigned int key) {
  switch (key) {
  }
}

void ComparisonWindow::Draw() {
  // Clear the screen.
  glClear(GL_COLOR_BUFFER_BIT);

  if (NULL == source_image_ || NULL == effect_image_) {
    return;
  }

  image_mutex_.lock();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  if (GetBestOrientation() == HORIZONTAL) {
    float scale = GetBestScaling(width(), height(),
                                 2 * source_image_->width(),
                                 source_image_->height());
    glRasterPos2i(0, 0);
    glPixelZoom(scale, scale);
    glDrawPixels(source_image_->width(),
                 source_image_->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 source_image_->pixels());
    glRasterPos2i(scale * source_image_->width(), 0);
    glDrawPixels(effect_image_->width(),
                 effect_image_->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 effect_image_->pixels());
  } else {
    float scale = GetBestScaling(width(), height(),
                                 source_image_->width(),
                                 2 * source_image_->height());
    glRasterPos2i(0, 0);
    glPixelZoom(scale, scale);
    glDrawPixels(source_image_->width(),
                 source_image_->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 source_image_->pixels());
    glRasterPos2i(0, scale * source_image_->height());
    glDrawPixels(effect_image_->width(),
                 effect_image_->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 effect_image_->pixels());
  }
  image_mutex_.unlock();
}

ComparisonWindow::Orientation ComparisonWindow::GetBestOrientation() {
  float window_aspect = static_cast<float>(width()) / height();
  float vertical_aspect = static_cast<float>(source_image_->width()) /
      (2 * source_image_->height());
  float horizontal_aspect = (2.0 * source_image_->width()) /
      source_image_->height();
  return (GetWindowFill(window_aspect, vertical_aspect) >
          GetWindowFill(window_aspect, horizontal_aspect)) ?
      VERTICAL : HORIZONTAL;
}

RenderProgressVisitor::RenderProgressVisitor(ComparisonWindow* window)
    : window_(window) {
}
  
void RenderProgressVisitor::Visit(const image::Image& latest) {
  window_->SetEffectImage(new image::Image(latest));
}

}  // namespace genart
