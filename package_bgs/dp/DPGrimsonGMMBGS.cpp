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
#include "DPGrimsonGMMBGS.h"

DPGrimsonGMMBGS::DPGrimsonGMMBGS() : firstTime(true), frameNumber(0), threshold(9.0), alpha(0.01), gaussians(3), showOutput(true) 
{
  std::cout << "DPGrimsonGMMBGS()" << std::endl;
}

DPGrimsonGMMBGS::~DPGrimsonGMMBGS()
{
  std::cout << "~DPGrimsonGMMBGS()" << std::endl;
}

void DPGrimsonGMMBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();

  if(firstTime)
    saveConfig();

  frame = new IplImage(img_input);
  
  if(firstTime)
    frame_data.ReleaseMemory(false);
  frame_data = frame;

  if(firstTime)
  {
    int width	= img_input.size().width;
    int height = img_input.size().height;

    lowThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    lowThresholdMask.Ptr()->origin = IPL_ORIGIN_BL;

    highThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    highThresholdMask.Ptr()->origin = IPL_ORIGIN_BL;

    params.SetFrameSize(width, height);
    params.LowThreshold() = threshold; //3.0f*3.0f;
    params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
    //params.Alpha() = 0.001f;
    params.Alpha() = alpha; //0.01f;
    params.MaxModes() = gaussians; //3;

    bgs.Initalize(params);
    bgs.InitModel(frame_data);
  }

  bgs.Subtract(frameNumber, frame_data, lowThresholdMask, highThresholdMask);
  lowThresholdMask.Clear();
  bgs.Update(frameNumber, frame_data, lowThresholdMask);
  
  cv::Mat foreground(highThresholdMask.Ptr());

  if(showOutput)
    cv::imshow("GMM (Grimson)", foreground);
  
  foreground.copyTo(img_output);

  delete frame;
  firstTime = false;
  frameNumber++;
}

void DPGrimsonGMMBGS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/DPGrimsonGMMBGS.xml", 0, CV_STORAGE_WRITE);

  cvWriteReal(fs, "threshold", threshold);
  cvWriteReal(fs, "alpha", alpha);
  cvWriteInt(fs, "gaussians", gaussians);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void DPGrimsonGMMBGS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/DPGrimsonGMMBGS.xml", 0, CV_STORAGE_READ);
  
  threshold = cvReadRealByName(fs, 0, "threshold", 9.0);
  alpha = cvReadRealByName(fs, 0, "alpha", 0.01);
  gaussians = cvReadIntByName(fs, 0, "gaussians", 3);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
