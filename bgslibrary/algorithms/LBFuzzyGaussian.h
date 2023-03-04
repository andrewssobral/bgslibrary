#pragma once

#include "IBGS.h"
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
