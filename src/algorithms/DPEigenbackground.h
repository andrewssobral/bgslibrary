#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "dp/Eigenbackground.h"

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class DPEigenbackground : public IBGS
    {
    private:
      long frameNumber;
      IplImage* frame;
      RgbImage frame_data;

      EigenbackgroundParams params;
      Eigenbackground bgs;
      BwImage lowThresholdMask;
      BwImage highThresholdMask;

      int threshold;
      int historySize;
      int embeddedDim;

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
