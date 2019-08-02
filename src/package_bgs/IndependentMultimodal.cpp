#include "IndependentMultimodal.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

IndependentMultimodal::IndependentMultimodal() : fps(10)
{
  std::cout << "IndependentMultimodal()" << std::endl;
  pIMBS = new BackgroundSubtractorIMBS(fps);
  setup("./config/IndependentMultimodal.xml");
}

IndependentMultimodal::~IndependentMultimodal()
{
  std::cout << "~IndependentMultimodal()" << std::endl;
  delete pIMBS;
}

void IndependentMultimodal::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  //get the fgmask and update the background model
  pIMBS->apply(img_input, img_foreground);

  //get background image
  pIMBS->getBackgroundImage(img_background);

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("IMBS FG", img_foreground);
    cv::imshow("IMBS BG", img_background);
  }
#endif

  firstTime = false;
}

void IndependentMultimodal::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void IndependentMultimodal::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  fs["showOutput"] >> showOutput;
  
  fs.release();
}

#endif
