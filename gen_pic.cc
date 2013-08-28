#include <chrono>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

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
DEFINE_string(output_poly, "", "If set, file to output raw polygons.");
DEFINE_string(effect_config, "",
              "Optional file of parameters for the effect.");
DEFINE_int32(display_step, 1,
             "Show current rendering side by side with image every this number "
             "of generations.  Set to 0 for no display");
DEFINE_bool(benchmark, false, "Run as a benchmark");

// A window that shows a side by side comparison of the original and the
// image our effect has created.
class ComparisonWindow : public util::Window2d {
 public:
  ComparisonWindow()
      : util::Window2d(640, 480, "GenPic") {
    source_image_.reset(NULL);
    effect_image_.reset(NULL);
  }
  virtual ~ComparisonWindow() {}
  

  // This takes ownership of the image!
  void SetSourceImage(const image::Image* image) {
    image_mutex_.lock();
    source_image_.reset(image);
    image_mutex_.unlock();
  }

  // This takes ownership of the image!
  void SetEffectImage(const image::Image* image) {
    image_mutex_.lock();
    effect_image_.reset(image);
    image_mutex_.unlock();
  }
  
 protected:  
  virtual void Keypress(unsigned int key) {
    switch (key) {
      case XK_s:  // 's'
        std::cout << "Saving the image." << std::endl;
        break;
    }
  }

  virtual void Draw() {
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);

    if (NULL == source_image_ || NULL == effect_image_) {
      return;
    }

    image_mutex_.lock();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (GetBestOrientation() == HORIZONTAL) {
      float scale = GetBestScaling(2 * source_image_->width(),
                                   source_image_->height());
      glRasterPos2i(0, 0);
      glPixelZoom(scale, scale);
      glDrawPixels(source_image_->width(),
                   source_image_->height(),
                   GL_RGB, GL_UNSIGNED_BYTE,
                   source_image_->pixels());
      glRasterPos2i(scale * source_image_->width(), 0);
      glDrawPixels(effect_image_->width(),
                   effect_image_->height(),
                   GL_RGB, GL_UNSIGNED_BYTE,
                   effect_image_->pixels());
    } else {
      float scale = GetBestScaling(source_image_->width(),
                                   2 * source_image_->height());
      glRasterPos2i(0, 0);
      glPixelZoom(scale, scale);
      glDrawPixels(source_image_->width(),
                   source_image_->height(),
                   GL_RGB, GL_UNSIGNED_BYTE,
                   source_image_->pixels());
      glRasterPos2i(0, scale * source_image_->height());
      glDrawPixels(effect_image_->width(),
                   effect_image_->height(),
                   GL_RGB, GL_UNSIGNED_BYTE,
                   effect_image_->pixels());
    }
    image_mutex_.unlock();
  }

 private:
  enum Orientation {
    VERTICAL,
    HORIZONTAL,
  };

  float GetBestScaling(int image_width, int image_height) {
    float window_aspect = static_cast<float>(width()) / height();
    float image_aspect = static_cast<float>(image_width) / image_height;
    if (window_aspect > image_aspect) {
      return static_cast<float>(height()) / image_height;
    } else {
      return static_cast<float>(width()) / image_width;
    }
  }

  float GetWindowFill(float window_aspect, float image_aspect) {
    if (window_aspect > image_aspect) {
      return image_aspect / window_aspect;
    } else {
      return window_aspect / image_aspect;
    }
  }

  Orientation GetBestOrientation() {
    float window_aspect = static_cast<float>(width()) / height();
    float vertical_aspect = static_cast<float>(source_image_->width()) /
        (2 * source_image_->height());
    float horizontal_aspect = (2.0 * source_image_->width()) /
        source_image_->height();
    return (GetWindowFill(window_aspect, vertical_aspect) >
            GetWindowFill(window_aspect, horizontal_aspect)) ?
        VERTICAL : HORIZONTAL;
  }
  
  // The rendering class owns the actual image pixels.
  std::mutex image_mutex_;
  std::unique_ptr<const image::Image> source_image_;
  std::unique_ptr<const image::Image> effect_image_;
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
    std::cerr << "Failed to initialize thread support in xlib." << std::endl;
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
      std::cerr << "ERROR: couldn't open effect config file: "
                << FLAGS_effect_config << std::endl;
      return 1;
    }
    google::protobuf::io::IstreamInputStream config_stream(&config_file);
    if (!google::protobuf::TextFormat::Parse(
            &config_stream,
            &effect_params)) {
      std::cerr << "ERROR: failed to parse config file." << std::endl;
      return 1;
    }
  }
  polygon_effect.SetParams(effect_params);

  // Set a hook for a window to display output.
  std::unique_ptr<ComparisonWindow> window;
  std::unique_ptr<RenderProgress> render_progress;
  std::unique_ptr<std::thread> render_thread;
  if (FLAGS_display_step > 0) {
    // Create a window for output.
    window.reset(new ComparisonWindow());
    window->SetSourceImage(new image::Image(src_image));
    // Attach a visitor for rendering intermediate output.
    render_progress.reset(new RenderProgress(window.get()));
    polygon_effect.AddVisitor(FLAGS_display_step, render_progress.get());
    // Start the rendering thread.
    render_thread.reset(new std::thread(&ComparisonWindow::Run, window.get()));
  }

  poly::output::PolygonImage output_polygons;
  polygon_effect.SetOutput(&output_polygons);

  // If running as a benchmark, use a given random seed and time the
  // rendering.
  std::chrono::time_point<std::chrono::system_clock> start, end;
  if (FLAGS_benchmark) {
    srand(0);
    start = std::chrono::high_resolution_clock::now();
  }
  
  // Render the image!
  polygon_effect.Render();

  // Save to a file.
  if (!FLAGS_output_image.empty()) {
    poly::SaveImageToFile(output_polygons, FLAGS_output_image);
  }
  if (!FLAGS_output_poly.empty()) {
    std::ofstream output_file(FLAGS_output_poly);
    output_polygons.SerializeToOstream(&output_file);
  }

  std::cout << "Finally, rendering finished." << std::endl;

  if (FLAGS_benchmark) {
    end = std::chrono::high_resolution_clock::now();
    int duration_milli =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start).count();
    std::cout << "BENCHMARK (ms): " << duration_milli << std::endl;
  }

  return 0;
}
