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
#include "KNN.h"

#if CV_MAJOR_VERSION >= 3

using namespace bgslibrary::algorithms;

KNN::KNN() :
  history(500), nSamples(7), dist2Threshold(20.0f * 20.0f), knnSamples(0),
  doShadowDetection(true), shadowValue(127), shadowThreshold(0.5f)
{
  std::cout << "KNN()" << std::endl;
  setup("./config/KNN.xml");
}

KNN::~KNN()
{
  std::cout << "~KNN()" << std::endl;
}

void KNN::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  //------------------------------------------------------------------
  // BackgroundSubtractorKNN
  // http://docs.opencv.org/trunk/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractorknn
  //
  //  The class implements the K nearest neigbours algorithm from:
  //  "Efficient Adaptive Density Estimation per Image Pixel for the Task of Background Subtraction"
  //  Z.Zivkovic, F. van der Heijden
  //  Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006
  //  http:  //www.zoranz.net/Publications/zivkovicPRL2006.pdf
  //
  //  Fast for small foreground object. Results on the benchmark data is at http://www.changedetection.net.
  //------------------------------------------------------------------

  int prevNSamples = nSamples;
  if (firstTime)
    knn = cv::createBackgroundSubtractorKNN(history, dist2Threshold, doShadowDetection);

  knn->setNSamples(nSamples);
  knn->setkNNSamples(knnSamples);
  knn->setShadowValue(shadowValue);
  knn->setShadowThreshold(shadowThreshold);

  knn->apply(img_input, img_foreground, prevNSamples != nSamples ? 0.f : 1.f);
  knn->getBackgroundImage(img_background);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("KNN FG", img_foreground);
    cv::imshow("KNN BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void KNN::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "history", history);
  cvWriteInt(fs, "nSamples", nSamples);
  cvWriteReal(fs, "dist2Threshold", dist2Threshold);
  cvWriteInt(fs, "knnSamples", knnSamples);
  cvWriteInt(fs, "doShadowDetection", doShadowDetection);
  cvWriteInt(fs, "shadowValue", shadowValue);
  cvWriteReal(fs, "shadowThreshold", shadowThreshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void KNN::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  history = cvReadIntByName(fs, nullptr, "history", 500);
  nSamples = cvReadIntByName(fs, nullptr, "nSamples", 7);
  dist2Threshold = cvReadRealByName(fs, nullptr, "dist2Threshold", 20.0f * 20.0f);
  knnSamples = cvReadIntByName(fs, nullptr, "knnSamples", 0);
  doShadowDetection = cvReadIntByName(fs, nullptr, "doShadowDetection", 1);
  shadowValue = cvReadIntByName(fs, nullptr, "shadowValue", 127);
  shadowThreshold = cvReadRealByName(fs, nullptr, "shadowThreshold", 0.5f);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}

#endif
