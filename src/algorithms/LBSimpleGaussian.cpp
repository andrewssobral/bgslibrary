#include "LBSimpleGaussian.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

LBSimpleGaussian::LBSimpleGaussian() :
  IBGS(quote(LBSimpleGaussian)),
  sensitivity(66), noiseVariance(162), learningRate(18)
{
  debug_construction(LBSimpleGaussian);
  initLoadSaveConfig(algorithmName);
}

LBSimpleGaussian::~LBSimpleGaussian() {
  debug_destruction(LBSimpleGaussian);
  delete m_pBGModel;
}

void LBSimpleGaussian::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage *frame = new IplImage(img_input);

  if (firstTime) {
    int w = cvGetSize(frame).width;
    int h = cvGetSize(frame).height;

    m_pBGModel = new BGModelGauss(w, h);
    m_pBGModel->InitModel(frame);
  }

  m_pBGModel->setBGModelParameter(0, sensitivity);
  m_pBGModel->setBGModelParameter(1, noiseVariance);
  m_pBGModel->setBGModelParameter(2, learningRate);

  m_pBGModel->UpdateModel(frame);

  img_foreground = cv::cvarrToMat(m_pBGModel->GetFG());
  img_background = cv::cvarrToMat(m_pBGModel->GetBG());

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

void LBSimpleGaussian::save_config(cv::FileStorage &fs) {
  fs << "sensitivity" << sensitivity;
  fs << "noiseVariance" << noiseVariance;
  fs << "learningRate" << learningRate;
  fs << "showOutput" << showOutput;
}

void LBSimpleGaussian::load_config(cv::FileStorage &fs) {
  fs["sensitivity"] >> sensitivity;
  fs["noiseVariance"] >> noiseVariance;
  fs["learningRate"] >> learningRate;
  fs["showOutput"] >> showOutput;
}

#endif
