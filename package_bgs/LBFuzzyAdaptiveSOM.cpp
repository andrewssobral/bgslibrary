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
#include "LBFuzzyAdaptiveSOM.h"

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
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "sensitivity", sensitivity);
  cvWriteInt(fs, "trainingSensitivity", trainingSensitivity);
  cvWriteInt(fs, "learningRate", learningRate);
  cvWriteInt(fs, "trainingLearningRate", trainingLearningRate);
  cvWriteInt(fs, "trainingSteps", trainingSteps);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void LBFuzzyAdaptiveSOM::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  sensitivity = cvReadIntByName(fs, nullptr, "sensitivity", 90);
  trainingSensitivity = cvReadIntByName(fs, nullptr, "trainingSensitivity", 240);
  learningRate = cvReadIntByName(fs, nullptr, "learningRate", 38);
  trainingLearningRate = cvReadIntByName(fs, nullptr, "trainingLearningRate", 255);
  trainingSteps = cvReadIntByName(fs, nullptr, "trainingSteps", 81);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
