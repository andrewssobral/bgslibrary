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
#include "GMG.h"

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3

using namespace bgslibrary::algorithms;

GMG::GMG() : initializationFrames(20), decisionThreshold(0.7)
{
  std::cout << "GMG()" << std::endl;
  setup("./config/GMG.xml");

  cv::initModule_video();
  cv::setUseOptimized(true);
  cv::setNumThreads(8);

  fgbg = cv::Algorithm::create<cv::BackgroundSubtractorGMG>("BackgroundSubtractor.GMG");
}

GMG::~GMG()
{
  std::cout << "~GMG()" << std::endl;
}

void GMG::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime)
  {
    fgbg->set("initializationFrames", initializationFrames);
    fgbg->set("decisionThreshold", decisionThreshold);
  }

  if (fgbg.empty())
  {
    std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
    return;
  }

  (*fgbg)(img_input, img_foreground);
  (*fgbg).getBackgroundImage(img_background);

  img_input.copyTo(img_segmentation);
  cv::add(img_input, cv::Scalar(100, 100, 0), img_segmentation, img_foreground);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("GMG FG (Godbehere-Matsukawa-Goldberg)", img_foreground);
    cv::imshow("GMG BG (Godbehere-Matsukawa-Goldberg)", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void GMG::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "initializationFrames", initializationFrames);
  cvWriteReal(fs, "decisionThreshold", decisionThreshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void GMG::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  initializationFrames = cvReadIntByName(fs, nullptr, "initializationFrames", 20);
  decisionThreshold = cvReadRealByName(fs, nullptr, "decisionThreshold", 0.7);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}

#endif
