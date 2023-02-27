#pragma once

#include "IBGS.h"

//extern "C" {
#include "SigmaDelta/sdLaMa091.h"
//}

namespace bgslibrary
{
  namespace algorithms
  {
    class SigmaDelta : public IBGS
    {
    private:
      int ampFactor;
      int minVar;
      int maxVar;
      sigmadelta::sdLaMa091_t* algorithm;

    public:
      SigmaDelta();
      ~SigmaDelta();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
      void applyParams();
    };

    bgs_register(SigmaDelta);
  }
}
