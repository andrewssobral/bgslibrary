#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "dp/GrimsonGMM.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class DPGrimsonGMM : public IBGS
    {
    private:
      long frameNumber;
      double threshold;
      double alpha;
      int gaussians;
      dp::RgbImage frame_data;
      dp::GrimsonParams params;
      dp::GrimsonGMM bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;

    public:
      DPGrimsonGMM();
      ~DPGrimsonGMM();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(DPGrimsonGMM);
  }
}

#endif
