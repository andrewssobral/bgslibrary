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
#include "LBP_MRF.h"

using namespace bgslibrary::algorithms;

LBP_MRF::LBP_MRF() :
  Detector(nullptr)
{
  std::cout << "LBP_MRF()" << std::endl;
  setup("./config/LBP_MRF.xml");
  Detector = new MotionDetection();
  Detector->SetMode(MotionDetection::md_LBPHistograms);
}

LBP_MRF::~LBP_MRF()
{
  std::cout << "~LBP_MRF()" << std::endl;
  delete Detector;
  Detector = nullptr;
}

void LBP_MRF::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage TempImage(img_input);
  MEImage InputImage(img_input.cols, img_input.rows, img_input.channels());
  MEImage OutputImage(img_input.cols, img_input.rows, img_input.channels());

  InputImage.SetIplImage((void*)&TempImage);

  Detector->DetectMotions(InputImage);
  Detector->GetMotionsMask(OutputImage);
  img_foreground = cv::cvarrToMat((IplImage*)OutputImage.GetIplImage());
  //bitwise_not(img_foreground, img_background);
  img_background = cv::Mat::zeros(img_input.size(), img_input.type());

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow("LBP-MRF FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void LBP_MRF::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LBP_MRF::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
