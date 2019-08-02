#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "IBGS.h"
#include "ILoadSaveConfig.h"
#include "T2F/T2FGMM.h"

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class T2FGMM_UV : public IBGS, public ILoadSaveConfig
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
      T2FGMM_UV();
      ~T2FGMM_UV();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<T2FGMM_UV> register_T2FGMM_UV("T2FGMM_UV");
  }
}

#endif
