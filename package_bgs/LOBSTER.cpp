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
#include "LOBSTER.h"

using namespace bgslibrary::algorithms;

LOBSTER::LOBSTER() :
  pLOBSTER(nullptr),
  fRelLBSPThreshold(BGSLOBSTER_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
  nLBSPThresholdOffset(BGSLOBSTER_DEFAULT_LBSP_OFFSET_SIMILARITY_THRESHOLD),
  nDescDistThreshold(BGSLOBSTER_DEFAULT_DESC_DIST_THRESHOLD),
  nColorDistThreshold(BGSLOBSTER_DEFAULT_COLOR_DIST_THRESHOLD),
  nBGSamples(BGSLOBSTER_DEFAULT_NB_BG_SAMPLES),
  nRequiredBGSamples(BGSLOBSTER_DEFAULT_REQUIRED_NB_BG_SAMPLES)
{
  std::cout << "LOBSTER()" << std::endl;
  setup("./config/LOBSTER.xml");
}

LOBSTER::~LOBSTER()
{
  if (pLOBSTER)
    delete pLOBSTER;
  std::cout << "~LOBSTER()" << std::endl;
}

void LOBSTER::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime)
  {
    pLOBSTER = new BackgroundSubtractorLOBSTER(
      fRelLBSPThreshold, nLBSPThresholdOffset, nDescDistThreshold,
      nColorDistThreshold, nBGSamples, nRequiredBGSamples);

    pLOBSTER->initialize(img_input, cv::Mat(img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
    firstTime = false;
  }

  pLOBSTER->apply(img_input, img_foreground);
  pLOBSTER->getBackgroundImage(img_background);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    imshow("LOBSTER FG", img_foreground);
    imshow("LOBSTER BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void LOBSTER::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteReal(fs, "fRelLBSPThreshold", fRelLBSPThreshold);
  cvWriteInt(fs, "nLBSPThresholdOffset", nLBSPThresholdOffset);
  cvWriteInt(fs, "nDescDistThreshold", nDescDistThreshold);
  cvWriteInt(fs, "nColorDistThreshold", nColorDistThreshold);
  cvWriteInt(fs, "nBGSamples", nBGSamples);
  cvWriteInt(fs, "nRequiredBGSamples", nRequiredBGSamples);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LOBSTER::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  fRelLBSPThreshold = cvReadRealByName(fs, nullptr, "fRelLBSPThreshold", BGSLOBSTER_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD);
  nLBSPThresholdOffset = cvReadIntByName(fs, nullptr, "nLBSPThresholdOffset", BGSLOBSTER_DEFAULT_LBSP_OFFSET_SIMILARITY_THRESHOLD);
  nDescDistThreshold = cvReadIntByName(fs, nullptr, "nDescDistThreshold", BGSLOBSTER_DEFAULT_DESC_DIST_THRESHOLD);
  nColorDistThreshold = cvReadIntByName(fs, nullptr, "nColorDistThreshold", BGSLOBSTER_DEFAULT_COLOR_DIST_THRESHOLD);
  nBGSamples = cvReadIntByName(fs, nullptr, "nBGSamples", BGSLOBSTER_DEFAULT_NB_BG_SAMPLES);
  nRequiredBGSamples = cvReadIntByName(fs, nullptr, "nRequiredBGSamples", BGSLOBSTER_DEFAULT_REQUIRED_NB_BG_SAMPLES);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
