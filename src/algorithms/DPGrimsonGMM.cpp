#include "DPGrimsonGMM.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

DPGrimsonGMM::DPGrimsonGMM() :
  IBGS(quote(DPGrimsonGMM)),
  frameNumber(0), threshold(9.0), 
  alpha(0.01), gaussians(3)
{
  debug_construction(DPGrimsonGMM);
  initLoadSaveConfig(algorithmName);
}

DPGrimsonGMM::~DPGrimsonGMM() {
  debug_destruction(DPGrimsonGMM);
}

void DPGrimsonGMM::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage _frame = cvIplImage(img_input);
  frame_data = cvCloneImage(&_frame);

  if (firstTime) {
    int width = img_input.size().width;
    int height = img_input.size().height;

    lowThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    lowThresholdMask.Ptr()->origin = IPL_ORIGIN_BL;

    highThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    highThresholdMask.Ptr()->origin = IPL_ORIGIN_BL;

    params.SetFrameSize(width, height);
    params.LowThreshold() = threshold; //3.0f*3.0f;
    params.HighThreshold() = 2 * params.LowThreshold();	// Note: high threshold is used by post-processing
    //params.Alpha() = 0.001f;
    params.Alpha() = alpha; //0.01f;
    params.MaxModes() = gaussians; //3;

    bgs.Initalize(params);
    bgs.InitModel(frame_data);
  }

  bgs.Subtract(frameNumber, frame_data, lowThresholdMask, highThresholdMask);
  lowThresholdMask.Clear();
  bgs.Update(frameNumber, frame_data, lowThresholdMask);

  img_foreground = cv::cvarrToMat(highThresholdMask.Ptr());
  img_background = cv::cvarrToMat(bgs.Background()->Ptr());
  //img_background = cv::Mat::zeros(img_input.size(), img_input.type());

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
  frame_data.ReleaseImage();

  firstTime = false;
  frameNumber++;
}

void DPGrimsonGMM::save_config(cv::FileStorage &fs) {
  fs << "threshold" << threshold;
  fs << "alpha" << alpha;
  fs << "gaussians" << gaussians;
  fs << "showOutput" << showOutput;
}

void DPGrimsonGMM::load_config(cv::FileStorage &fs) {
  fs["threshold"] >> threshold;
  fs["alpha"] >> alpha;
  fs["gaussians"] >> gaussians;
  fs["showOutput"] >> showOutput;
}

#endif
