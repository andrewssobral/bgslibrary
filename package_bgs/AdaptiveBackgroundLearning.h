#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class AdaptiveBackgroundLearning : 
      public IBGS, public ILoadSaveConfig
    {
    private:
      double alpha;
      long limit;
      long counter;
      double minVal;
      double maxVal;
      bool enableThreshold;
      int threshold;

    public:
      AdaptiveBackgroundLearning();
      ~AdaptiveBackgroundLearning();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<AdaptiveBackgroundLearning> 
      register_AdaptiveBackgroundLearning("AdaptiveBackgroundLearning");
  }
}
