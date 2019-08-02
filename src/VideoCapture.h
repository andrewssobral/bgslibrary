#pragma once

#include <iostream>
#include <fstream>
#include <memory>
//#include <chrono>
//#include <thread>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/imgproc/types_c.h>

#include "Config.h"
#include "IFrameProcessor.h"
#include "package_bgs/ILoadSaveConfig.h"

namespace bgslibrary
{
  class VideoCapture : public ILoadSaveConfig
  {
  private:
    std::shared_ptr<IFrameProcessor> frameProcessor;
    cv::VideoCapture capture;
    cv::Mat frame;
    int key;
    int64 start_time;
    int64 delta_time;
    double freq;
    double fps;
    int frameNumber;
    int stopAt;
    bool useCamera;
    int cameraIndex;
    bool useVideo;
    std::string videoFileName;
    int input_resize_percent;
    bool showOutput;
    bool enableFlip;
    double loopDelay = 33.333;
    bool firstTime = true;

  public:
    VideoCapture();
    ~VideoCapture();

    void setFrameProcessor(std::shared_ptr<IFrameProcessor> frameProcessorPtr);
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
