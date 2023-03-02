#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "IBGS.h"
#include "T2F/T2FGMM.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class T2FGMM_UM : public IBGS
    {
    private:
      long frameNumber;
      double threshold;
      double alpha;
      float km;
      float kv;
      int gaussians;
      dp::RgbImage frame_data;
      dp::T2FGMMParams params;
      dp::T2FGMM bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;

    public:
      T2FGMM_UM();
      ~T2FGMM_UM();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(T2FGMM_UM);
  }
}

#endif
