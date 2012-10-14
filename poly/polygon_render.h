// A class for doing offline rendering of a genome.

#ifndef GENART_POLY_POLYGON_RENDER_H
#define GENART_POLY_POLYGON_RENDER_H

struct GLUtesselator;

namespace image {
  class Image;
}  // namespace image

namespace poly {

class Genome;
struct OfflineGLWindow;

class PolygonRender {
 public:
  PolygonRender(int width, int height);
  ~PolygonRender();

  // Create an offline rendering context with GLX.
  void Init();

  image::Image* ToImage(const Genome& genome);
  
 private:
  int width_, height_;
  OfflineGLWindow* win_;

  GLUtesselator* tess_;
};

}  // namespace poly

#endif
