/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "WeightedMovingVarianceBGS.h"

WeightedMovingVarianceBGS::WeightedMovingVarianceBGS() : firstTime(true), enableWeight(true), 
  enableThreshold(true), threshold(15), showOutput(true)
{
  std::cout << "WeightedMovingVarianceBGS()" << std::endl;
}

WeightedMovingVarianceBGS::~WeightedMovingVarianceBGS()
{
  std::cout << "~WeightedMovingVarianceBGS()" << std::endl;
}

void WeightedMovingVarianceBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();

  if(firstTime)
    saveConfig();

  if(img_input_prev_1.empty())
  {
    img_input.copyTo(img_input_prev_1);
    return;
  }

  if(img_input_prev_2.empty())
  {
    img_input_prev_1.copyTo(img_input_prev_2);
    img_input.copyTo(img_input_prev_1);
    return;
  }

  cv::Mat img_input_f(img_input.size(), CV_32F);
  img_input.convertTo(img_input_f, CV_32F, 1./255.);

  cv::Mat img_input_prev_1_f(img_input.size(), CV_32F);
  img_input_prev_1.convertTo(img_input_prev_1_f, CV_32F, 1./255.);

  cv::Mat img_input_prev_2_f(img_input.size(), CV_32F);
  img_input_prev_2.convertTo(img_input_prev_2_f, CV_32F, 1./255.);

  cv::Mat img_foreground;

  // Weighted mean
  cv::Mat img_mean_f(img_input.size(), CV_32F);
  
  if(enableWeight)
    img_mean_f = ((img_input_f * 0.5) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.2));
  else
    img_mean_f = ((img_input_f * 0.3) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.3));
  
  // Weighted variance
  cv::Mat img_1_f(img_input.size(), CV_32F);
  cv::Mat img_2_f(img_input.size(), CV_32F);
  cv::Mat img_3_f(img_input.size(), CV_32F);
  cv::Mat img_4_f(img_input.size(), CV_32F);

  if(enableWeight)
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
  img_sqrt_f.convertTo(img_sqrt, CV_8U, 255.0/(maxVal - minVal), -minVal);
  img_sqrt.copyTo(img_foreground);

  if(img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if(enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

  if(showOutput)
    cv::imshow("W Moving Variance", img_foreground);

  img_foreground.copyTo(img_output);

  img_input_prev_1.copyTo(img_input_prev_2);
  img_input.copyTo(img_input_prev_1);
  
  firstTime = false;
}

//unused
cv::Mat WeightedMovingVarianceBGS::computeWeightedMean(const std::vector<cv::Mat> &v_img_input_f, const std::vector<double> &weights)
{
  cv::Mat img;
  return img;
}

cv::Mat WeightedMovingVarianceBGS::computeWeightedVariance(const cv::Mat &img_input_f, const cv::Mat &img_mean_f, const double weight)
{
  //ERROR in return (weight * ((cv::abs(img_input_f - img_mean_f))^2.));
  
  cv::Mat img_f_absdiff(img_input_f.size(), CV_32F);
  cv::absdiff(img_input_f, img_mean_f, img_f_absdiff);
  cv::Mat img_f_pow(img_input_f.size(), CV_32F);
  cv::pow(img_f_absdiff, 2.0, img_f_pow);
  cv::Mat img_f = weight * img_f_pow;
  
  return img_f;
}

void WeightedMovingVarianceBGS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/WeightedMovingVarianceBGS.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "enableWeight", enableWeight);
  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showOutput", showOutput);
  
  cvReleaseFileStorage(&fs);
}

void WeightedMovingVarianceBGS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/WeightedMovingVarianceBGS.xml", 0, CV_STORAGE_READ);
  
  enableWeight = cvReadIntByName(fs, 0, "enableWeight", true);
  enableThreshold = cvReadIntByName(fs, 0, "enableThreshold", true);
  threshold = cvReadIntByName(fs, 0, "threshold", 15);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);
  
  cvReleaseFileStorage(&fs);
}
