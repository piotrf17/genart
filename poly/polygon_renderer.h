// Classes for online and offline rendering of a polygon image.

#ifndef GENART_POLY_POLYGON_RENDER_H
#define GENART_POLY_POLYGON_RENDER_H

#include <vector>

#include "core/genome_renderer.h"

struct GLUtesselator;

namespace genart {

namespace image {
class Image;
}  // namespace image

namespace poly {

class AnimatedPolygonImage;
class Polygon;
struct OfflineGLWindow;

// A class that will render a polygon image to the current display.  The
// thread calling it must have a current GLX context.  This class basically
// wraps the resources required to use the GL tesselator.  It should be
// reused between rendering images to save on tesselator construction time.
class PolygonRender {
 public:
  PolygonRender();
  ~PolygonRender();

  void Render(const std::vector<Polygon>& polygons,
              int width, int height);

  /*  void Render(const AnimatedPolygonImage& image,
      int width, int height);*/
  
 private:
  GLUtesselator* tess_;
};

// Internally this class uses a PolygonRender to create the image, but it
// also creates and manages an offline OpenGL window.  All GLX context is
// managed, so threads can treat this class as a black box that creates
// pixel images from a polygon description.
class OfflinePolygonRenderer : public core::GenomeRenderer {
 public:
  OfflinePolygonRenderer();
  ~OfflinePolygonRenderer();

  virtual void Reset(int width, int height);
  
  // Create an offline rendering context with GLX.
  // TODO(piotrf): the general effect code seems to think this can be
  // called multiple times on the same object, but I'm pretty sure that's
  // not true.
  virtual bool Init();

  virtual std::unique_ptr<image::Image> ToImage(const core::Genome& genome);
  
 private:
  PolygonRender render_;
  
  int width_, height_;
  OfflineGLWindow* win_;

  GLUtesselator* tess_;
};

}  // namespace poly
}  // namespace genart

#endif
