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
#include "TextureBGS.h"
//#include "ConnectedComponents.h"

class DPTextureBGS : public IBGS
{
private:
  bool firstTime;
  bool showOutput;

  int width;
  int height;
  int size;
  TextureBGS bgs;
  IplImage* frame;
  RgbImage image;
  BwImage fgMask;
  BwImage tempMask;
  TextureArray* bgModel;
  RgbImage texture;
  unsigned char* modeArray;
  TextureHistogram* curTextureHist;
  //ConnectedComponents cc;
  //CBlobResult largeBlobs;
  //IplConvKernel* dilateElement;
  //IplConvKernel* erodeElement;
  //bool enableFiltering;

public:
  DPTextureBGS();
  ~DPTextureBGS();

  void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
  void saveConfig();
  void loadConfig();
};
