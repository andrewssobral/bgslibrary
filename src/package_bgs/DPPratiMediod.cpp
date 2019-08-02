#include "DPPratiMediod.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

DPPratiMediod::DPPratiMediod() :
  frameNumber(0), threshold(30), samplingRate(5), historySize(16), weight(5)
{
  std::cout << "DPPratiMediod()" << std::endl;
  setup("./config/DPPratiMediod.xml");
}

DPPratiMediod::~DPPratiMediod()
{
  std::cout << "~DPPratiMediod()" << std::endl;
}

void DPPratiMediod::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  frame = new IplImage(img_input);

  if (firstTime)
    frame_data.ReleaseMemory(false);
  frame_data = frame;

  if (firstTime)
  {
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
  if (showOutput)
  {
    cv::imshow("Temporal Median FG (Cucchiara&Calderara)", img_foreground);
    cv::imshow("Temporal Median BG (Cucchiara&Calderara)", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;
  firstTime = false;
  frameNumber++;
}

void DPPratiMediod::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);

  fs << "threshold" << threshold;
  fs << "samplingRate" << samplingRate;
  fs << "historySize" << historySize;
  fs << "weight" << weight;
  fs << "showOutput" << showOutput;

  fs.release();
}

void DPPratiMediod::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);

  fs["threshold"] >> threshold;
  fs["samplingRate"] >> samplingRate;
  fs["historySize"] >> historySize;
  fs["weight"] >> weight;
  fs["showOutput"] >> showOutput;

  fs.release();
}

#endif
