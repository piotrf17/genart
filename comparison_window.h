#ifndef GENART_COMPARISON_WINDOW_H_
#define GENART_COMPARISON_WINDOW_H_

#include <memory>
#include <mutex>

#include "core/effect_visitor.h"
#include "util/window.h"

namespace genart {

namespace image {
class Image;
}  // namespace image

// A window that shows a side by side comparison of the original and the
// image our effect has created.
class ComparisonWindow : public util::Window2d {
 public:
  ComparisonWindow();
  virtual ~ComparisonWindow();
  
  // Set the source or effect image in the comparison.  These methods
  // are thread safe, and take ownership of the image.
  void SetSourceImage(const image::Image* image);
  void SetEffectImage(const image::Image* image);
  
 protected:  
  virtual void Keypress(unsigned int key);
  virtual void Draw();
  
 private:
  enum Orientation {
    VERTICAL,
    HORIZONTAL,
  };

  Orientation GetBestOrientation();
  
  // The rendering class owns the actual image pixels.
  std::mutex image_mutex_;
  std::unique_ptr<const image::Image> source_image_;
  std::unique_ptr<const image::Image> effect_image_;
};

// A visitor class that updates our comparison window with the latest rendering.
class RenderProgressVisitor : public core::EffectVisitor {
 public:
  explicit RenderProgressVisitor(ComparisonWindow* window);

  virtual void Visit(const image::Image& latest);

 private:
  ComparisonWindow* window_;
};

}  // namespace genart

#endif  // GENART_COMPARISON_WINDOW_H_
