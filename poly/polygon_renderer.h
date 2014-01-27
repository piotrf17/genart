// Classes for online and offline rendering of a polygon image.

#ifndef GENART_POLY_POLYGON_RENDERER_H_
#define GENART_POLY_POLYGON_RENDERER_H_

#include <memory>
#include <vector>

struct GLUtesselator;

namespace genart {

namespace image {
class Image;
}  // namespace image

namespace util {
class OfflineGLContext;
}  // namespace util

namespace poly {

class AnimatedPolygonImage;
class Polygon;
class PolygonGenome;

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
class OfflinePolygonRenderer {
 public:
  OfflinePolygonRenderer(int width, int height);
  ~OfflinePolygonRenderer();

  // WARNING: this is currently thread hostile.  Only one thread can call
  // this function for any OfflinePolygonRenderer object at any time.
  // TODO(piotrf): fix please!
  std::unique_ptr<image::Image> Render(const PolygonGenome& genome);
  
 private:
  std::unique_ptr<util::OfflineGLContext> context_;
  PolygonRender render_;
  
  int width_, height_;
};

}  // namespace poly
}  // namespace genart

#endif  // GENART_POLY_POLYGON_RENDERER_H_
