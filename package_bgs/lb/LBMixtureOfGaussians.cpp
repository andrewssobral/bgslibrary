/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "LBMixtureOfGaussians.h"

LBMixtureOfGaussians::LBMixtureOfGaussians() : firstTime(true), showOutput(true), sensitivity(81), bgThreshold(83), learningRate(59), noiseVariance(206)
{
  std::cout << "LBMixtureOfGaussians()" << std::endl;
}

LBMixtureOfGaussians::~LBMixtureOfGaussians()
{
  delete m_pBGModel;
  std::cout << "~LBMixtureOfGaussians()" << std::endl;
}

void LBMixtureOfGaussians::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();
  
  IplImage *frame = new IplImage(img_input);
  
  if(firstTime)
  {
    saveConfig();

    int w = cvGetSize(frame).width;
    int h = cvGetSize(frame).height;

    m_pBGModel = new BGModelMog(w,h);
    m_pBGModel->InitModel(frame);
  }
  
  m_pBGModel->setBGModelParameter(0,sensitivity);
  m_pBGModel->setBGModelParameter(1,bgThreshold);
  m_pBGModel->setBGModelParameter(2,learningRate);
  m_pBGModel->setBGModelParameter(3,noiseVariance);

  m_pBGModel->UpdateModel(frame);

  img_foreground = cv::Mat(m_pBGModel->GetFG());
  img_background = cv::Mat(m_pBGModel->GetBG());
    
  if(showOutput)
  {
    cv::imshow("MOG Mask", img_foreground);
    cv::imshow("MOG Model", img_background);
  }

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
  
  delete frame;
  
  firstTime = false;
}

//void LBMixtureOfGaussians::finish(void)
//{
//  delete m_pBGModel;
//}

void LBMixtureOfGaussians::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/LBMixtureOfGaussians.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "sensitivity", sensitivity);
  cvWriteInt(fs, "bgThreshold", bgThreshold);
  cvWriteInt(fs, "learningRate", learningRate);
  cvWriteInt(fs, "noiseVariance", noiseVariance);
  
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LBMixtureOfGaussians::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/LBMixtureOfGaussians.xml", 0, CV_STORAGE_READ);
  
  sensitivity = cvReadIntByName(fs, 0, "sensitivity", 81);
  bgThreshold = cvReadIntByName(fs, 0, "bgThreshold", 83);
  learningRate = cvReadIntByName(fs, 0, "learningRate", 59);
  noiseVariance = cvReadIntByName(fs, 0, "noiseVariance", 206);
  
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}