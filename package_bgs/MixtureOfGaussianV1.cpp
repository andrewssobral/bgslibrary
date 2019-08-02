#include "MixtureOfGaussianV1.h"

#if CV_MAJOR_VERSION == 2

using namespace bgslibrary::algorithms;

MixtureOfGaussianV1::MixtureOfGaussianV1() :
  alpha(0.05), enableThreshold(true), threshold(15)
{
  std::cout << "MixtureOfGaussianV1()" << std::endl;
  setup("./config/MixtureOfGaussianV1.xml");
}

MixtureOfGaussianV1::~MixtureOfGaussianV1()
{
  std::cout << "~MixtureOfGaussianV1()" << std::endl;
}

void MixtureOfGaussianV1::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  //------------------------------------------------------------------
  // BackgroundSubtractorMOG
  // http://opencv.itseez.com/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractormog
  //
  // Gaussian Mixture-based Backbround/Foreground Segmentation Algorithm.
  //
  // The class implements the algorithm described in:
  //   P. KadewTraKuPong and R. Bowden,
  //   An improved adaptive background mixture model for real-time tracking with shadow detection,
  //   Proc. 2nd European Workshp on Advanced Video-Based Surveillance Systems, 2001
  //------------------------------------------------------------------

  mog(img_input, img_foreground, alpha);
  mog.getBackgroundImage(img_background);

  if (enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

  if (img_foreground.empty())
    img_foreground = cv::Mat::zeros(img_input.size(), img_input.type());

  if (img_background.empty())
    img_background = cv::Mat::zeros(img_input.size(), img_input.type());

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("GMM FG (KadewTraKuPong&Bowden)", img_foreground);
    cv::imshow("GMM BG (KadewTraKuPong&Bowden)", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void MixtureOfGaussianV1::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  fs << "alpha" << alpha;
  fs << "enableThreshold" << enableThreshold;
  fs << "threshold" << threshold;
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void MixtureOfGaussianV1::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  fs["alpha"] >> alpha;
  fs["enableThreshold"] >> enableThreshold;
  fs["threshold"] >> threshold;
  fs["showOutput"] >> showOutput;
  
  fs.release();
}

#endif
