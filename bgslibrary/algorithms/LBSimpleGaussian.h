#pragma once

#include "IBGS.h"
#include "lb/BGModelGauss.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class LBSimpleGaussian : public IBGS
    {
    private:
      lb::BGModel* m_pBGModel;
      int sensitivity;
      int noiseVariance;
      int learningRate;

    public:
      LBSimpleGaussian();
      ~LBSimpleGaussian();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(LBSimpleGaussian);
  }
}
