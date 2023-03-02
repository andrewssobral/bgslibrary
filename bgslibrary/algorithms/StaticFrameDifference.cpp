#include "StaticFrameDifference.h"

using namespace bgslibrary::algorithms;

StaticFrameDifference::StaticFrameDifference() :
  IBGS(quote(StaticFrameDifference)),
  enableThreshold(true), threshold(15)
{
  debug_construction(StaticFrameDifference);
  initLoadSaveConfig(algorithmName);
}

StaticFrameDifference::~StaticFrameDifference() {
  debug_destruction(StaticFrameDifference);
}

void StaticFrameDifference::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_background.empty())
    img_input.copyTo(img_background);

  cv::absdiff(img_input, img_background, img_foreground);

  if (img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if (enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void StaticFrameDifference::save_config(cv::FileStorage &fs) {
  fs << "enableThreshold" << enableThreshold;
  fs << "threshold" << threshold;
  fs << "showOutput" << showOutput;
}

void StaticFrameDifference::load_config(cv::FileStorage &fs) {
  fs["enableThreshold"] >> enableThreshold;
  fs["threshold"] >> threshold;
  fs["showOutput"] >> showOutput;
}
