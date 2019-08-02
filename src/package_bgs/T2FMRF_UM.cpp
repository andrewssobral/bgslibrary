#include "T2FMRF_UM.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

T2FMRF_UM::T2FMRF_UM() :
  frameNumber(0), threshold(9.0), alpha(0.01), km(2.f), kv(0.9f), gaussians(3)
{
  std::cout << "T2FMRF_UM()" << std::endl;
  setup("./config/DPMean.xml");
}

T2FMRF_UM::~T2FMRF_UM()
{
  std::cout << "~T2FMRF_UM()" << std::endl;
}

void T2FMRF_UM::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
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
    params.Type() = TYPE_T2FMRF_UM;
    params.KM() = km; // Factor control for the T2FMRF-UM [0,3] default: 2
    params.KV() = kv; // Factor control for the T2FMRF-UV [0.3,1] default: 0.9

    bgs.Initalize(params);
    bgs.InitModel(frame_data);

    old_labeling = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    old = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

    mrf.height = height;
    mrf.width = width;
    mrf.Build_Classes_OldLabeling_InImage_LocalEnergy();

    firstTime = false;
  }

  bgs.Subtract(frameNumber, frame_data, lowThresholdMask, highThresholdMask);
  cvCopy(lowThresholdMask.Ptr(), old);

  /************************************************************************/
  /* the code for MRF, it can be noted when using other methods   */
  /************************************************************************/
  //the optimization process is done when the foreground detection is stable,
  if (frameNumber >= 10)
  {
    gmm = bgs.gmm();
    hmm = bgs.hmm();
    mrf.background2 = frame_data.Ptr();
    mrf.in_image = lowThresholdMask.Ptr();
    mrf.out_image = lowThresholdMask.Ptr();
    mrf.InitEvidence2(gmm, hmm, old_labeling);
    mrf.ICM2();
    cvCopy(mrf.out_image, lowThresholdMask.Ptr());
  }

  cvCopy(old, old_labeling);

  lowThresholdMask.Clear();
  bgs.Update(frameNumber, frame_data, lowThresholdMask);

  img_foreground = cv::cvarrToMat(highThresholdMask.Ptr());
  img_background = cv::cvarrToMat(bgs.Background()->Ptr());
  //img_background = cv::Mat::zeros(img_input.size(), img_input.type());

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow("T2FMRF-UM", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;
  frameNumber++;
}

void T2FMRF_UM::saveConfig()
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

void T2FMRF_UM::loadConfig()
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
