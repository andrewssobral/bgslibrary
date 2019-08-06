#include "LBFuzzyAdaptiveSOM.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

LBFuzzyAdaptiveSOM::LBFuzzyAdaptiveSOM() :
  sensitivity(90), trainingSensitivity(240), learningRate(38), trainingLearningRate(255), trainingSteps(81)
{
  std::cout << "LBFuzzyAdaptiveSOM()" << std::endl;
  setup("./config/LBFuzzyAdaptiveSOM.xml");
}

LBFuzzyAdaptiveSOM::~LBFuzzyAdaptiveSOM()
{
  delete m_pBGModel;
  std::cout << "~LBFuzzyAdaptiveSOM()" << std::endl;
}

void LBFuzzyAdaptiveSOM::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage *frame = new IplImage(img_input);

  if (firstTime)
  {
    int w = cvGetSize(frame).width;
    int h = cvGetSize(frame).height;

    m_pBGModel = new BGModelFuzzySom(w, h);
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
  if (showOutput)
  {
    cv::imshow("FSOM Mask", img_foreground);
    cv::imshow("FSOM Model", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;

  firstTime = false;
}

void LBFuzzyAdaptiveSOM::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  fs << "sensitivity" << sensitivity;
  fs << "trainingSensitivity" << trainingSensitivity;
  fs << "learningRate" << learningRate;
  fs << "trainingLearningRate" << trainingLearningRate;
  fs << "trainingSteps" << trainingSteps;
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void LBFuzzyAdaptiveSOM::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  fs["sensitivity"] >> sensitivity;
  fs["trainingSensitivity"] >> trainingSensitivity;
  fs["learningRate"] >> learningRate;
  fs["trainingLearningRate"] >> trainingLearningRate;
  fs["trainingSteps"] >> trainingSteps;
  fs["showOutput"] >> showOutput;
  
  fs.release();
}

#endif
