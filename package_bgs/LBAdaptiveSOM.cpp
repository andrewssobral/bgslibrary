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
#include "LBAdaptiveSOM.h"

using namespace bgslibrary::algorithms;

LBAdaptiveSOM::LBAdaptiveSOM() :
  sensitivity(75), trainingSensitivity(245), learningRate(62), trainingLearningRate(255), trainingSteps(55)
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
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "sensitivity", sensitivity);
  cvWriteInt(fs, "trainingSensitivity", trainingSensitivity);
  cvWriteInt(fs, "learningRate", learningRate);
  cvWriteInt(fs, "trainingLearningRate", trainingLearningRate);
  cvWriteInt(fs, "trainingSteps", trainingSteps);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LBAdaptiveSOM::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  sensitivity = cvReadIntByName(fs, nullptr, "sensitivity", 75);
  trainingSensitivity = cvReadIntByName(fs, nullptr, "trainingSensitivity", 245);
  learningRate = cvReadIntByName(fs, nullptr, "learningRate", 62);
  trainingLearningRate = cvReadIntByName(fs, nullptr, "trainingLearningRate", 255);
  trainingSteps = cvReadIntByName(fs, nullptr, "trainingSteps", 55);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
