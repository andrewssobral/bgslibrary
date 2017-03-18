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
#include "DPPratiMediod.h"

using namespace bgslibrary::algorithms;

DPPratiMediod::DPPratiMediod() :
  frameNumber(0), threshold(30), samplingRate(5), historySize(16), weight(5)
{
  std::cout << "DPPratiMediod()" << std::endl;
  setup("./config/DPPratiMediod.xml");
}

DPPratiMediod::~DPPratiMediod()
{
  std::cout << "~DPPratiMediod()" << std::endl;
}

void DPPratiMediod::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  frame = new IplImage(img_input);

  if (firstTime)
    frame_data.ReleaseMemory(false);
  frame_data = frame;

  if (firstTime)
  {
    int width = img_input.size().width;
    int height = img_input.size().height;

    lowThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    lowThresholdMask.Ptr()->origin = IPL_ORIGIN_BL;

    highThresholdMask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
    highThresholdMask.Ptr()->origin = IPL_ORIGIN_BL;

    params.SetFrameSize(width, height);
    params.LowThreshold() = threshold;
    params.HighThreshold() = 2 * params.LowThreshold();	// Note: high threshold is used by post-processing
    params.SamplingRate() = samplingRate;
    params.HistorySize() = historySize;
    params.Weight() = weight;

    bgs.Initalize(params);
    bgs.InitModel(frame_data);
  }

  bgs.Subtract(frameNumber, frame_data, lowThresholdMask, highThresholdMask);
  lowThresholdMask.Clear();
  bgs.Update(frameNumber, frame_data, lowThresholdMask);

  img_foreground = cv::cvarrToMat(highThresholdMask.Ptr());
  img_background = cv::cvarrToMat(bgs.Background()->Ptr());

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("Temporal Median FG (Cucchiara&Calderara)", img_foreground);
    cv::imshow("Temporal Median BG (Cucchiara&Calderara)", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete frame;
  firstTime = false;
  frameNumber++;
}

void DPPratiMediod::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "samplingRate", samplingRate);
  cvWriteInt(fs, "historySize", historySize);
  cvWriteInt(fs, "weight", weight);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void DPPratiMediod::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  threshold = cvReadIntByName(fs, nullptr, "threshold", 30);
  samplingRate = cvReadIntByName(fs, nullptr, "samplingRate", 5);
  historySize = cvReadIntByName(fs, nullptr, "historySize", 16);
  weight = cvReadIntByName(fs, nullptr, "weight", 5);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
