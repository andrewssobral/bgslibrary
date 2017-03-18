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
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>

#include "Config.h"
#include "IFrameProcessor.h"

namespace bgslibrary
{
  class VideoCapture
  {
  private:
    IFrameProcessor* frameProcessor;
    cv::VideoCapture capture;
    IplImage* frame;
    int key;
    int64 start_time;
    int64 delta_time;
    double freq;
    double fps;
    long frameNumber;
    long stopAt;
    bool useCamera;
    int cameraIndex;
    bool useVideo;
    std::string videoFileName;
    int input_resize_percent;
    bool showOutput;
    bool enableFlip;

  public:
    VideoCapture();
    ~VideoCapture();

    void setFrameProcessor(IFrameProcessor* frameProcessorPtr);
    void setCamera(int cameraIndex);
    void setVideo(std::string filename);
    void start();

  private:
    void setUpCamera();
    void setUpVideo();

    void saveConfig();
    void loadConfig();
  };
}
