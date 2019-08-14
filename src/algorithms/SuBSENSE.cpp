#include "SuBSENSE.h"

using namespace bgslibrary::algorithms;

SuBSENSE::SuBSENSE() :
  IBGS(quote(SuBSENSE)),
  pSubsense(0),
  fRelLBSPThreshold(lbsp::BGSSUBSENSE_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
  nDescDistThresholdOffset(lbsp::BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET),
  nMinColorDistThreshold(lbsp::BGSSUBSENSE_DEFAULT_MIN_COLOR_DIST_THRESHOLD),
  nBGSamples(lbsp::BGSSUBSENSE_DEFAULT_NB_BG_SAMPLES),
  nRequiredBGSamples(lbsp::BGSSUBSENSE_DEFAULT_REQUIRED_NB_BG_SAMPLES),
  nSamplesForMovingAvgs(lbsp::BGSSUBSENSE_DEFAULT_N_SAMPLES_FOR_MV_AVGS)
{
  debug_construction(SuBSENSE);
  initLoadSaveConfig(algorithmName);
}

SuBSENSE::~SuBSENSE() {
  debug_destruction(SuBSENSE);
  if (pSubsense)
    delete pSubsense;
}

void SuBSENSE::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
    pSubsense = new lbsp::BackgroundSubtractorSuBSENSE(
      fRelLBSPThreshold, nDescDistThresholdOffset, nMinColorDistThreshold,
      nBGSamples, nRequiredBGSamples, nSamplesForMovingAvgs);

    pSubsense->initialize(img_input, cv::Mat(img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
    firstTime = false;
  }

  pSubsense->apply(img_input, img_foreground);
  pSubsense->getBackgroundImage(img_background);

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void SuBSENSE::save_config(cv::FileStorage &fs) {
  fs << "fRelLBSPThreshold" << fRelLBSPThreshold;
  fs << "nDescDistThresholdOffset" << nDescDistThresholdOffset;
  fs << "nMinColorDistThreshold" << nMinColorDistThreshold;
  fs << "nBGSamples" << nBGSamples;
  fs << "nRequiredBGSamples" << nRequiredBGSamples;
  fs << "nSamplesForMovingAvgs" << nSamplesForMovingAvgs;
  fs << "showOutput" << showOutput;
}

void SuBSENSE::load_config(cv::FileStorage &fs) {
  fs["fRelLBSPThreshold"] >> fRelLBSPThreshold;
  fs["nDescDistThresholdOffset"] >> nDescDistThresholdOffset;
  fs["nMinColorDistThreshold"] >> nMinColorDistThreshold;
  fs["nBGSamples"] >> nBGSamples;
  fs["nRequiredBGSamples"] >> nRequiredBGSamples;
  fs["nSamplesForMovingAvgs"] >> nSamplesForMovingAvgs;
  fs["showOutput"] >> showOutput;
}
