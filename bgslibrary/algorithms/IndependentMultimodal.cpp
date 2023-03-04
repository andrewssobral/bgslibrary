#include "IndependentMultimodal.h"

using namespace bgslibrary::algorithms;

IndependentMultimodal::IndependentMultimodal() : 
  IBGS(quote(IndependentMultimodal)), fps(10)
{
  debug_construction(IndependentMultimodal);
  initLoadSaveConfig(algorithmName);
  pIMBS = new imbs::BackgroundSubtractorIMBS(fps);
}

IndependentMultimodal::~IndependentMultimodal() {
  debug_destruction(IndependentMultimodal);
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
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  firstTime = false;
}

void IndependentMultimodal::save_config(cv::FileStorage &fs) {
  fs << "fps" << fps;
  fs << "showOutput" << showOutput;
}

void IndependentMultimodal::load_config(cv::FileStorage &fs) {
  fs["fps"] >> fps;
  fs["showOutput"] >> showOutput;
}
