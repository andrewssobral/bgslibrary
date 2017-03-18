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
#include "FrameDifference.h"

using namespace bgslibrary::algorithms;

FrameDifference::FrameDifference() :
  enableThreshold(true), threshold(15)
{
  std::cout << "FrameDifference()" << std::endl;
  setup("./config/FrameDifference.xml");
}

FrameDifference::~FrameDifference()
{
  std::cout << "~FrameDifference()" << std::endl;
}

void FrameDifference::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_background.empty())
  {
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
    cv::imshow("Frame Difference", img_foreground);
#endif

  img_foreground.copyTo(img_output);

  img_input.copyTo(img_background);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void FrameDifference::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void FrameDifference::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  enableThreshold = cvReadIntByName(fs, nullptr, "enableThreshold", true);
  threshold = cvReadIntByName(fs, nullptr, "threshold", 15);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
