#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 4

#include "lb/BGModelFuzzyGauss.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class LBFuzzyGaussian : public IBGS
    {
    private:
      lb::BGModel* m_pBGModel;
      int sensitivity;
      int bgThreshold;
      int learningRate;
      int noiseVariance;

    public:
      LBFuzzyGaussian();
      ~LBFuzzyGaussian();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(LBFuzzyGaussian);
  }
}

#endif
