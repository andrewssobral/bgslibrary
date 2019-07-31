#include "DPGrimsonGMM.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

DPGrimsonGMM::DPGrimsonGMM() :
  frameNumber(0), threshold(9.0), alpha(0.01), gaussians(3)
{
  std::cout << "DPGrimsonGMM()" << std::endl;
  setup("./config/DPGrimsonGMM.xml");
}

DPGrimsonGMM::~DPGrimsonGMM()
{
  std::cout << "~DPGrimsonGMM()" << std::endl;
}

void DPGrimsonGMM::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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
    cv::imshow("GMM (Grimson)", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;
  firstTime = false;
  frameNumber++;
}

void DPGrimsonGMM::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);

  fs << "threshold" << threshold;
  fs << "alpha" << alpha;
  fs << "gaussians" << gaussians;
  fs << "showOutput" << showOutput;

  fs.release();
}

void DPGrimsonGMM::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);

  fs["threshold"] >> threshold;
  fs["alpha"] >> alpha;
  fs["gaussians"] >> gaussians;
  fs["showOutput"] >> showOutput;

  fs.release();
}

#endif
