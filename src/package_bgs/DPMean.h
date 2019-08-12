#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "dp/MeanBGS.h"

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class DPMean : public IBGS, public ILoadSaveConfig
    {
    private:
      long frameNumber;
      IplImage* frame;
      RgbImage frame_data;

      MeanParams params;
      MeanBGS bgs;
      BwImage lowThresholdMask;
      BwImage highThresholdMask;

      int threshold;
      double alpha;
      int learningFrames;

    public:
      DPMean();
      ~DPMean();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(DPMean);
  }
}

#endif
