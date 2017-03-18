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
#include "VuMeter.h"

using namespace bgslibrary::algorithms;

VuMeter::VuMeter() :
  enableFilter(true), binSize(8), alpha(0.995), threshold(0.03)
{
  std::cout << "VuMeter()" << std::endl;
  setup("./config/VuMeter.xml");
}

VuMeter::~VuMeter()
{
  cvReleaseImage(&mask);
  cvReleaseImage(&background);
  cvReleaseImage(&gray);

  std::cout << "~VuMeter()" << std::endl;
}

void VuMeter::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);
  frame = new IplImage(img_input);

  if (firstTime)
  {
    bgs.SetAlpha(alpha);
    bgs.SetBinSize(binSize);
    bgs.SetThreshold(threshold);

    gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    cvCvtColor(frame, gray, CV_RGB2GRAY);

    background = cvCreateImage(cvGetSize(gray), IPL_DEPTH_8U, 1);
    cvCopy(gray, background);

    mask = cvCreateImage(cvGetSize(gray), IPL_DEPTH_8U, 1);
    cvZero(mask);
  }
  else
    cvCvtColor(frame, gray, CV_RGB2GRAY);

  bgs.UpdateBackground(gray, background, mask);
  img_foreground = cv::cvarrToMat(mask);
  img_background = cv::cvarrToMat(background);

  if (enableFilter)
  {
    cv::erode(img_foreground, img_foreground, cv::Mat());
    cv::medianBlur(img_foreground, img_foreground, 5);
  }

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("VuMeter", img_foreground);
    cv::imshow("VuMeter Bkg Model", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;
  firstTime = false;
}

void VuMeter::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "enableFilter", enableFilter);

  cvWriteInt(fs, "binSize", binSize);
  cvWriteReal(fs, "alpha", alpha);
  cvWriteReal(fs, "threshold", threshold);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void VuMeter::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  enableFilter = cvReadIntByName(fs, nullptr, "enableFilter", true);

  binSize = cvReadIntByName(fs, nullptr, "binSize", 8);
  alpha = cvReadRealByName(fs, nullptr, "alpha", 0.995);
  threshold = cvReadRealByName(fs, nullptr, "threshold", 0.03);

  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
