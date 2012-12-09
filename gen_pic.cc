#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <boost/thread.hpp>
#include <gflags/gflags.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "image/image.h"
#include "poly/effect_visitor.h"
#include "poly/polygon_effect.h"
#include "poly/polygon_image.pb.h"
#include "poly/util.h"
#include "util/svg_writer.h"
#include "util/window.h"

DEFINE_string(input_image, "", "Source image for creation.");
DEFINE_string(output_image, "", "Image to output upon finish.");
DEFINE_string(effect_config, "",
              "Optional file of parameters for the effect.");
DEFINE_int32(display_step, 1,
             "Show current rendering side by side with image every this number "
             "of generations.  Set to 0 for no display");

// A window that shows a side by side comparison of the original and the
// image our effect has created.
class ComparisonWindow : public util::Window {
 public:
  ComparisonWindow() :
      util::Window("GenPic", 640, 480) {
    source_image = NULL;
    effect_image = NULL;
  }
  virtual ~ComparisonWindow() {}
  

  // This takes ownership of the image!
  void SetSourceImage(const image::Image* image) {
    image_mutex.lock();
    source_image.reset(image);
    image_mutex.unlock();
  }

  // This takes ownership of the image!
  void SetEffectImage(const image::Image* image) {
    image_mutex.lock();
    effect_image.reset(image);
    image_mutex.unlock();
  }
  
 protected:
  virtual void HandleKey(unsigned int state, unsigned int keycode) {
    switch (keycode) {
      case 39:  // 's'
        std::cout << "Saving the image." << std::endl;
        break;
    }
  }
  
  virtual void Draw() {
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);

    if (NULL == source_image || NULL == effect_image) {
      return;
    }

    image_mutex.lock();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glRasterPos2i(0, 0);
    glDrawPixels(source_image->width(),
                 source_image->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 source_image->pixels());
    glRasterPos2i(source_image->width(), 0);
    glDrawPixels(effect_image->width(),
                 effect_image->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 effect_image->pixels());
    image_mutex.unlock();
  }

  // The rendering class owns the actual image pixels.
  boost::mutex image_mutex;
  std::unique_ptr<const image::Image> source_image;
  std::unique_ptr<const image::Image> effect_image;
};

// A visitor class that updates our comparison window with the latest rendering.
class RenderProgress : public poly::EffectVisitor {
 public:
  explicit RenderProgress(ComparisonWindow* window)
      : window_(window) {
  }
  
  virtual void Visit(const image::Image& latest) {
    window_->SetEffectImage(new image::Image(latest));
  }

 private:
  ComparisonWindow* window_;
};

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (XInitThreads() == 0) {
    std::cout << "Failed to initialize thread support in xlib." << std::endl;
    return 1;
  }
  
  // Create a new renderer.
  poly::PolygonEffect polygon_effect;

  // Set the source image.
  image::Image src_image(FLAGS_input_image.c_str());
  polygon_effect.SetInput(&src_image);

  std::cout << src_image.width() << " " << src_image.height() << std::endl;
  
  // Set rendering parameters.
  poly::EffectParams effect_params;
  if (FLAGS_effect_config != "") {
    std::ifstream config_file(FLAGS_effect_config.c_str());
    if (!config_file.is_open()) {
      std::cout << "ERROR: couldn't open effect config file: "
                << FLAGS_effect_config << std::endl;
      return 1;
    }
    google::protobuf::io::IstreamInputStream config_stream(&config_file);
    if (!google::protobuf::TextFormat::Parse(
            &config_stream,
            &effect_params)) {
      std::cout << "ERROR: failed to parse config file." << std::endl;
      return 1;
    }
  }
  polygon_effect.SetParams(effect_params);

  // Set a hook for a window to display output.
  std::unique_ptr<ComparisonWindow> window;
  std::unique_ptr<RenderProgress> render_progress;
  if (FLAGS_display_step > 0) {
    // Create a window for output.
    window.reset(new ComparisonWindow());
    window->SetSourceImage(new image::Image(src_image));
    // Attach a visitor for rendering intermediate output.
    render_progress.reset(new RenderProgress(window.get()));
    polygon_effect.AddVisitor(FLAGS_display_step, render_progress.get());
  }

  // Render the image!
  poly::output::PolygonImage output_polygons;
  polygon_effect.SetOutput(&output_polygons);
  polygon_effect.Render();

  // Save the image.
  poly::SaveImageToFile(output_polygons, FLAGS_output_image);

  std::cout << "Finally, rendering finished." << std::endl;
  getchar();
  
  return 0;
}
