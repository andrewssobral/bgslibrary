#include "VuMeter.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

VuMeter::VuMeter() :
  IBGS(quote(VuMeter)),
  enableFilter(true), binSize(8), 
  alpha(0.995), threshold(0.03)
{
  debug_construction(VuMeter);
  initLoadSaveConfig(algorithmName);
}

VuMeter::~VuMeter() {
  debug_destruction(VuMeter);
  cvReleaseImage(&mask);
  cvReleaseImage(&background);
  cvReleaseImage(&gray);
}

void VuMeter::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);
  frame = new IplImage(img_input);

  if (firstTime) {
    bgs.SetAlpha(alpha);
    bgs.SetBinSize(binSize);
    bgs.SetThreshold(threshold);

    gray = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
    cvCvtColor(frame, gray, CV_RGB2GRAY);

    background = cvCreateImage(cvGetSize(gray), IPL_DEPTH_8U, 1);
    cvCopy(gray, background);

    mask = cvCreateImage(cvGetSize(gray), IPL_DEPTH_8U, 1);
    cvZero(mask);
  }
  else
    cvCvtColor(frame, gray, CV_RGB2GRAY);

  bgs.UpdateBackground(gray, background, mask);
  img_foreground = cv::cvarrToMat(mask);
  img_background = cv::cvarrToMat(background);

  if (enableFilter) {
    cv::erode(img_foreground, img_foreground, cv::Mat());
    cv::medianBlur(img_foreground, img_foreground, 5);
  }

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;
  firstTime = false;
}

void VuMeter::save_config(cv::FileStorage &fs) {
  fs << "enableFilter" << enableFilter;
  fs << "binSize" << binSize;
  fs << "alpha" << alpha;
  fs << "threshold" << threshold;
  fs << "showOutput" << showOutput;
}

void VuMeter::load_config(cv::FileStorage &fs) {
  fs["enableFilter"] >> enableFilter;
  fs["binSize"] >> binSize;
  fs["alpha"] >> alpha;
  fs["threshold"] >> threshold;
  fs["showOutput"] >> showOutput;
}

#endif
