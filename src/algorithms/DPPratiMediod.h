#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "dp/PratiMediodBGS.h"

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class DPPratiMediod : public IBGS
    {
    private:
      long frameNumber;
      IplImage* frame;
      RgbImage frame_data;

      PratiParams params;
      PratiMediodBGS bgs;
      BwImage lowThresholdMask;
      BwImage highThresholdMask;

      int threshold;
      int samplingRate;
      int historySize;
      int weight;

    public:
      DPPratiMediod();
      ~DPPratiMediod();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(DPPratiMediod);
  }
}

#endif
