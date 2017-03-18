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
#include "WeightedMovingMean.h"

using namespace bgslibrary::algorithms;

WeightedMovingMean::WeightedMovingMean() :
  enableWeight(true), enableThreshold(true), threshold(15)
{
  std::cout << "WeightedMovingMean()" << std::endl;
  setup("./config/WeightedMovingMean.xml");
}

WeightedMovingMean::~WeightedMovingMean()
{
  std::cout << "~WeightedMovingMean()" << std::endl;
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
  if (showOutput)
  {
    cv::imshow("W Moving Mean FG Mask", img_foreground);
    cv::imshow("W Moving Mean BG Model", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  img_input_prev_1.copyTo(img_input_prev_2);
  img_input.copyTo(img_input_prev_1);

  firstTime = false;
}

void WeightedMovingMean::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "enableWeight", enableWeight);
  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void WeightedMovingMean::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  enableWeight = cvReadIntByName(fs, nullptr, "enableWeight", true);
  enableThreshold = cvReadIntByName(fs, nullptr, "enableThreshold", true);
  threshold = cvReadIntByName(fs, nullptr, "threshold", 15);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
