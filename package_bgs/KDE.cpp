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
#include "KDE.h"

using namespace bgslibrary::algorithms;

KDE::KDE() :
  SequenceLength(50), TimeWindowSize(100), SDEstimationFlag(1), lUseColorRatiosFlag(1),
  th(10e-8), alpha(0.3), framesToLearn(10), frameNumber(0)
{
  p = new NPBGSubtractor;
  std::cout << "KDE()" << std::endl;
  setup("./config/KDE.xml");
}

KDE::~KDE()
{
  delete FGImage;
  delete p;
  std::cout << "~KDE()" << std::endl;
}

void KDE::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime)
  {
    rows = img_input.size().height;
    cols = img_input.size().width;
    color_channels = img_input.channels();

    // SequenceLength: number of samples for each pixel.
    // TimeWindowSize: Time window for sampling. for example in the call above, the bg will sample 50 points out of 100 frames.
    // this rate will affect how fast the model adapt.
    // SDEstimationFlag: True means to estimate suitable kernel bandwidth to each pixel, False uses a default value.
    // lUseColorRatiosFlag: True means use normalized RGB for color (recommended.)
    p->Intialize(rows, cols, color_channels, SequenceLength, TimeWindowSize, SDEstimationFlag, lUseColorRatiosFlag);
    // th: 0-1 is the probability threshold for a pixel to be a foregroud. typically make it small as 10e-8. the smaller the value the less false positive and more false negative.
    // alpha: 0-1, for color. typically set to 0.3. this affect shadow suppression.
    p->SetThresholds(th, alpha);

    FGImage = new unsigned char[rows*cols];
    //FilteredFGImage = new unsigned char[rows*cols];
    FilteredFGImage = 0;
    DisplayBuffers = 0;

    img_foreground = cv::Mat::zeros(img_input.size(), CV_8UC1);
    img_background = cv::Mat::zeros(img_input.size(), img_input.type());

    frameNumber = 0;
    firstTime = false;
  }

  // Stores the first N frames to build the background model
  if (frameNumber < framesToLearn)
  {
    p->AddFrame(img_input.data);
    frameNumber++;
  }
  else
  {
    // Build the background model with first 10 frames
    if (frameNumber == framesToLearn)
    {
      p->Estimation();
      frameNumber++;
    }

    // Now, we can subtract the background
    ((NPBGSubtractor*)p)->NBBGSubtraction(img_input.data, FGImage, FilteredFGImage, DisplayBuffers);

    // At each frame also you can call the update function to adapt the bg
    // here you pass a mask where pixels with true value will be masked out of the update.
    ((NPBGSubtractor*)p)->Update(FGImage);

    img_foreground.data = FGImage;
  }

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow("KDE", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void KDE::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "framesToLearn", framesToLearn);
  cvWriteInt(fs, "SequenceLength", SequenceLength);
  cvWriteInt(fs, "TimeWindowSize", TimeWindowSize);
  cvWriteInt(fs, "SDEstimationFlag", SDEstimationFlag);
  cvWriteInt(fs, "lUseColorRatiosFlag", lUseColorRatiosFlag);
  cvWriteReal(fs, "th", th);
  cvWriteReal(fs, "alpha", alpha);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void KDE::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  framesToLearn = cvReadIntByName(fs, nullptr, "framesToLearn", 10);
  SequenceLength = cvReadIntByName(fs, nullptr, "SequenceLength", 50);
  TimeWindowSize = cvReadIntByName(fs, nullptr, "TimeWindowSize", 100);
  SDEstimationFlag = cvReadIntByName(fs, nullptr, "SDEstimationFlag", 1);
  lUseColorRatiosFlag = cvReadIntByName(fs, nullptr, "lUseColorRatiosFlag", 1);
  th = cvReadRealByName(fs, nullptr, "th", 10e-8);
  alpha = cvReadRealByName(fs, nullptr, "alpha", 0.3);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
