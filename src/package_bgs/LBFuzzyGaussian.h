#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "lb/BGModelFuzzyGauss.h"

using namespace lb_library;
using namespace lb_library::FuzzyGaussian;

namespace bgslibrary
{
  namespace algorithms
  {
    class LBFuzzyGaussian : public IBGS, public ILoadSaveConfig
    {
    private:
      BGModel* m_pBGModel;
      int sensitivity;
      int bgThreshold;
      int learningRate;
      int noiseVariance;

    public:
      LBFuzzyGaussian();
      ~LBFuzzyGaussian();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(LBFuzzyGaussian);
  }
}

#endif
