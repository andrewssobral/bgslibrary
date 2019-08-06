#include "T2FGMM_UV.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

T2FGMM_UV::T2FGMM_UV() :
  frameNumber(0), threshold(9.0), alpha(0.01), km(1.5f), kv(0.6f), gaussians(3)
{
  std::cout << "T2FGMM_UV()" << std::endl;
  setup("./config/T2FGMM_UV.xml");
}

T2FGMM_UV::~T2FGMM_UV()
{
  std::cout << "~T2FGMM_UV()" << std::endl;
}

void T2FGMM_UV::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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
    params.HighThreshold() = 2 * params.LowThreshold();
    params.Alpha() = alpha;
    params.MaxModes() = gaussians;
    params.Type() = TYPE_T2FGMM_UV;
    params.KM() = km; // Factor control for the T2FGMM-UM [0,3] default: 1.5
    params.KV() = kv; // Factor control for the T2FGMM-UV [0.3,1] default: 0.6

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
    cv::imshow("T2FGMM-UV", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;
  firstTime = false;
  frameNumber++;
}

void T2FGMM_UV::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  fs << "threshold" << threshold;
  fs << "alpha" << alpha;
  fs << "km" << km;
  fs << "kv" << kv;
  fs << "gaussians" << gaussians;
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void T2FGMM_UV::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  fs["threshold"] >> threshold;
  fs["alpha"] >> alpha;
  fs["km"] >> km;
  fs["kv"] >> kv;
  fs["gaussians"] >> gaussians;
  fs["showOutput"] >> showOutput;
  
  fs.release();
}

#endif
