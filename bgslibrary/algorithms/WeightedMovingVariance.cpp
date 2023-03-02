#include "WeightedMovingVariance.h"

using namespace bgslibrary::algorithms;

WeightedMovingVariance::WeightedMovingVariance() :
  IBGS(quote(WeightedMovingVariance)),
  enableWeight(true), enableThreshold(true), threshold(15)
{
  debug_construction(WeightedMovingVariance);
  initLoadSaveConfig(algorithmName);
}

WeightedMovingVariance::~WeightedMovingVariance() {
  debug_destruction(WeightedMovingVariance);
}

void WeightedMovingVariance::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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

  // Weighted mean
  cv::Mat img_mean_f(img_input.size(), CV_32F);

  if (enableWeight)
    img_mean_f = ((img_input_f * 0.5) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.2));
  else
    img_mean_f = ((img_input_f * 0.3) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.3));

  // Weighted variance
  cv::Mat img_1_f(img_input.size(), CV_32F);
  cv::Mat img_2_f(img_input.size(), CV_32F);
  cv::Mat img_3_f(img_input.size(), CV_32F);
  cv::Mat img_4_f(img_input.size(), CV_32F);

  if (enableWeight)
  {
    img_1_f = computeWeightedVariance(img_input_f, img_mean_f, 0.5);
    img_2_f = computeWeightedVariance(img_input_prev_1_f, img_mean_f, 0.3);
    img_3_f = computeWeightedVariance(img_input_prev_2_f, img_mean_f, 0.2);
    img_4_f = (img_1_f + img_2_f + img_3_f);
  }
  else
  {
    img_1_f = computeWeightedVariance(img_input_f, img_mean_f, 0.3);
    img_2_f = computeWeightedVariance(img_input_prev_1_f, img_mean_f, 0.3);
    img_3_f = computeWeightedVariance(img_input_prev_2_f, img_mean_f, 0.3);
    img_4_f = (img_1_f + img_2_f + img_3_f);
  }

  // Standard deviation
  cv::Mat img_sqrt_f(img_input.size(), CV_32F);
  cv::sqrt(img_4_f, img_sqrt_f);
  cv::Mat img_sqrt(img_input.size(), CV_8U);
  double minVal, maxVal;
  minVal = 0.; maxVal = 1.;
  img_sqrt_f.convertTo(img_sqrt, CV_8U, 255.0 / (maxVal - minVal), -minVal);
  img_sqrt.copyTo(img_foreground);

  if (img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if (enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);

  img_background = cv::Mat::zeros(img_input.size(), img_input.type());
  img_background.copyTo(img_bgmodel);

  img_input_prev_1.copyTo(img_input_prev_2);
  img_input.copyTo(img_input_prev_1);

  firstTime = false;
}

cv::Mat WeightedMovingVariance::computeWeightedVariance(const cv::Mat &img_input_f, const cv::Mat &img_mean_f, const double weight)
{
  //ERROR in return (weight * ((cv::abs(img_input_f - img_mean_f))^2.));

  cv::Mat img_f_absdiff(img_input_f.size(), CV_32F);
  cv::absdiff(img_input_f, img_mean_f, img_f_absdiff);
  cv::Mat img_f_pow(img_input_f.size(), CV_32F);
  cv::pow(img_f_absdiff, 2.0, img_f_pow);
  cv::Mat img_f = weight * img_f_pow;

  return img_f;
}

void WeightedMovingVariance::save_config(cv::FileStorage &fs) {
  fs << "enableWeight" << enableWeight;
  fs << "enableThreshold" << enableThreshold;
  fs << "threshold" << threshold;
  fs << "showOutput" << showOutput;
}

void WeightedMovingVariance::load_config(cv::FileStorage &fs) {
  fs["enableWeight"] >> enableWeight;
  fs["enableThreshold"] >> enableThreshold;
  fs["threshold"] >> threshold;
  fs["showOutput"] >> showOutput;
}
