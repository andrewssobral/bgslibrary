#pragma once

#include <opencv2/opencv.hpp>

#include "utils/GenericMacros.h"

namespace bgslibrary
{
  class IFrameProcessor
  {
  public:
    IFrameProcessor(){
      //debug_construction(IFrameProcessor);
    }
    virtual ~IFrameProcessor() {
      //debug_destruction(IFrameProcessor);
    }
    virtual void process(const cv::Mat &input) = 0;
  };
}
