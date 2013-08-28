#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

#include <gflags/gflags.h>
#include <GL/glx.h>
#include <opencv2/opencv.hpp>

#include "comparison_window.h"
#include "image/image.h"
#include "poly/polygon_effect.h"
#include "poly/polygon_image.pb.h"
#include "poly/util.h"

DEFINE_string(input_movie, "", "Source movie.");
DEFINE_int32(display_step, 1,
             "Show current rendering side by side with image every this number "
             "of generations.  Set to 0 for no display.");

using genart::ComparisonWindow;
using genart::RenderProgressVisitor;

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
