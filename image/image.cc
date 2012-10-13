#include "image/image.h"

#include <CImg.h>

namespace image {

Image::Image(const std::string& filename) {
  cimg_library::CImg<unsigned char> cimg(filename.c_str());
  width_ = cimg.width();
  height_ = cimg.height();
  pixels_ = new unsigned char[3 * width_ * height_];

  // CImg stores color channes seperately, i.e. the data is stored as:
  // R1, R2, R3, .... G1, G2, G3, .... B1, B2, B3
  // whereas OpenGL expectes interleaved channels:
  // R1, G1, B1, R2, G2, B2, R3, G3, B3, ...
  // In addition, OpenGL places the origin at the lower left of the image,
  // while CImg uses the upper right.
  const int n_pixels = width_ * height_;
  for (int i = 0; i < height_; ++i) {
    for (int j = 0; j < width_; ++j) {
      // Flip vertically.
      const int gl_pixel = i * width_ + j;
      const int cimg_pixel = (height_ - i - 1) * width_ + j;
      // Interleave channels.
      pixels_[3 * gl_pixel + 0] = cimg.data()[0 * n_pixels + cimg_pixel];
      pixels_[3 * gl_pixel + 1] = cimg.data()[1 * n_pixels + cimg_pixel];
      pixels_[3 * gl_pixel + 2] = cimg.data()[2 * n_pixels + cimg_pixel];
    }
  }
}

Image::~Image() {
  delete[] pixels_;
}

int Image::width() const {
  return width_;
}

int Image::height() const {
  return height_;
}

const unsigned char* Image::pixels() const {
  return pixels_;
}

unsigned char* Image::mutable_pixels() {
  return pixels_;
}


}  // namespace image
