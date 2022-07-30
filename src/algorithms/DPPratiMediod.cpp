#include "DPPratiMediod.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

DPPratiMediod::DPPratiMediod() :
  IBGS(quote(DPPratiMediod)),
  frameNumber(0), threshold(30), samplingRate(5), 
  historySize(16), weight(5)
{
  debug_construction(DPPratiMediod);
  initLoadSaveConfig(algorithmName);
}

DPPratiMediod::~DPPratiMediod() {
  debug_destruction(DPPratiMediod);
}

void DPPratiMediod::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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
    params.LowThreshold() = threshold;
    params.HighThreshold() = 2 * params.LowThreshold();	// Note: high threshold is used by post-processing
    params.SamplingRate() = samplingRate;
    params.HistorySize() = historySize;
    params.Weight() = weight;

    bgs.Initalize(params);
    bgs.InitModel(frame_data);
  }

  bgs.Subtract(frameNumber, frame_data, lowThresholdMask, highThresholdMask);
  lowThresholdMask.Clear();
  bgs.Update(frameNumber, frame_data, lowThresholdMask);

  img_foreground = cv::cvarrToMat(highThresholdMask.Ptr());
  img_background = cv::cvarrToMat(bgs.Background()->Ptr());

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
  frame_data.ReleaseImage();

  firstTime = false;
  frameNumber++;
}

void DPPratiMediod::save_config(cv::FileStorage &fs) {
  fs << "threshold" << threshold;
  fs << "samplingRate" << samplingRate;
  fs << "historySize" << historySize;
  fs << "weight" << weight;
  fs << "showOutput" << showOutput;
}

void DPPratiMediod::load_config(cv::FileStorage &fs) {
  fs["threshold"] >> threshold;
  fs["samplingRate"] >> samplingRate;
  fs["historySize"] >> historySize;
  fs["weight"] >> weight;
  fs["showOutput"] >> showOutput;
}

#endif
