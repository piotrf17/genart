#include <chrono>
#include <cstdlib>
#include <fstream>
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
#include "util/multi_thread_condition.h"
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
class ComparisonWindow : public util::Window {
 public:
  ComparisonWindow(util::MultiThreadCondition* done_condition)
      : util::Window("GenPic", 640, 480),
        done_condition_(done_condition) {
    source_image.reset(NULL);
    effect_image.reset(NULL);
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
  virtual void HandleClose() {
    done_condition_->Notify();
  }
  
  virtual void HandleKey(unsigned int state, unsigned int keycode) {
    switch (keycode) {
      case 39:  // 's'
        std::cout << "Saving the image." << std::endl;
        break;
    }
  }
  
  virtual void HandleDraw() {
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

 private:
  // The rendering class owns the actual image pixels.
  std::mutex image_mutex;
  std::unique_ptr<const image::Image> source_image;
  std::unique_ptr<const image::Image> effect_image;

  util::MultiThreadCondition* done_condition_;
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

  util::MultiThreadCondition done_condition;
  
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
    window.reset(new ComparisonWindow(&done_condition));
    window->SetSourceImage(new image::Image(src_image));
    // Attach a visitor for rendering intermediate output.
    render_progress.reset(new RenderProgress(window.get()));
    polygon_effect.AddVisitor(FLAGS_display_step, render_progress.get());
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
  std::thread render_thread([&](){
      polygon_effect.Render();

      // Save to a file.
      if (!FLAGS_output_image.empty()) {
        poly::SaveImageToFile(output_polygons, FLAGS_output_image);
      }
      if (!FLAGS_output_poly.empty()) {
        std::ofstream output_file(FLAGS_output_poly);
        output_polygons.SerializeToOstream(&output_file);
      }
      
      // Wait for keypress upon finish.
      std::cout << "Finally, rendering finished." << std::endl;
      //      getchar();

      // Notify main thread that we've finished, that also shuts
      // down the UI window.
      done_condition.Notify();
    });

  // Either the UI thread will finish (user closes the window), or
  // we'll finish rendering.
  done_condition.Wait();

  if (FLAGS_benchmark) {
    end = std::chrono::high_resolution_clock::now();
    int duration_milli =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end - start).count();
    std::cout << "BENCHMARK (ms): " << duration_milli << std::endl;
  }

  return 0;
}
