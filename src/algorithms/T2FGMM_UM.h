#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "IBGS.h"
#include "T2F/T2FGMM.h"

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class T2FGMM_UM : public IBGS
    {
    private:
      long frameNumber;
      IplImage* frame;
      RgbImage frame_data;

      T2FGMMParams params;
      T2FGMM bgs;
      BwImage lowThresholdMask;
      BwImage highThresholdMask;

      double threshold;
      double alpha;
      float km;
      float kv;
      int gaussians;

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
