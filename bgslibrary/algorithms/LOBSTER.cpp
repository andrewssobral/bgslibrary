#include "LOBSTER.h"

using namespace bgslibrary::algorithms;

LOBSTER::LOBSTER() :
  IBGS(quote(LOBSTER)),
  pLOBSTER(nullptr),
  fRelLBSPThreshold(lbsp::BGSLOBSTER_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
  nLBSPThresholdOffset(lbsp::BGSLOBSTER_DEFAULT_LBSP_OFFSET_SIMILARITY_THRESHOLD),
  nDescDistThreshold(lbsp::BGSLOBSTER_DEFAULT_DESC_DIST_THRESHOLD),
  nColorDistThreshold(lbsp::BGSLOBSTER_DEFAULT_COLOR_DIST_THRESHOLD),
  nBGSamples(lbsp::BGSLOBSTER_DEFAULT_NB_BG_SAMPLES),
  nRequiredBGSamples(lbsp::BGSLOBSTER_DEFAULT_REQUIRED_NB_BG_SAMPLES)
{
  debug_construction(LOBSTER);
  initLoadSaveConfig(algorithmName);
}

LOBSTER::~LOBSTER() {
  debug_destruction(LOBSTER);
  if (pLOBSTER)
    delete pLOBSTER;
}

void LOBSTER::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
    pLOBSTER = new lbsp::BackgroundSubtractorLOBSTER(
      fRelLBSPThreshold, nLBSPThresholdOffset, nDescDistThreshold,
      nColorDistThreshold, nBGSamples, nRequiredBGSamples);

    pLOBSTER->initialize(img_input, cv::Mat(img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
    firstTime = false;
  }

  pLOBSTER->apply(img_input, img_foreground);
  pLOBSTER->getBackgroundImage(img_background);

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void LOBSTER::save_config(cv::FileStorage &fs) {
  fs << "fRelLBSPThreshold" << fRelLBSPThreshold;
  fs << "nLBSPThresholdOffset" << nLBSPThresholdOffset;
  fs << "nDescDistThreshold" << nDescDistThreshold;
  fs << "nColorDistThreshold" << nColorDistThreshold;
  fs << "nBGSamples" << nBGSamples;
  fs << "nRequiredBGSamples" << nRequiredBGSamples;
  fs << "showOutput" << showOutput;
}

void LOBSTER::load_config(cv::FileStorage &fs) {
  fs["fRelLBSPThreshold"] >> fRelLBSPThreshold;
  fs["nLBSPThresholdOffset"] >> nLBSPThresholdOffset;
  fs["nDescDistThreshold"] >> nDescDistThreshold;
  fs["nColorDistThreshold"] >> nColorDistThreshold;
  fs["nBGSamples"] >> nBGSamples;
  fs["nRequiredBGSamples"] >> nRequiredBGSamples;
  fs["showOutput"] >> showOutput;
}
