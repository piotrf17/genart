#include <chrono>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

#include <gflags/gflags.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <GL/glx.h>

#include "comparison_window.h"
#include "genetic_effect_builder.h"
#include "core/params.pb.h"
#include "core/genetic_effect.h"
#include "core/genome.h"
#include "image/image.h"
#include "util/svg_writer.h"

DEFINE_string(input_image, "", "Source image for creation.");
DEFINE_string(output_image, "", "Image to output upon finish.");
DEFINE_string(output_poly, "", "If set, file to output raw polygons.");
DEFINE_string(effect_config, "",
              "Optional file of parameters for the effect.");
DEFINE_int32(display_step, 1,
             "Show current rendering side by side with image every this number "
             "of generations.  Set to 0 for no display");
DEFINE_bool(benchmark, false, "Run as a benchmark");

using genart::core::EffectParams;
using genart::core::GeneticEffect;
using genart::core::Genome;
using genart::image::Image;
using genart::ComparisonWindow;
using genart::NewGeneticEffect;
using genart::RenderProgressVisitor;

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (XInitThreads() == 0) {
    std::cerr << "Failed to initialize thread support in xlib." << std::endl;
    return 1;
  }

  // Load the source image.
  Image src_image(FLAGS_input_image.c_str());
  std::cout << "Loaded image " << FLAGS_input_image << "("
            << src_image.width() << "x" << src_image.height()
            << ")" << std::endl;
  
  // Load and parse rendering parameters.
  EffectParams effect_params;
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

  std::unique_ptr<GeneticEffect> effect = NewGeneticEffect(src_image,
                                                           effect_params);

  // Set a hook for a window to display output.
  std::unique_ptr<ComparisonWindow> window;
  std::unique_ptr<RenderProgressVisitor> render_progress;
  std::unique_ptr<std::thread> render_thread;
  if (FLAGS_display_step > 0) {
    // Create a window for output.
    window.reset(new ComparisonWindow());
    window->SetSourceImage(new Image(src_image));
    // Attach a visitor for rendering intermediate output.
    render_progress.reset(new RenderProgressVisitor(window.get()));
    effect->AddVisitor(FLAGS_display_step, render_progress.get());
    // Start the rendering thread.
    render_thread.reset(new std::thread(&ComparisonWindow::Run, window.get()));
  }

  // If running as a benchmark, use a given random seed and time the
  // rendering.
  std::chrono::time_point<std::chrono::system_clock> start, end;
  if (FLAGS_benchmark) {
    srand(0);
    start = std::chrono::high_resolution_clock::now();
  }
  
  // Render the image!
  std::unique_ptr<Genome> output = effect->Render();

  // Save to a file.
  /*
  if (!FLAGS_output_image.empty()) {
    poly::SaveImageToFile(output_polygons, FLAGS_output_image);
  }
  if (!FLAGS_output_poly.empty()) {
    std::ofstream output_file(FLAGS_output_poly);
    output_polygons.SerializeToOstream(&output_file);
  }
  */

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
