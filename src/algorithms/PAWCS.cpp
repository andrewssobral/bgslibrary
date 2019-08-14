#include "PAWCS.h"

using namespace bgslibrary::algorithms;

PAWCS::PAWCS() : 
  IBGS(quote(PAWCS)),
  pPAWCS(nullptr),
  fRelLBSPThreshold(lbsp::BGSPAWCS_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
  nDescDistThresholdOffset(lbsp::BGSPAWCS_DEFAULT_DESC_DIST_THRESHOLD_OFFSET),
  nMinColorDistThreshold(lbsp::BGSPAWCS_DEFAULT_MIN_COLOR_DIST_THRESHOLD),
  nMaxNbWords(lbsp::BGSPAWCS_DEFAULT_MAX_NB_WORDS),
  nSamplesForMovingAvgs(lbsp::BGSPAWCS_DEFAULT_N_SAMPLES_FOR_MV_AVGS)
{
  debug_construction(PAWCS);
  initLoadSaveConfig(algorithmName);
}

PAWCS::~PAWCS() {
  debug_destruction(PAWCS);
  if (pPAWCS)
    delete pPAWCS;
}

void PAWCS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
    pPAWCS = new lbsp::BackgroundSubtractorPAWCS(
      fRelLBSPThreshold, nDescDistThresholdOffset, nMinColorDistThreshold,
      nMaxNbWords, nSamplesForMovingAvgs);

    pPAWCS->initialize(img_input, cv::Mat(img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
    firstTime = false;
  }

  pPAWCS->apply(img_input, img_foreground);
  pPAWCS->getBackgroundImage(img_background);

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void PAWCS::save_config(cv::FileStorage &fs) {
  fs << "fRelLBSPThreshold" << fRelLBSPThreshold;
  fs << "nDescDistThresholdOffset" << nDescDistThresholdOffset;
  fs << "nMinColorDistThreshold" << nMinColorDistThreshold;
  fs << "nMaxNbWords" << nMaxNbWords;
  fs << "nSamplesForMovingAvgs" << nSamplesForMovingAvgs;
  fs << "showOutput" << showOutput;
}

void PAWCS::load_config(cv::FileStorage &fs) {
  fs["fRelLBSPThreshold"] >> fRelLBSPThreshold;
  fs["nDescDistThresholdOffset"] >> nDescDistThresholdOffset;
  fs["nMinColorDistThreshold"] >> nMinColorDistThreshold;
  fs["nMaxNbWords"] >> nMaxNbWords;
  fs["nSamplesForMovingAvgs"] >> nSamplesForMovingAvgs;
  fs["showOutput"] >> showOutput;
}
