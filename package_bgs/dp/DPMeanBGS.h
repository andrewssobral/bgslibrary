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
#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>


#include "../IBGS.h"
#include "MeanBGS.h"

using namespace Algorithms::BackgroundSubtraction;

class DPMeanBGS : public IBGS
{
private:
  bool firstTime;
  long frameNumber;
  IplImage* frame;
  BgsRgbImage frame_data;

  MeanParams params;
  MeanBGS bgs;
  BgsBwImage lowThresholdMask;
  BgsBwImage highThresholdMask;

  int threshold;
  double alpha;
  int learningFrames;
  bool showOutput;

public:
  DPMeanBGS();
  ~DPMeanBGS();

  void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
  void saveConfig();
  void loadConfig();
};

