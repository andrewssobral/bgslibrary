#include "FrameDifference.h"

using namespace bgslibrary::algorithms;

FrameDifference::FrameDifference() :
  IBGS(quote(FrameDifference)),
  enableThreshold(true), threshold(15)
{
  debug_construction(FrameDifference);
  initLoadSaveConfig(algorithmName);
}

FrameDifference::~FrameDifference() {
  debug_destruction(FrameDifference);
}

void FrameDifference::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_background.empty()) {
    img_input.copyTo(img_background);
    return;
  }

  cv::absdiff(img_background, img_input, img_foreground);

  if (img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if (enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);

  img_input.copyTo(img_background);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void FrameDifference::save_config(cv::FileStorage &fs) {
  fs << "enableThreshold" << enableThreshold;
  fs << "threshold" << threshold;
  fs << "showOutput" << showOutput;
}

void FrameDifference::load_config(cv::FileStorage &fs) {
  fs["enableThreshold"] >> enableThreshold;
  fs["threshold"] >> threshold;
  fs["showOutput"] >> showOutput;
}
