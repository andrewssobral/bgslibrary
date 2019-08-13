#include "LBP_MRF.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

LBP_MRF::LBP_MRF() :
  IBGS(quote(LBP_MRF)),
  Detector(nullptr)
{
  debug_construction(LBP_MRF);
  initLoadSaveConfig(algorithmName);
  Detector = new MotionDetection();
  Detector->SetMode(MotionDetection::md_LBPHistograms);
}

LBP_MRF::~LBP_MRF() {
  debug_destruction(LBP_MRF);
  delete Detector;
  Detector = nullptr;
}

void LBP_MRF::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage TempImage(img_input);
  MEImage InputImage(img_input.cols, img_input.rows, img_input.channels());
  MEImage OutputImage(img_input.cols, img_input.rows, img_input.channels());

  InputImage.SetIplImage((void*)&TempImage);

  Detector->DetectMotions(InputImage);
  Detector->GetMotionsMask(OutputImage);
  img_foreground = cv::cvarrToMat((IplImage*)OutputImage.GetIplImage());
  //bitwise_not(img_foreground, img_background);
  img_background = cv::Mat::zeros(img_input.size(), img_input.type());

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void LBP_MRF::save_config(cv::FileStorage &fs) {
  fs << "showOutput" << showOutput;
}

void LBP_MRF::load_config(cv::FileStorage &fs) {
  fs["showOutput"] >> showOutput;
}

#endif
