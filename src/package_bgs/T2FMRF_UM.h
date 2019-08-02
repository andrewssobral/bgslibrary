#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "IBGS.h"
#include "ILoadSaveConfig.h"
#include "T2F/MRF.h"

using namespace Algorithms::BackgroundSubtraction;

namespace bgslibrary
{
  namespace algorithms
  {
    class T2FMRF_UM : public IBGS, public ILoadSaveConfig
    {
    private:
      long frameNumber;
      IplImage *frame;
      RgbImage frame_data;

      IplImage *old_labeling;
      IplImage *old;

      T2FMRFParams params;
      T2FMRF bgs;
      BwImage lowThresholdMask;
      BwImage highThresholdMask;

      double threshold;
      double alpha;
      float km;
      float kv;
      int gaussians;

      MRF_TC mrf;
      GMM *gmm;
      HMM *hmm;

    public:
      T2FMRF_UM();
      ~T2FMRF_UM();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<T2FMRF_UM> register_T2FMRF_UM("T2FMRF_UM");
  }
}

#endif
