#pragma once

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class AdaptiveBackgroundLearning : public IBGS
    {
    private:
      double alpha;
      int limit;
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
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(AdaptiveBackgroundLearning);
  }
}
