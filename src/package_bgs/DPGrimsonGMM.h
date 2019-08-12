#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "dp/GrimsonGMM.h"

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class DPGrimsonGMM : public IBGS, public ILoadSaveConfig
    {
    private:
      long frameNumber;
      IplImage* frame;
      RgbImage frame_data;

      GrimsonParams params;
      GrimsonGMM bgs;
      BwImage lowThresholdMask;
      BwImage highThresholdMask;

      double threshold;
      double alpha;
      int gaussians;

    public:
      DPGrimsonGMM();
      ~DPGrimsonGMM();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(DPGrimsonGMM);
  }
}

#endif
