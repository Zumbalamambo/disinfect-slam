#include "sr300.h"

const int SR300::FPS;
const int SR300::WIDTH;
const int SR300::HEIGHT;

SR300::SR300() : align_to_color_(RS2_STREAM_COLOR) {
  cfg_.enable_stream(RS2_STREAM_DEPTH, 
      WIDTH, HEIGHT, rs2_format::RS2_FORMAT_Z16, FPS);
  cfg_.enable_stream(RS2_STREAM_COLOR,
      WIDTH, HEIGHT, rs2_format::RS2_FORMAT_RGB8, FPS);
  pipe_profile_ = pipe_.start(cfg_);
}

SR300::~SR300() {
  pipe_.stop();
}

double SR300::get_depth_scale() const {
  auto sensor = pipe_profile_.get_device().first<rs2::depth_sensor>();
  return 1. / sensor.get_depth_scale();
}

rs2_intrinsics SR300::get_camera_intrinsics() const {
  auto color_stream = pipe_profile_.get_stream(RS2_STREAM_COLOR)
                                   .as<rs2::video_stream_profile>();
  return color_stream.get_intrinsics();
}

void SR300::get_rgbd_frame(cv::Mat *color_img, cv::Mat *depth_img) const {
  auto frameset = pipe_.wait_for_frames();
  frameset = align_to_color_.process(frameset);

  *color_img = cv::Mat(cv::Size(WIDTH, HEIGHT), CV_8UC3, 
      (void*)frameset.get_color_frame().get_data(), cv::Mat::AUTO_STEP);
  *depth_img = cv::Mat(cv::Size(WIDTH, HEIGHT), CV_16UC1,
      (void*)frameset.get_depth_frame().get_data(), cv::Mat::AUTO_STEP);
}
