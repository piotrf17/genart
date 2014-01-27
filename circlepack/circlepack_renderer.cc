#include "circlepack/circlepack_renderer.h"

#include <glog/logging.h>
#include <GL/gl.h>

#include "image/image.h"
#include "circlepack/circlepack_genome.h"
#include "util/offline_gl_context.h"

using genart::image::Image;
using genart::util::OfflineGLContext;

namespace genart {
namespace circlepack {

OfflineCirclepackRenderer::OfflineCirclepackRenderer(int width, int height)
    : context_(new OfflineGLContext(width, height)) {
}

std::unique_ptr<Image> OfflineCirclepackRenderer::Render(
    const CirclepackGenome& genome) {
  CHECK(context_->Acquire());
  glClear(GL_COLOR_BUFFER_BIT);

  // TODO(piotrf): render stuff here!

  return context_->GetImage();
}

}  // namespace circlepack
}  // namespace genart
