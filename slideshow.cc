#include <fstream>
#include <iostream>
#include <memory>

#include <GL/gl.h>

#include "poly/animated_polygon_image.h"
#include "poly/polygon.h"
#include "poly/polygon_animator.h"
#include "poly/polygon_image.pb.h"
#include "poly/polygon_render.h"
#include "poly/util.h"
#include "util/window.h"

class MainWindow : public util::Window2d {
 public:
  MainWindow(poly::AnimatedPolygonImage* image)
      : util::Window2d(640, 480, "Slideshow"),
        image_(image),
        t_(0.0),
        t_pre(0.0) {
    image_->SetTime(0.0);
  }
  virtual ~MainWindow() {}

 protected:
  virtual void Keypress(unsigned int key) {
    switch (key) {
      case XK_Escape:
        Close();
        break;
    }
  }

  virtual void Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // For some reason this gets reset after every frame.
    // TODO(piotrf): figure out why and only set it once.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (t_pre < 1.0) {
      t_pre += 0.01;
    } else {
      t_ = std::min(1.0, t_ + 0.01);
    }
    image_->SetTime(t_);

    // Eiffel: 400 x 340
    // London: 400 x 288
    render_.Render(*image_, 400,
                   static_cast<int>(340 * (1 - t_) + 288 * t_));
  }

 private:
  poly::PolygonRender render_;

  poly::AnimatedPolygonImage* image_;

  double t_, t_pre;
};

int main(int argc, char** argv) {
  poly::output::PolygonImage img1, img2;

  std::ifstream infile1("/home/piotrf/Desktop/kitten-100.poly");
  img1.ParseFromIstream(&infile1);

  std::ifstream infile2("/home/piotrf/Desktop/puppy-100.poly");
  img2.ParseFromIstream(&infile2);

  std::cout << "image 1 has " << img1.polygon_size() << " polygons" << std::endl;
  std::cout << "image 2 has " << img2.polygon_size() << " polygons" << std::endl;

  std::vector<poly::Polygon> image1, image2;
  poly::PolygonProtoToVector(img1, &image1);
  poly::PolygonProtoToVector(img2, &image2);

  std::unique_ptr<poly::AnimatedPolygonImage>transition(
      poly::PolygonAnimator::Animate(image1, image2));
  
  MainWindow window(transition.get());
  window.Run();
}
