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

VuMeter::VuMeter() : firstTime(true), showOutput(true), enableFilter(true), binSize(8), alpha(0.995), threshold(0.03)
{
  std::cout << "VuMeter()" << std::endl;
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
  if(img_input.empty())
    return;
  else
    frame = new IplImage(img_input);

  loadConfig();

  if(firstTime)
  {
    bgs.SetAlpha(alpha);
    bgs.SetBinSize(binSize);
    bgs.SetThreshold(threshold);

    gray = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
    cvCvtColor(frame,gray,CV_RGB2GRAY);

    background = cvCreateImage(cvGetSize(gray),IPL_DEPTH_8U,1);
    cvCopy(gray, background);

    mask = cvCreateImage(cvGetSize(gray),IPL_DEPTH_8U,1);
    cvZero(mask);

    saveConfig();
  }
  else
    cvCvtColor(frame,gray,CV_RGB2GRAY);
  
  bgs.UpdateBackground(gray,background,mask);
  cv::Mat img_foreground(mask);
  cv::Mat img_bkg(background);

  if(enableFilter)
  {
    cv::erode(img_foreground,img_foreground,cv::Mat());
    cv::medianBlur(img_foreground, img_foreground, 5);
  }

  if(showOutput)
  {
    if(!img_foreground.empty())
      cv::imshow("VuMeter", img_foreground);
    
    if(!img_bkg.empty())
      cv::imshow("VuMeter Bkg Model", img_bkg);
  }

  img_foreground.copyTo(img_output);
  img_bkg.copyTo(img_bgmodel);
  
  delete frame;
  firstTime = false;
}

void VuMeter::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/VuMeter.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "enableFilter", enableFilter);
  
  cvWriteInt(fs, "binSize", binSize);
  cvWriteReal(fs, "alpha", alpha);
  cvWriteReal(fs, "threshold", threshold);
  
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void VuMeter::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/VuMeter.xml", 0, CV_STORAGE_READ);
  
  enableFilter = cvReadIntByName(fs, 0, "enableFilter", true);
  
  binSize = cvReadIntByName(fs, 0, "binSize", 8);
  alpha = cvReadRealByName(fs, 0, "alpha", 0.995);
  threshold = cvReadRealByName(fs, 0, "threshold", 0.03);
  
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
