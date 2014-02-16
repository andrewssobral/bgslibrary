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

Csaba, Kertész: Texture-Based Foreground Detection, International Journal of Signal Processing,
Image Processing and Pattern Recognition (IJSIP), Vol. 4, No. 4, 2011.

*/
#include "LbpMrf.h"

#include "MotionDetection.hpp"

LbpMrf::LbpMrf() : firstTime(true), Detector(NULL), showOutput(true)
{
  std::cout << "LbpMrf()" << std::endl;
  Detector = new MotionDetection();
  Detector->SetMode(MotionDetection::md_LBPHistograms);
}

LbpMrf::~LbpMrf()
{
  std::cout << "~LbpMrf()" << std::endl;
  delete Detector;
  Detector = NULL;
}

void LbpMrf::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();

  if(firstTime)
  {
    saveConfig();
  }

  IplImage TempImage(img_input);
  MEImage InputImage(img_input.cols, img_input.rows, img_input.channels());
  MEImage OutputImage(img_input.cols, img_input.rows, img_input.channels());

  InputImage.SetIplImage((void*)&TempImage);

  Detector->DetectMotions(InputImage);
  Detector->GetMotionsMask(OutputImage);
  img_output = (IplImage*)OutputImage.GetIplImage();
  bitwise_not(img_output, img_bgmodel);

  if(showOutput)
  {
    cv::imshow("LBP-MRF FG", img_output);
    cv::imshow("LBP-MRF BG", img_bgmodel);
  }

  firstTime = false;
}

void LbpMrf::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/LbpMrf.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LbpMrf::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/LbpMrf.xml", 0, CV_STORAGE_READ);

  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
