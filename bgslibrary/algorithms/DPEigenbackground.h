#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "dp/Eigenbackground.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class DPEigenbackground : public IBGS
    {
    private:
      long frameNumber;
      int threshold;
      int historySize;
      int embeddedDim;
      dp::RgbImage frame_data;
      dp::EigenbackgroundParams params;
      dp::Eigenbackground bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;

    public:
      DPEigenbackground();
      ~DPEigenbackground();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(DPEigenbackground);
  }
}

#endif
