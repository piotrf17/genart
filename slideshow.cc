#include <fstream>
#include <iostream>

#include <GL/gl.h>

#include "poly/polygon.h"
#include "poly/polygon_image.pb.h"
#include "poly/polygon_render.h"
#include "poly/util.h"
#include "util/window.h"

class MainWindow : public util::Window {
 public:
  MainWindow(const std::vector<poly::Polygon>& image)
      : util::Window("Slideshow", 640, 480),
        image_(image) {
  }
  virtual ~MainWindow() {}

 protected:
  virtual void HandleClose() {
  }

  virtual void HandleKey(unsigned int state, unsigned int keycode) {
  }

  virtual void HandleDraw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // For some reason this gets reset after every frame.
    // TODO(piotrf): figure out why and only set it once.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    render_.Render(image_, 300, 429);
  }

 private:
  poly::PolygonRender render_;

  const std::vector<poly::Polygon>& image_;
};

int main(int argc, char** argv) {
  poly::output::PolygonImage img1, img2;

  std::ifstream infile1("/home/piotrf/Desktop/eiffel.poly");
  img1.ParseFromIstream(&infile1);

  std::ifstream infile2("/home/piotrf/Desktop/london.poly");
  img2.ParseFromIstream(&infile2);

  std::cout << "image 1 has " << img1.polygon_size() << " polygons" << std::endl;
  std::cout << "image 2 has " << img2.polygon_size() << " polygons" << std::endl;

  std::vector<poly::Polygon> image1;
  poly::PolygonProtoToVector(img1, &image1);

  MainWindow window(image1);

  getchar();
}
