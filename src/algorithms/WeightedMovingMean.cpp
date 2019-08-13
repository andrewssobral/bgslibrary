#include "WeightedMovingMean.h"

using namespace bgslibrary::algorithms;

WeightedMovingMean::WeightedMovingMean() :
  IBGS(quote(WeightedMovingMean)),
  enableWeight(true), enableThreshold(true), threshold(15)
{
  debug_construction(WeightedMovingMean);
  initLoadSaveConfig(algorithmName);
}

WeightedMovingMean::~WeightedMovingMean() {
  debug_destruction(WeightedMovingMean);
}

void WeightedMovingMean::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_input_prev_1.empty()) {
    img_input.copyTo(img_input_prev_1);
    return;
  }

  if (img_input_prev_2.empty()) {
    img_input_prev_1.copyTo(img_input_prev_2);
    img_input.copyTo(img_input_prev_1);
    return;
  }

  cv::Mat img_input_f(img_input.size(), CV_32F);
  img_input.convertTo(img_input_f, CV_32F, 1. / 255.);

  cv::Mat img_input_prev_1_f(img_input.size(), CV_32F);
  img_input_prev_1.convertTo(img_input_prev_1_f, CV_32F, 1. / 255.);

  cv::Mat img_input_prev_2_f(img_input.size(), CV_32F);
  img_input_prev_2.convertTo(img_input_prev_2_f, CV_32F, 1. / 255.);

  cv::Mat img_background_f(img_input.size(), CV_32F);

  if (enableWeight)
    img_background_f = ((img_input_f * 0.5) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.2));
  else
    img_background_f = ((img_input_f)+(img_input_prev_1_f)+(img_input_prev_2_f)) / 3.0;

  double minVal, maxVal;
  minVal = 0.; maxVal = 1.;
  img_background_f.convertTo(img_background, CV_8U, 255.0 / (maxVal - minVal), -minVal);

  cv::absdiff(img_input, img_background, img_foreground);

  if (img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if (enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  img_input_prev_1.copyTo(img_input_prev_2);
  img_input.copyTo(img_input_prev_1);

  firstTime = false;
}

void WeightedMovingMean::save_config(cv::FileStorage &fs) {
  fs << "enableWeight" << enableWeight;
  fs << "enableThreshold" << enableThreshold;
  fs << "threshold" << threshold;
  fs << "showOutput" << showOutput;
}

void WeightedMovingMean::load_config(cv::FileStorage &fs) {
  fs["enableWeight"] >> enableWeight;
  fs["enableThreshold"] >> enableThreshold;
  fs["threshold"] >> threshold;
  fs["showOutput"] >> showOutput;
}
