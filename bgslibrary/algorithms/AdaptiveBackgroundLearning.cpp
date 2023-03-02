#include "AdaptiveBackgroundLearning.h"

using namespace bgslibrary::algorithms;

AdaptiveBackgroundLearning::AdaptiveBackgroundLearning() :
  IBGS(quote(AdaptiveBackgroundLearning)),
  alpha(0.05), limit(-1), counter(0), minVal(0.0),
  maxVal(1.0), enableThreshold(true), threshold(15)
{
  debug_construction(AdaptiveBackgroundLearning);
  initLoadSaveConfig(algorithmName);
}

AdaptiveBackgroundLearning::~AdaptiveBackgroundLearning() {
  debug_destruction(AdaptiveBackgroundLearning);
}

void AdaptiveBackgroundLearning::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_background.empty())
    img_input.copyTo(img_background);

  cv::Mat img_input_f(img_input.size(), CV_32F);
  img_input.convertTo(img_input_f, CV_32F, 1. / 255.);

  cv::Mat img_background_f(img_background.size(), CV_32F);
  img_background.convertTo(img_background_f, CV_32F, 1. / 255.);

  cv::Mat img_diff_f(img_input.size(), CV_32F);
  cv::absdiff(img_input_f, img_background_f, img_diff_f);

  if ((limit > 0 && limit < counter) || limit == -1)
  {
    img_background_f = alpha*img_input_f + (1 - alpha)*img_background_f;

    cv::Mat img_new_background(img_input.size(), CV_8U);
    img_background_f.convertTo(img_new_background, CV_8U, 255.0 / (maxVal - minVal), -minVal);
    img_new_background.copyTo(img_background);

    if (limit > 0 && limit < counter)
      counter++;
  }

  //cv::Mat img_foreground(img_input.size(), CV_8U);
  img_diff_f.convertTo(img_foreground, CV_8UC1, 255.0 / (maxVal - minVal), -minVal);

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

  firstTime = false;
}

void AdaptiveBackgroundLearning::save_config(cv::FileStorage &fs) {
  fs << "alpha" << alpha;
  fs << "limit" << limit;
  fs << "enableThreshold" << enableThreshold;
  fs << "threshold" << threshold;
  fs << "showOutput" << showOutput;
}

void AdaptiveBackgroundLearning::load_config(cv::FileStorage &fs) {
  fs["alpha"] >> alpha;
  fs["limit"] >> limit;
  fs["enableThreshold"] >> enableThreshold;
  fs["threshold"] >> threshold;
  fs["showOutput"] >> showOutput;
}
