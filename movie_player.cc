#include <iostream>
#include <fstream>

#include <gflags/gflags.h>
#include <GL/gl.h>

#include "poly/polygon_image.pb.h"
#include "util/recordio.h"

DEFINE_string(input, "", "Source of input movie.");
DEFINE_double(fps, 24.999, "Target framerate for playback.");
DEFINE_bool(stepped_playback, false, "Freeze after every frame.");

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  std::ifstream input_movie(FLAGS_input);
  util::RecordReader record_reader(&input_movie);
  
  poly::output::PolygonImage frame;
  while (record_reader.ReadProtocolMessage(&frame)) {
    std::cout << "frame has this many polygons: "
              << frame.polygon_size() << std::endl;
  }
  
  return 0;
}
