#include "DPEigenbackground.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

DPEigenbackground::DPEigenbackground() :
  frameNumber(0), threshold(225), historySize(20), embeddedDim(10)
{
  std::cout << "DPEigenbackground()" << std::endl;
  setup("./config/DPEigenbackground.xml");
}

DPEigenbackground::~DPEigenbackground()
{
  std::cout << "~DPEigenbackground()" << std::endl;
}

void DPEigenbackground::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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
    params.LowThreshold() = threshold; //15*15;
    params.HighThreshold() = 2 * params.LowThreshold();	// Note: high threshold is used by post-processing
    //params.HistorySize() = 100;
    params.HistorySize() = historySize;
    //params.EmbeddedDim() = 20;
    params.EmbeddedDim() = embeddedDim;

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
    cv::imshow("Eigenbackground (Oliver)", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;
  firstTime = false;
  frameNumber++;
}

void DPEigenbackground::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);

  fs << "threshold" << threshold;
  fs << "historySize" << historySize;
  fs << "embeddedDim" << embeddedDim;
  fs << "showOutput" << showOutput;

  fs.release();
}

void DPEigenbackground::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);

  fs["threshold"] >> threshold;
  fs["historySize"] >> historySize;
  fs["embeddedDim"] >> embeddedDim;
  fs["showOutput"] >> showOutput;

  fs.release();
}

#endif
