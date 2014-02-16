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

GMG::GMG() : firstTime(true), initializationFrames(20), decisionThreshold(0.7), showOutput(true)
{
  std::cout << "GMG()" << std::endl;

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
  if(img_input.empty())
    return;

  loadConfig();

  if(firstTime)
  {
    fgbg->set("initializationFrames", initializationFrames);
    fgbg->set("decisionThreshold", decisionThreshold);

    saveConfig();
  }
  
  if(fgbg.empty())
  {
    std::cerr << "Failed to create BackgroundSubtractor.GMG Algorithm." << std::endl;
    return;
  }

  (*fgbg)(img_input, img_foreground);

  cv::Mat img_background;
  (*fgbg).getBackgroundImage(img_background);

  img_input.copyTo(img_segmentation);
  cv::add(img_input, cv::Scalar(100, 100, 0), img_segmentation, img_foreground);

  if(showOutput)
  {
    if (!img_foreground.empty())
      cv::imshow("GMG FG (Godbehere-Matsukawa-Goldberg)", img_foreground);
    
    if (!img_background.empty())
      cv::imshow("GMG BG (Godbehere-Matsukawa-Goldberg)", img_background);
  }

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void GMG::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/GMG.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "initializationFrames", initializationFrames);
  cvWriteReal(fs, "decisionThreshold", decisionThreshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void GMG::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/GMG.xml", 0, CV_STORAGE_READ);
  
  initializationFrames = cvReadIntByName(fs, 0, "initializationFrames", 20);
  decisionThreshold = cvReadRealByName(fs, 0, "decisionThreshold", 0.7);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);
  
  cvReleaseFileStorage(&fs);
}
