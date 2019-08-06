#include "LBAdaptiveSOM.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

LBAdaptiveSOM::LBAdaptiveSOM() :
  sensitivity(75), trainingSensitivity(245),
  learningRate(62), trainingLearningRate(255),
  trainingSteps(55)
{
  std::cout << "LBAdaptiveSOM()" << std::endl;
  setup("./config/LBAdaptiveSOM.xml");
}

LBAdaptiveSOM::~LBAdaptiveSOM()
{
  delete m_pBGModel;
  std::cout << "~LBAdaptiveSOM()" << std::endl;
}

void LBAdaptiveSOM::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage *frame = new IplImage(img_input);

  if (firstTime)
  {
    int w = cvGetSize(frame).width;
    int h = cvGetSize(frame).height;

    m_pBGModel = new BGModelSom(w, h);
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
    cv::imshow("SOM Mask", img_foreground);
    cv::imshow("SOM Model", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;

  firstTime = false;
}

void LBAdaptiveSOM::saveConfig()
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

void LBAdaptiveSOM::loadConfig()
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
