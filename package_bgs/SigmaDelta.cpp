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
#include "SigmaDelta.h"

using namespace bgslibrary::algorithms;

SigmaDelta::SigmaDelta() :
  ampFactor(1), minVar(15), maxVar(255), algorithm(sdLaMa091New())
{
  applyParams();
  std::cout << "SigmaDelta()" << std::endl;
  setup("./config/SigmaDelta.xml");
}

SigmaDelta::~SigmaDelta()
{
  sdLaMa091Free(algorithm);
  std::cout << "~SigmaDelta()" << std::endl;
}

void SigmaDelta::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime)
  {
    sdLaMa091AllocInit_8u_C3R(algorithm, img_input.data, img_input.cols, img_input.rows, img_input.step);
    img_foreground = cv::Mat(img_input.size(), CV_8UC1);
    img_background = cv::Mat(img_input.size(), CV_8UC3);
    firstTime = false;
  }
  else
  {
    cv::Mat img_output_tmp(img_input.rows, img_input.cols, CV_8UC3);
    sdLaMa091Update_8u_C3R(algorithm, img_input.data, img_output_tmp.data);

    unsigned char* tmpBuffer = (unsigned char*)img_output_tmp.data;
    unsigned char* outBuffer = (unsigned char*)img_foreground.data;

    for (size_t i = 0; i < img_foreground.total(); ++i) {
      *outBuffer = *tmpBuffer;
      ++outBuffer;
      tmpBuffer += img_output_tmp.channels();
    }
  }

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow("Sigma-Delta", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void SigmaDelta::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "ampFactor", ampFactor);
  cvWriteInt(fs, "minVar", minVar);
  cvWriteInt(fs, "maxVar", maxVar);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void SigmaDelta::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  ampFactor = cvReadIntByName(fs, nullptr, "ampFactor", 1);
  minVar = cvReadIntByName(fs, nullptr, "minVar", 15);
  maxVar = cvReadIntByName(fs, nullptr, "maxVar", 255);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  applyParams();

  cvReleaseFileStorage(&fs);
}

void SigmaDelta::applyParams()
{
  sdLaMa091SetAmplificationFactor(algorithm, ampFactor);
  sdLaMa091SetMinimalVariance(algorithm, minVar);
  sdLaMa091SetMaximalVariance(algorithm, maxVar);
}
