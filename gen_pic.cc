#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include <gflags/gflags.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "image/image.h"
#include "poly/polygon_effect.h"
#include "util/window.h"

DEFINE_string(input_image, "../amelia.jpg",
              "Source image for creation.");
DEFINE_string(output_image, "../amelia_poly.jpg",
              "Image to output upon finish.");
DEFINE_string(effect_config, "",
              "Optional file of parameters for the effect.");
DEFINE_int32(display_step, 1,
             "Show current rendering side by side with image every this number "
             "of generations.  Set to 0 for no display");

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  // Create a new renderer.
  poly::PolygonEffect polygon_effect;

  // Set the source image.
  image::Image src_image(FLAGS_input_image.c_str());
  polygon_effect.SetInput(&src_image);
  printf("%d %d\n", src_image.width(), src_image.height());
  
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
    google::protobuf::TextFormat::Parse(
        &config_stream,
        &effect_params);
  }
  polygon_effect.SetParams(effect_params);

  // Set a hook for a window to display output.
  std::unique_ptr<util::Window> window;
  if (FLAGS_display_step > 0) {
    // Create a window for output
    window.reset(new util::Window("GenArt", 600, 480));
    window->DrawImage(src_image, 0, 0);
    //    polygon_effect.AddVisitor(FLAGS_display_step, Visualizer);
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
