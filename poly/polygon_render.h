// Classes for online and offline rendering of a polygon image.

#ifndef GENART_POLY_POLYGON_RENDER_H
#define GENART_POLY_POLYGON_RENDER_H

#include <vector>

struct GLUtesselator;

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

  void Render(const AnimatedPolygonImage& image,
              int width, int height);
  
 private:
  GLUtesselator* tess_;
};

// Internally this class uses a PolygonRender to create the image, but it
// also creates and manages an offline OpenGL window.  All GLX context is
// managed, so threads can treat this class as a black box that creates
// pixel images from a polygon description.
class OfflinePolygonRender {
 public:
  OfflinePolygonRender(int width, int height);
  ~OfflinePolygonRender();

  // Create an offline rendering context with GLX.
  bool Init();

  image::Image* ToImage(const std::vector<Polygon>& polygons);
  
 private:
  PolygonRender render_;
  
  int width_, height_;
  OfflineGLWindow* win_;

  GLUtesselator* tess_;
};

}  // namespace poly

#endif
