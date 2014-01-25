#ifndef GENART_IMAGE_IMAGE_H
#define GENART_IMAGE_IMAGE_H

#include <string>

namespace cv {
  class Mat;
}  // namespace cv

namespace genart {
namespace image {

class Image {
 public:
  // Initialize from the image stored in the given file, uses CImg
  // to load jpg, png, bmp, etc.
  explicit Image(const std::string& filename);

  // Initialize from an OpenCV Mat type.
  explicit Image(const cv::Mat& cv_image);

  // Initialize from an array of pixels, the Image object takes ownership
  // of the array.
  Image(unsigned char* pixels, int width, int height);

  // Various constructors / destructors / assignment operators.
  Image(const Image& image);
  Image(Image&& image);
  
  ~Image();

  Image& operator= (const Image& image);
  Image& operator= (Image&& image);

  // Converting to other formats.
  cv::Mat ToMat() const;

  // Accessors.
  int width() const;
  int height() const;

  const unsigned char* pixels() const;
  unsigned char* mutable_pixels();

 private:
  int width_, height_;
  unsigned char* pixels_;
};

}  // namespace image
}  // namespace genart

#endif
