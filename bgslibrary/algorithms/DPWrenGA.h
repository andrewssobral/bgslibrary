#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "dp/WrenGA.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class DPWrenGA : public IBGS
    {
    private:
      long frameNumber;
      double threshold;
      double alpha;
      int learningFrames;
      dp::RgbImage frame_data;
      dp::WrenParams params;
      dp::WrenGA bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;

    public:
      DPWrenGA();
      ~DPWrenGA();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(DPWrenGA);
  }
}

#endif
