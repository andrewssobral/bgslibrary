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
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  fs << "fRelLBSPThreshold" << fRelLBSPThreshold;
  fs << "nDescDistThresholdOffset" << nDescDistThresholdOffset;
  fs << "nMinColorDistThreshold" << nMinColorDistThreshold;
  fs << "nMaxNbWords" << nMaxNbWords;
  fs << "nSamplesForMovingAvgs" << nSamplesForMovingAvgs;
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void PAWCS::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  fs["fRelLBSPThreshold"] >> fRelLBSPThreshold;
  fs["nDescDistThresholdOffset"] >> nDescDistThresholdOffset;
  fs["nMinColorDistThreshold"] >> nMinColorDistThreshold;
  fs["nMaxNbWords"] >> nMaxNbWords;
  fs["nSamplesForMovingAvgs"] >> nSamplesForMovingAvgs;
  fs["showOutput"] >> showOutput;
  
  fs.release();
}
