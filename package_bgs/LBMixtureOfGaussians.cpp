#include "LBMixtureOfGaussians.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

LBMixtureOfGaussians::LBMixtureOfGaussians() :
  sensitivity(81), bgThreshold(83), learningRate(59), noiseVariance(206)
{
  std::cout << "LBMixtureOfGaussians()" << std::endl;
  setup("./config/LBMixtureOfGaussians.xml");
}

LBMixtureOfGaussians::~LBMixtureOfGaussians()
{
  delete m_pBGModel;
  std::cout << "~LBMixtureOfGaussians()" << std::endl;
}

void LBMixtureOfGaussians::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage *frame = new IplImage(img_input);

  if (firstTime)
  {
    int w = cvGetSize(frame).width;
    int h = cvGetSize(frame).height;

    m_pBGModel = new BGModelMog(w, h);
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
  if (showOutput)
  {
    cv::imshow("MOG Mask", img_foreground);
    cv::imshow("MOG Model", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;

  firstTime = false;
}

void LBMixtureOfGaussians::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  fs << "sensitivity" << sensitivity;
  fs << "bgThreshold" << bgThreshold;
  fs << "learningRate" << learningRate;
  fs << "noiseVariance" << noiseVariance;
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void LBMixtureOfGaussians::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  fs["sensitivity"] >> sensitivity;
  fs["bgThreshold"] >> bgThreshold;
  fs["learningRate"] >> learningRate;
  fs["noiseVariance"] >> noiseVariance;
  fs["showOutput"] >> showOutput;
  
  fs.release();
}

#endif
