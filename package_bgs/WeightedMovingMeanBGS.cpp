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
#include "WeightedMovingMeanBGS.h"

WeightedMovingMeanBGS::WeightedMovingMeanBGS() : firstTime(true), enableWeight(true), enableThreshold(true), threshold(15), showOutput(true), showBackground(false)
{
  std::cout << "WeightedMovingMeanBGS()" << std::endl;
}

WeightedMovingMeanBGS::~WeightedMovingMeanBGS()
{
  std::cout << "~WeightedMovingMeanBGS()" << std::endl;
}

void WeightedMovingMeanBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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

  cv::Mat img_background_f(img_input.size(), CV_32F);
  
  if(enableWeight)
    img_background_f = ((img_input_f * 0.5) + (img_input_prev_1_f * 0.3) + (img_input_prev_2_f * 0.2));
  else
    img_background_f = ((img_input_f) + (img_input_prev_1_f) + (img_input_prev_2_f)) / 3.0;

  cv::Mat img_background(img_background_f.size(), CV_8U);

  double minVal, maxVal;
  minVal = 0.; maxVal = 1.;
  img_background_f.convertTo(img_background, CV_8U, 255.0/(maxVal - minVal), -minVal);
  
  if(showBackground)
    cv::imshow("W Moving Mean BG Model", img_background);

  cv::Mat img_foreground;
  cv::absdiff(img_input, img_background, img_foreground);

  if(img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if(enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

  if(showOutput)
    cv::imshow("W Moving Mean FG Mask", img_foreground);

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  img_input_prev_1.copyTo(img_input_prev_2);
  img_input.copyTo(img_input_prev_1);

  firstTime = false;
}

void WeightedMovingMeanBGS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/WeightedMovingMeanBGS.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "enableWeight", enableWeight);
  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showOutput", showOutput);
  cvWriteInt(fs, "showBackground", showBackground);

  cvReleaseFileStorage(&fs);
}

void WeightedMovingMeanBGS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/WeightedMovingMeanBGS.xml", 0, CV_STORAGE_READ);
  
  enableWeight = cvReadIntByName(fs, 0, "enableWeight", true);
  enableThreshold = cvReadIntByName(fs, 0, "enableThreshold", true);
  threshold = cvReadIntByName(fs, 0, "threshold", 15);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);
  showBackground = cvReadIntByName(fs, 0, "showBackground", false);

  cvReleaseFileStorage(&fs);
}