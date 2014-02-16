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
#include "StaticFrameDifferenceBGS.h"

StaticFrameDifferenceBGS::StaticFrameDifferenceBGS() : firstTime(true), enableThreshold(true), threshold(15), showOutput(true)
{
  std::cout << "StaticFrameDifferenceBGS()" << std::endl;
}

StaticFrameDifferenceBGS::~StaticFrameDifferenceBGS()
{
  std::cout << "~StaticFrameDifferenceBGS()" << std::endl;
}

void StaticFrameDifferenceBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  if(img_background.empty())
    img_input.copyTo(img_background);
  
  loadConfig();

  if(firstTime)
    saveConfig();

  cv::absdiff(img_input, img_background, img_foreground);

  if(img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  if(enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

  if(showOutput)
    cv::imshow("Static Frame Difference", img_foreground);

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void StaticFrameDifferenceBGS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/StaticFrameDifferenceBGS.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void StaticFrameDifferenceBGS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/StaticFrameDifferenceBGS.xml", 0, CV_STORAGE_READ);
  
  enableThreshold = cvReadIntByName(fs, 0, "enableThreshold", true);
  threshold = cvReadIntByName(fs, 0, "threshold", 15);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}