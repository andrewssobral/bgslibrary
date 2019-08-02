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
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  fs << "initializationFrames" << initializationFrames;
  fs << "decisionThreshold" << decisionThreshold;
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void GMG::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  fs["initializationFrames"] >> initializationFrames;
  fs["decisionThreshold"] >> decisionThreshold;
  fs["showOutput"] >> showOutput;
  
  fs.release();
}

#endif
