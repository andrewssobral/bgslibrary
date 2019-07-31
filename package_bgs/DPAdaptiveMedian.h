#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "dp/AdaptiveMedianBGS.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class DPAdaptiveMedian : public IBGS, public ILoadSaveConfig
    {
    private:
      long frameNumber;
      IplImage* frame;
      RgbImage frame_data;
      AdaptiveMedianParams params;
      AdaptiveMedianBGS bgs;
      BwImage lowThresholdMask;
      BwImage highThresholdMask;
      int threshold;
      int samplingRate;
      int learningFrames;

    public:
      DPAdaptiveMedian();
      ~DPAdaptiveMedian();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<DPAdaptiveMedian> register_DPAdaptiveMedian("DPAdaptiveMedian");
  }
}

#endif
