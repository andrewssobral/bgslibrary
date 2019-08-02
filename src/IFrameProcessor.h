#pragma once

#include <opencv2/opencv.hpp>

namespace bgslibrary
{
  class IFrameProcessor
  {
  public:
    virtual void process(const cv::Mat &input) = 0;
    virtual ~IFrameProcessor(){}
  };
}
