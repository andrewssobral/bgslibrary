#pragma once

#include <iostream>
#include <sstream>

#include "VideoCapture.h"
#include "FrameProcessor.h"

namespace bgslibrary
{
  class VideoAnalysis
  {
  private:
    std::unique_ptr<VideoCapture> videoCapture;
    std::shared_ptr<FrameProcessor> frameProcessor;
    bool use_file;
    std::string filename;
    bool use_camera;
    int cameraIndex;
    bool use_comp;
    long frameToStop;
    std::string imgref;

  public:
    VideoAnalysis();
    ~VideoAnalysis();

    bool setup(int argc, const char **argv);
    void start();
  };
}
