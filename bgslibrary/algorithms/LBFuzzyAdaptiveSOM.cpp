#include "LBFuzzyAdaptiveSOM.h"

using namespace bgslibrary::algorithms;

LBFuzzyAdaptiveSOM::LBFuzzyAdaptiveSOM() :
  IBGS(quote(LBFuzzyAdaptiveSOM)),
  sensitivity(90), trainingSensitivity(240), learningRate(38), 
  trainingLearningRate(255), trainingSteps(81)
{
  debug_construction(LBFuzzyAdaptiveSOM);
  initLoadSaveConfig(algorithmName);
}

LBFuzzyAdaptiveSOM::~LBFuzzyAdaptiveSOM() {
  debug_destruction(LBFuzzyAdaptiveSOM);
  delete m_pBGModel;
}

void LBFuzzyAdaptiveSOM::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage _frame = cvIplImage(img_input);
  IplImage* frame = cvCloneImage(&_frame);

  if (firstTime) {
    int w = img_input.size().width;
    int h = img_input.size().height;

    m_pBGModel = new lb::BGModelFuzzySom(w, h);
    m_pBGModel->InitModel(frame);
  }

  m_pBGModel->setBGModelParameter(0, sensitivity);
  m_pBGModel->setBGModelParameter(1, trainingSensitivity);
  m_pBGModel->setBGModelParameter(2, learningRate);
  m_pBGModel->setBGModelParameter(3, trainingLearningRate);
  m_pBGModel->setBGModelParameter(5, trainingSteps);

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

void LBFuzzyAdaptiveSOM::save_config(cv::FileStorage &fs) {
  fs << "sensitivity" << sensitivity;
  fs << "trainingSensitivity" << trainingSensitivity;
  fs << "learningRate" << learningRate;
  fs << "trainingLearningRate" << trainingLearningRate;
  fs << "trainingSteps" << trainingSteps;
  fs << "showOutput" << showOutput;
}

void LBFuzzyAdaptiveSOM::load_config(cv::FileStorage &fs) {
  fs["sensitivity"] >> sensitivity;
  fs["trainingSensitivity"] >> trainingSensitivity;
  fs["learningRate"] >> learningRate;
  fs["trainingLearningRate"] >> trainingLearningRate;
  fs["trainingSteps"] >> trainingSteps;
  fs["showOutput"] >> showOutput;
}
