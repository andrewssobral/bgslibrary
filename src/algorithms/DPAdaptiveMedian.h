#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "IBGS.h"
#include "dp/AdaptiveMedianBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class DPAdaptiveMedian : public IBGS
    {
    private:
      long frameNumber;
      int threshold;
      int samplingRate;
      int learningFrames;
      dp::RgbImage frame_data;
      dp::AdaptiveMedianParams params;
      dp::AdaptiveMedianBGS bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;

    public:
      DPAdaptiveMedian();
      ~DPAdaptiveMedian();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(DPAdaptiveMedian);
  }
}

#endif
