#pragma once

#include "IBGS.h"
#include "lb/BGModelMog.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class LBMixtureOfGaussians : public IBGS
    {
    private:
      lb::BGModel* m_pBGModel;
      int sensitivity;
      int bgThreshold;
      int learningRate;
      int noiseVariance;

    public:
      LBMixtureOfGaussians();
      ~LBMixtureOfGaussians();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(LBMixtureOfGaussians);
  }
}
