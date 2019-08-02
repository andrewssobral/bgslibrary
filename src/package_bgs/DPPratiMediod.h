#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "dp/PratiMediodBGS.h"

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class DPPratiMediod : public IBGS, public ILoadSaveConfig
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
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<DPPratiMediod> register_DPPratiMediod("DPPratiMediod");
  }
}

#endif
