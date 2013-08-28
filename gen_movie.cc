#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#include <gflags/gflags.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <opencv2/opencv.hpp>

#include "image/image.h"
#include "poly/effect_visitor.h"
#include "poly/polygon_effect.h"
#include "poly/polygon_image.pb.h"
#include "poly/util.h"
#include "util/window.h"

DEFINE_string(input_movie, "", "Source movie.");
DEFINE_int32(display_step, 1,
             "Show current rendering side by side with image every this number "
             "of generations.  Set to 0 for no display.");

class ComparisonWindow : public util::Window2d {
 public:
  ComparisonWindow() : util::Window2d(640, 480, "GenMovies") {
    source_image.reset(nullptr);
    effect_image.reset(nullptr);
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
  virtual void Keypress(unsigned int key) {
  }

  virtual void Draw() {
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT);

    if (nullptr == source_image || nullptr == effect_image) {
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
};

// A visitor class that updates our comparison window with the latest rendering.
class RenderProgressVisitor : public poly::EffectVisitor {
 public:
  explicit RenderProgressVisitor(ComparisonWindow* window)
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

  // Open the video input file.
  cv::VideoCapture src_video(FLAGS_input_movie);
  if (!src_video.isOpened()) {
    std::cerr << "Failed to open video file: " << FLAGS_input_movie
              << std::endl;
    return 1;
  }

  // Create a new renderer.
  poly::PolygonEffect polygon_effect;

  // Set a hook for a window to display output.
  std::unique_ptr<ComparisonWindow> window;
  std::unique_ptr<RenderProgressVisitor> render_progress_visitor;
  std::unique_ptr<std::thread> render_thread;
  if (FLAGS_display_step > 0) {
    window.reset(new ComparisonWindow());
    // Attach a visitor for rendering intermediate output.
    render_progress_visitor.reset(new RenderProgressVisitor(window.get()));
    polygon_effect.AddVisitor(FLAGS_display_step,
                              render_progress_visitor.get());
    // Start the rendering thread.
    render_thread.reset(new std::thread(&ComparisonWindow::Run, window.get()));
  }
  
  for (;;) {
    // Grab the next frame from the video.
    if (!src_video.grab()) {
      std::cout << "Finished with the movie!" << std::endl;
      break;
    }
    cv::Mat frame;
    src_video.retrieve(frame);
    std::unique_ptr<image::Image> src_image(new image::Image(frame));

    window->SetSourceImage(new image::Image(*src_image.get()));
    window->SetEffectImage(new image::Image(*src_image.get()));

    getchar();
  }

  return 0;
}
