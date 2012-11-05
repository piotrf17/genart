#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "image/image.h"
#include "poly/effect_visitor.h"
#include "poly/polygon_effect.h"
#include "util/window.h"

DEFINE_string(input_image, "../eiffel.jpg",
              "Source image for creation.");
DEFINE_string(output_image, "../amelia_poly.jpg",
              "Image to output upon finish.");
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
  

  void SetSourceImage(const image::Image& image) {
    source_image = &image;
  }

  void SetEffectImage(const image::Image& image) {
    effect_image = &image;
  }
  
 private:
  virtual void Draw() {
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);

    if (NULL == source_image) {
      return;
    }
    glRasterPos2i(0, 0);
    glDrawPixels(source_image->width(),
                 source_image->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 source_image->pixels());

    if (NULL == effect_image) {
      return;
    }
    glRasterPos2i(source_image->width(), 0);
    glDrawPixels(effect_image->width(),
                 effect_image->height(),
                 GL_RGB, GL_UNSIGNED_BYTE,
                 effect_image->pixels());
  }

  const image::Image* source_image;
  const image::Image* effect_image;
};

// A visitor class that updates our comparison window with the latest rendering.
class RenderProgress : public poly::EffectVisitor {
 public:
  explicit RenderProgress(const image::Image& source, ComparisonWindow* window)
      : window_(window) {
    window_->SetSourceImage(source);
  }
  
  virtual void Visit(const image::Image* latest) {
    latest_.reset(latest);  // take ownership of the image.
    window_->SetEffectImage(*latest_);
  }

 private:
  std::unique_ptr<const image::Image> latest_;
  ComparisonWindow* window_;
};

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

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
    // Attach a visitor for rendering intermediate output.
    render_progress.reset(new RenderProgress(src_image, window.get()));
    polygon_effect.AddVisitor(FLAGS_display_step, render_progress.get());
  }

  // Render the image!
  /*  poly::PolygonImage output_polygons;
  polygon_renderer.SetOutput(&output_polygons);*/
  polygon_effect.Render();
  /*
  // Save the image.
  poly::SaveImageToFile(output_polygons, FLAGS_output_image);
                                */

  getchar();
  
  return 0;
}
