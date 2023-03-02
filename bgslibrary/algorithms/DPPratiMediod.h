#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "dp/PratiMediodBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class DPPratiMediod : public IBGS
    {
    private:
      long frameNumber;
      int threshold;
      int samplingRate;
      int historySize;
      int weight;
      dp::RgbImage frame_data;
      dp::PratiParams params;
      dp::PratiMediodBGS bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;

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
