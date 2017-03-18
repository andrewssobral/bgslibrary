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
#include "SuBSENSE.h"

using namespace bgslibrary::algorithms;

SuBSENSE::SuBSENSE() :
  pSubsense(0),
  fRelLBSPThreshold(BGSSUBSENSE_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
  nDescDistThresholdOffset(BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET),
  nMinColorDistThreshold(BGSSUBSENSE_DEFAULT_MIN_COLOR_DIST_THRESHOLD),
  nBGSamples(BGSSUBSENSE_DEFAULT_NB_BG_SAMPLES),
  nRequiredBGSamples(BGSSUBSENSE_DEFAULT_REQUIRED_NB_BG_SAMPLES),
  nSamplesForMovingAvgs(BGSSUBSENSE_DEFAULT_N_SAMPLES_FOR_MV_AVGS)
{
  std::cout << "SuBSENSE()" << std::endl;
}

SuBSENSE::~SuBSENSE() {
  if (pSubsense)
    delete pSubsense;
  std::cout << "~SuBSENSE()" << std::endl;
}

void SuBSENSE::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime)
  {
    pSubsense = new BackgroundSubtractorSuBSENSE(
      fRelLBSPThreshold, nDescDistThresholdOffset, nMinColorDistThreshold,
      nBGSamples, nRequiredBGSamples, nSamplesForMovingAvgs);

    pSubsense->initialize(img_input, cv::Mat(img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
    firstTime = false;
  }

  pSubsense->apply(img_input, img_foreground);
  pSubsense->getBackgroundImage(img_background);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    imshow("SuBSENSE FG", img_foreground);
    imshow("SuBSENSE BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void SuBSENSE::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteReal(fs, "fRelLBSPThreshold", fRelLBSPThreshold);
  cvWriteInt(fs, "nDescDistThresholdOffset", nDescDistThresholdOffset);
  cvWriteInt(fs, "nMinColorDistThreshold", nMinColorDistThreshold);
  cvWriteInt(fs, "nBGSamples", nBGSamples);
  cvWriteInt(fs, "nRequiredBGSamples", nRequiredBGSamples);
  cvWriteInt(fs, "nSamplesForMovingAvgs", nSamplesForMovingAvgs);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void SuBSENSE::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  fRelLBSPThreshold = cvReadRealByName(fs, nullptr, "fRelLBSPThreshold", BGSSUBSENSE_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD);
  nDescDistThresholdOffset = cvReadIntByName(fs, nullptr, "nDescDistThresholdOffset", BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET);
  nMinColorDistThreshold = cvReadIntByName(fs, nullptr, "nMinColorDistThreshold", BGSSUBSENSE_DEFAULT_MIN_COLOR_DIST_THRESHOLD);
  nBGSamples = cvReadIntByName(fs, nullptr, "nBGSamples", BGSSUBSENSE_DEFAULT_NB_BG_SAMPLES);
  nRequiredBGSamples = cvReadIntByName(fs, nullptr, "nRequiredBGSamples", BGSSUBSENSE_DEFAULT_REQUIRED_NB_BG_SAMPLES);
  nSamplesForMovingAvgs = cvReadIntByName(fs, nullptr, "nSamplesForMovingAvgs", BGSSUBSENSE_DEFAULT_N_SAMPLES_FOR_MV_AVGS);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
