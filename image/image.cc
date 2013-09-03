#include "image/image.h"

#include <CImg.h>
#include <opencv2/opencv.hpp>

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

Image::Image(const cv::Mat& cv_image) {
  width_ = cv_image.cols;
  height_ = cv_image.rows;
  pixels_ = new unsigned char[3 * width_ * height_];
  
  const uint8_t* cv_data = static_cast<const uint8_t*>(cv_image.data);
  const int channels = cv_image.channels();
  
  // OpenCV uses BGR instead of RGB :(
  for (int i = 0; i < height_; ++i) {
    for (int j = 0; j < width_; ++j) {
      const int gl_pixel = i * width_ + j;
      const int cv_pixel = (height_ - i - 1) * width_ + j;
      pixels_[3 * gl_pixel + 0] = cv_data[channels * cv_pixel + 2];
      pixels_[3 * gl_pixel + 1] = cv_data[channels * cv_pixel + 1];
      pixels_[3 * gl_pixel + 2] = cv_data[channels * cv_pixel + 0];
    }
  }
}

Image::Image(unsigned char* pixels, int width, int height)
    : width_(width),
      height_(height),
      pixels_(pixels) {
}

Image::Image(const Image& image) {
  width_ = image.width_;
  height_ = image.height_;
  const int byte_size = 3 * width_ * height_;
  pixels_ = new unsigned char[byte_size];
  memcpy(pixels_, image.pixels_, byte_size);
}

Image::Image(Image&& image)
    : width_(image.width_),
      height_(image.height_),
      pixels_(image.pixels_) {
  image.pixels_ = nullptr;
}

Image::~Image() {
  delete[] pixels_;
}

Image& Image::operator= (const Image& image) {
  width_ = image.width_;
  height_ = image.height_;
  const int byte_size = 3 * width_ * height_;
  pixels_ = new unsigned char[byte_size];
  memcpy(pixels_, image.pixels_, byte_size);
  return *this;
}

Image& Image::operator= (Image&& image) {
  width_ = image.width_;
  height_ = image.height_;
  pixels_ = image.pixels_;
  image.pixels_ = nullptr;
  return *this;
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
