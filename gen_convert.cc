// gen_convert
//
// Convert between the genart binary proto output files an standard image and
// video files.  The type of the output is determined by the extension, and
// whether the input is a movie or video.
//
// Supported output types:
// .avi

#include <string>

#include <gflags/gflags.h>
#include <opencv2/opencv.hpp>

#include "image/image.h"
#include "poly/polygon_image.pb.h"
#include "poly/polygon.h"
#include "poly/polygon_render.h"
#include "poly/util.h"
#include "util/recordio.h"

DEFINE_string(input, "", "Path to input genart file.");
DEFINE_string(output, "", "Path to intended output file.");
DEFINE_double(fps, 25, "If the output is a video, then the fps to encode at.");

namespace {

bool EndsWith(const std::string &full_string, const std::string &ending) {
  if (full_string.length() >= ending.length()) {
    return (0 == full_string.compare(full_string.length() - ending.length(),
                                     ending.length(), ending));
  } else {
    return false;
  }
}

bool OutputAsVideo(const std::string& input,
                   const std::string& output,
                   double fps) {
  // Open the input movie and read the first frame to get size info.
  poly::output::PolygonImage frame;
  std::ifstream input_file(input);
  util::RecordReader movie_reader(&input_file);
  if (!movie_reader.ReadProtocolMessage(&frame)) {
    std::cerr << "Could not open input movie: " << input << std::endl;
    return false;
  }

  // Open an output movie, use h264 encoding.
  cv::VideoWriter movie_writer;
  movie_writer.open(output, CV_FOURCC('X','V','I','D'), fps,
      cv::Size(frame.width(), frame.height()), true);
  if (!movie_writer.isOpened()) {
    std::cerr << "Could not open the output movie: " << output << std::endl;
    return false;
  }

  poly::OfflinePolygonRender render(frame.width(), frame.height());
  if (!render.Init()) {
    return false;
  }

  do {
    std::vector<poly::Polygon> frame_polygons;
    PolygonProtoToVector(frame, &frame_polygons);
    std::unique_ptr<image::Image> frame_image(render.ToImage(frame_polygons));
    
    cv::Mat mat_frame(frame_image->ToMat());
    movie_writer << mat_frame;
  } while (movie_reader.ReadProtocolMessage(&frame));

  return true;
}

}  // namespace

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (EndsWith(FLAGS_output, ".avi")) {
    OutputAsVideo(FLAGS_input, FLAGS_output, FLAGS_fps);
  } else {
    std::cerr << "Unknown output format for: " << FLAGS_output;
  }
}
