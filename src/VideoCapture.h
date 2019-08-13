#pragma once

#include <iostream>
#include <fstream>
#include <memory>
//#include <chrono>
//#include <thread>
#include <opencv2/opencv.hpp>
// opencv legacy includes
//#include <opencv2/highgui/highgui_c.h>
//#include <opencv2/imgproc/imgproc_c.h>
//#include <opencv2/imgproc/types_c.h>

#include "utils/GenericKeys.h"
#include "utils/ILoadSaveConfig.h"
#include "IFrameProcessor.h"

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
    bool showFPS;
    bool enableFlip;
    double loopDelay = 33.333;
    bool firstTime = true;

  public:
    VideoCapture();
    ~VideoCapture();

    void setFrameProcessor(const std::shared_ptr<IFrameProcessor> &_frameProcessor);
    void setCamera(int _index);
    void setVideo(std::string _filename);
    void start();

  private:
    void setUpCamera();
    void setUpVideo();
    
    void save_config(cv::FileStorage &fs);
    void load_config(cv::FileStorage &fs);
  };
}
