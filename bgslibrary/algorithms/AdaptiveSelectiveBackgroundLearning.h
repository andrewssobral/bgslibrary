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
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(AdaptiveSelectiveBackgroundLearning);
  }
}
