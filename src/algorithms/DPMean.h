#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "dp/MeanBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class DPMean : public IBGS
    {
    private:
      long frameNumber;
      int threshold;
      double alpha;
      int learningFrames;
      dp::RgbImage frame_data;
      dp::MeanParams params;
      dp::MeanBGS bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;

    public:
      DPMean();
      ~DPMean();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(DPMean);
  }
}

#endif
