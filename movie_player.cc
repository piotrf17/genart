#include <chrono>
#include <iostream>
#include <fstream>
#include <memory>

#include <gflags/gflags.h>
#include <GL/gl.h>

#include "poly/polygon_image.pb.h"
#include "poly/polygon.h"
#include "poly/polygon_render.h"
#include "poly/util.h"
#include "util/recordio.h"
#include "util/window.h"

DEFINE_string(input, "", "Source of input movie.");
DEFINE_double(fps, 24.999, "Target framerate for playback.");
DEFINE_bool(stepped_playback, false, "Freeze after every frame.");

using std::chrono::monotonic_clock;
using std::chrono::duration;
using std::chrono::time_point;

namespace genart {

class MovieWindow : public util::Window2d {
 public:
  MovieWindow(int width, int height, const std::string& filename, double fps)
      : util::Window2d(width, height, "GenView"),
        movie_file_(filename),
        movie_reader_(&movie_file_),
        last_frame_time_(time_point<monotonic_clock>::min()),
        spf_(1 / fps),
        pause_(false) {
  }
  
  virtual ~MovieWindow() {
  }

 protected:
  virtual bool Init() {
    if (!Window2d::Init()) {
      return false;
    }
    glShadeModel(GL_FLAT);
    glClearColor(0.0, 0.0, 0.0, 1);
    glDisable(GL_DEPTH_TEST);
    glFlush();
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    return true;
  }
  
  virtual void Keypress(unsigned int key) {
    switch (key) {
      case XK_Escape:
        Close();
        break;
      case XK_space:
        pause_ = !pause_;
        break;
    }
  }
  
  virtual void Draw() {
    auto now = monotonic_clock::now();
    if (!pause_ &&
        (last_frame_time_ == time_point<monotonic_clock>::min() ||
         now - last_frame_time_ >= spf_)) {
      poly::output::PolygonImage frame;
      if (movie_reader_.ReadProtocolMessage(&frame)) {
        std::vector<poly::Polygon> frame_polygons;
        PolygonProtoToVector(frame, &frame_polygons);
        glClear(GL_COLOR_BUFFER_BIT);
        renderer_.Render(frame_polygons,
                         frame.width(), frame.height());
      }
      last_frame_time_ = now;
    }
  }

 private:
  // Input file and record reader for movie frames.
  std::ifstream movie_file_;
  util::RecordReader movie_reader_;

  // An object to render polygons to the screne.
  poly::PolygonRender renderer_;

  // Time keeping for movie playback.
  time_point<monotonic_clock> last_frame_time_;
  duration<double> spf_;
  bool pause_;
};

}  // namespace genart

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  // Get the height and width from the first frame.
  std::ifstream input_movie(FLAGS_input);
  util::RecordReader record_reader(&input_movie);
  poly::output::PolygonImage frame;
  if (!record_reader.ReadProtocolMessage(&frame)) {
    std::cerr << "Failed to open file:" << FLAGS_input << std::endl;
    return 0;
  }
  record_reader.Close();

  genart::MovieWindow window(frame.width(), frame.height(), FLAGS_input,
                             FLAGS_fps);
  window.Run();
  
  return 0;
}
