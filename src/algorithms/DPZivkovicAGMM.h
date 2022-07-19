#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "dp/ZivkovicAGMM.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class DPZivkovicAGMM : public IBGS
    {
    private:
      long frameNumber;
      double threshold;
      double alpha;
      int gaussians;
      dp::RgbImage frame_data;
      dp::ZivkovicParams params;
      dp::ZivkovicAGMM bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;

    public:
      DPZivkovicAGMM();
      ~DPZivkovicAGMM();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(DPZivkovicAGMM);
  }
}

#endif
