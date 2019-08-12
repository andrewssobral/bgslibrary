#pragma once

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class AdaptiveSelectiveBackgroundLearning : public IBGS
    {
    private:
      double alphaLearn;
      double alphaDetection;
      int learningFrames;
      long counter;
      double minVal;
      double maxVal;
      int threshold;

    public:
      AdaptiveSelectiveBackgroundLearning();
      ~AdaptiveSelectiveBackgroundLearning();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(AdaptiveSelectiveBackgroundLearning);
  }
}
