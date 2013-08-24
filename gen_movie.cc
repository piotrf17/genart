#include <iostream>

#include <gflags/gflags.h>
#include <opencv2/opencv.hpp>

DEFINE_string(input_movie, "", "Source movie.");

int main(int argc, char** argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  cv::VideoCapture src_video(FLAGS_input_movie);
  if (!src_video.isOpened()) {
    std::cerr << "Failed to open video file: " << FLAGS_input_movie
              << std::endl;
    return 1;
  }

  cv::Mat frame;
  
  for (;;) {
    if (!src_video.grab()) {
      std::cout << "no more frames!" << std::endl;
      break;
    }
    
    src_video.retrieve(frame);

    cv::namedWindow( "Display Image", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Display Image", frame );

    cv::waitKey(0);
  }
  
  return 0;
}
