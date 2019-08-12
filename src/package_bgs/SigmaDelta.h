#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

//extern "C" {
#include "SigmaDelta/sdLaMa091.h"
//}

namespace bgslibrary
{
  namespace algorithms
  {
    class SigmaDelta : public IBGS, public ILoadSaveConfig
    {
    private:
      int ampFactor;
      int minVar;
      int maxVar;
      sdLaMa091_t* algorithm;

    public:
      SigmaDelta();
      ~SigmaDelta();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
      void applyParams();
    };

    bgs_register(SigmaDelta);
  }
}
