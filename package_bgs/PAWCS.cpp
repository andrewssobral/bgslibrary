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
#include "PAWCS.h"

using namespace bgslibrary::algorithms;

PAWCS::PAWCS() : pPAWCS(nullptr),
fRelLBSPThreshold(BGSPAWCS_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
nDescDistThresholdOffset(BGSPAWCS_DEFAULT_DESC_DIST_THRESHOLD_OFFSET),
nMinColorDistThreshold(BGSPAWCS_DEFAULT_MIN_COLOR_DIST_THRESHOLD),
nMaxNbWords(BGSPAWCS_DEFAULT_MAX_NB_WORDS),
nSamplesForMovingAvgs(BGSPAWCS_DEFAULT_N_SAMPLES_FOR_MV_AVGS)
{
  std::cout << "PAWCS()" << std::endl;
  setup("./config/PAWCS.xml");
}
PAWCS::~PAWCS()
{
  if (pPAWCS)
    delete pPAWCS;
  std::cout << "~PAWCS()" << std::endl;
}

void PAWCS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime)
  {
    pPAWCS = new BackgroundSubtractorPAWCS(
      fRelLBSPThreshold, nDescDistThresholdOffset, nMinColorDistThreshold,
      nMaxNbWords, nSamplesForMovingAvgs);

    pPAWCS->initialize(img_input, cv::Mat(img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
    firstTime = false;
  }

  pPAWCS->apply(img_input, img_foreground);
  pPAWCS->getBackgroundImage(img_background);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    imshow("PAWCS FG", img_foreground);
    imshow("PAWCS BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void PAWCS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteReal(fs, "fRelLBSPThreshold", fRelLBSPThreshold);
  cvWriteInt(fs, "nDescDistThresholdOffset", nDescDistThresholdOffset);
  cvWriteInt(fs, "nMinColorDistThreshold", nMinColorDistThreshold);
  cvWriteInt(fs, "nMaxNbWords", nMaxNbWords);
  cvWriteInt(fs, "nSamplesForMovingAvgs", nSamplesForMovingAvgs);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void PAWCS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  fRelLBSPThreshold = cvReadRealByName(fs, nullptr, "fRelLBSPThreshold", BGSPAWCS_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD);
  nDescDistThresholdOffset = cvReadIntByName(fs, nullptr, "nDescDistThresholdOffset", BGSPAWCS_DEFAULT_DESC_DIST_THRESHOLD_OFFSET);
  nMinColorDistThreshold = cvReadIntByName(fs, nullptr, "nMinColorDistThreshold", BGSPAWCS_DEFAULT_MIN_COLOR_DIST_THRESHOLD);
  nMaxNbWords = cvReadIntByName(fs, nullptr, "nMaxNbWords", BGSPAWCS_DEFAULT_MAX_NB_WORDS);
  nSamplesForMovingAvgs = cvReadIntByName(fs, nullptr, "nSamplesForMovingAvgs", BGSPAWCS_DEFAULT_N_SAMPLES_FOR_MV_AVGS);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
