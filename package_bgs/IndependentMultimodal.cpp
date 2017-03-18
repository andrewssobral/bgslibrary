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
#include "IndependentMultimodal.h"

using namespace bgslibrary::algorithms;

IndependentMultimodal::IndependentMultimodal() : fps(10)
{
  std::cout << "IndependentMultimodal()" << std::endl;
  pIMBS = new BackgroundSubtractorIMBS(fps);
  setup("./config/IndependentMultimodal.xml");
}

IndependentMultimodal::~IndependentMultimodal()
{
  std::cout << "~IndependentMultimodal()" << std::endl;
  delete pIMBS;
}

void IndependentMultimodal::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  //get the fgmask and update the background model
  pIMBS->apply(img_input, img_foreground);

  //get background image
  pIMBS->getBackgroundImage(img_background);

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("IMBS FG", img_foreground);
    cv::imshow("IMBS BG", img_background);
  }
#endif

  firstTime = false;
}

void IndependentMultimodal::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void IndependentMultimodal::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
