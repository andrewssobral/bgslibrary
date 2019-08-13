#include "GMG.h"

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3

using namespace bgslibrary::algorithms;

GMG::GMG() : 
  IBGS(quote(GMG)),
  initializationFrames(20), decisionThreshold(0.7)
{
  debug_construction(GMG);
  initLoadSaveConfig(algorithmName);

  cv::initModule_video();
  cv::setUseOptimized(true);
  cv::setNumThreads(4);

  fgbg = cv::Algorithm::create<cv::BackgroundSubtractorGMG>("BackgroundSubtractor.GMG");
}

GMG::~GMG() {
  debug_destruction(GMG);
}

void GMG::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
    fgbg->set("initializationFrames", initializationFrames);
    fgbg->set("decisionThreshold", decisionThreshold);
  }

  if (fgbg.empty()) {
    std::cerr << "Failed to create " + algorithmName << std::endl;
    return;
  }

  (*fgbg)(img_input, img_foreground);
  (*fgbg).getBackgroundImage(img_background);

  img_input.copyTo(img_segmentation);
  cv::add(img_input, cv::Scalar(100, 100, 0), img_segmentation, img_foreground);

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void GMG::save_config(cv::FileStorage &fs) {
  fs << "initializationFrames" << initializationFrames;
  fs << "decisionThreshold" << decisionThreshold;
  fs << "showOutput" << showOutput;
}

void GMG::load_config(cv::FileStorage &fs) {
  fs["initializationFrames"] >> initializationFrames;
  fs["decisionThreshold"] >> decisionThreshold;
  fs["showOutput"] >> showOutput;
}

#endif
