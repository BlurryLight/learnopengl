#pragma once

#include <iostream>

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

namespace pd {

class VideoCamera {
public:
  VideoCamera() {
    cam_ = cv::VideoCapture(0);
    if (!cam_.isOpened()) {

      std::cerr << " Camera cannot open" << std::endl;
      exit(-1);
    }
    cam_ >> frame_;
    height_ = frame_.rows;
    width_ = frame_.cols;
  }

  unsigned char *get_frame() {
    cam_.read(frame_);
    auto image = this->cvMat_to_tenture(frame_);
    return image;
  }
  std::pair<int, int> get_frame_size() {
    return std::make_pair(width_, height_);
  }

private:
  unsigned char *cvMat_to_tenture(cv::Mat &img) {
    // in opencv3 it has another name
    cvtColor(img, img, cv::COLOR_BGR2RGB);
    flip(img, img, -1);
    return img.data;
  }
  cv::VideoCapture cam_;
  cv::Mat frame_;
  int height_;
  int width_;
};

} // namespace pd
