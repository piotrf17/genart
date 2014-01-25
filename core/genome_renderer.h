// A GenomeRenderer is an interface to an object that can render a genome
// into an image.  A renderer often can have heavy-weight graphics context
// and objects, and should be constructed once and re-used for all images.

#ifndef GENART_CORE_GENOME_RENDERER_H_
#define GENART_CORE_GENOME_RENDERER_H_

#include <memory>

namespace genart {

namespace image {
class Image;
}  // namespace image
  
namespace core {

class Genome;

class GenomeRenderer {
 public:
  
  virtual void Reset(int width, int height) = 0;
  
  virtual bool Init() = 0;

  virtual std::unique_ptr<image::Image> ToImage(const Genome& genome) = 0;

 protected:
};

}  // namespace core
}  // namespace genart

#endif
