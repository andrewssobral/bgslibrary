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
#include "AdaptiveBackgroundLearning.h"

AdaptiveBackgroundLearning::AdaptiveBackgroundLearning() : firstTime(true), alpha(0.05), limit(-1), counter(0), minVal(0.0), maxVal(1.0), 
  enableThreshold(true), threshold(15), showForeground(true), showBackground(true)
{
  std::cout << "AdaptiveBackgroundLearning()" << std::endl;
}

AdaptiveBackgroundLearning::~AdaptiveBackgroundLearning()
{
  std::cout << "~AdaptiveBackgroundLearning()" << std::endl;
}

void AdaptiveBackgroundLearning::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();

  if(firstTime)
    saveConfig();

  if(img_background.empty())
    img_input.copyTo(img_background);

  cv::Mat img_input_f(img_input.size(), CV_32F);
  img_input.convertTo(img_input_f, CV_32F, 1./255.);

  cv::Mat img_background_f(img_background.size(), CV_32F);
  img_background.convertTo(img_background_f, CV_32F, 1./255.);

  cv::Mat img_diff_f(img_input.size(), CV_32F);
  cv::absdiff(img_input_f, img_background_f, img_diff_f);

  if((limit > 0 && limit < counter) || limit == -1)
  {
    img_background_f = alpha*img_input_f + (1-alpha)*img_background_f;
    
    cv::Mat img_new_background(img_input.size(), CV_8U);
    img_background_f.convertTo(img_new_background, CV_8U, 255.0/(maxVal - minVal), -minVal);
    img_new_background.copyTo(img_background);

    if(limit > 0 && limit < counter)
      counter++;
  }
  
  cv::Mat img_foreground(img_input.size(), CV_8U);
  img_diff_f.convertTo(img_foreground, CV_8U, 255.0/(maxVal - minVal), -minVal);

  if(img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if(enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);
  
  if(showForeground)
    cv::imshow("A-Learning FG", img_foreground);

  if(showBackground)
    cv::imshow("A-Learning BG", img_background);

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void AdaptiveBackgroundLearning::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/AdaptiveBackgroundLearning.xml", 0, CV_STORAGE_WRITE);

  cvWriteReal(fs, "alpha", alpha);
  cvWriteInt(fs, "limit", limit);
  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showForeground", showForeground);
  cvWriteInt(fs, "showBackground", showBackground);

  cvReleaseFileStorage(&fs);
}

void AdaptiveBackgroundLearning::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/AdaptiveBackgroundLearning.xml", 0, CV_STORAGE_READ);
  
  alpha = cvReadRealByName(fs, 0, "alpha", 0.05);
  limit = cvReadIntByName(fs, 0, "limit", -1);
  enableThreshold = cvReadIntByName(fs, 0, "enableThreshold", true);
  threshold = cvReadIntByName(fs, 0, "threshold", 15);
  showForeground = cvReadIntByName(fs, 0, "showForeground", true);
  showBackground = cvReadIntByName(fs, 0, "showBackground", true);

  cvReleaseFileStorage(&fs);
}
