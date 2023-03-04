#include "LBFuzzyGaussian.h"

using namespace bgslibrary::algorithms;

LBFuzzyGaussian::LBFuzzyGaussian() :
  IBGS(quote(LBFuzzyGaussian)),
  sensitivity(72), bgThreshold(162), 
  learningRate(49), noiseVariance(195)
{
  debug_construction(LBFuzzyGaussian);
  initLoadSaveConfig(algorithmName);
}

LBFuzzyGaussian::~LBFuzzyGaussian() {
  debug_destruction(LBFuzzyGaussian);
  delete m_pBGModel;
}

void LBFuzzyGaussian::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage _frame = cvIplImage(img_input);
  IplImage* frame = cvCloneImage(&_frame);

  if (firstTime) {
    int w = img_input.size().width;
    int h = img_input.size().height;

    m_pBGModel = new lb::BGModelFuzzyGauss(w, h);
    m_pBGModel->InitModel(frame);
  }

  m_pBGModel->setBGModelParameter(0, sensitivity);
  m_pBGModel->setBGModelParameter(1, bgThreshold);
  m_pBGModel->setBGModelParameter(2, learningRate);
  m_pBGModel->setBGModelParameter(3, noiseVariance);

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
  cvReleaseImage(&frame);

  firstTime = false;
}

void LBFuzzyGaussian::save_config(cv::FileStorage &fs) {
  fs << "sensitivity" << sensitivity;
  fs << "bgThreshold" << bgThreshold;
  fs << "learningRate" << learningRate;
  fs << "noiseVariance" << noiseVariance;
  fs << "showOutput" << showOutput;
}

void LBFuzzyGaussian::load_config(cv::FileStorage &fs) {
  fs["sensitivity"] >> sensitivity;
  fs["bgThreshold"] >> bgThreshold;
  fs["learningRate"] >> learningRate;
  fs["noiseVariance"] >> noiseVariance;
  fs["showOutput"] >> showOutput;
}
