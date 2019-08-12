#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "lb/BGModelFuzzySom.h"

using namespace lb_library;
using namespace lb_library::FuzzyAdaptiveSOM;

namespace bgslibrary
{
  namespace algorithms
  {
    class LBFuzzyAdaptiveSOM : public IBGS
    {
    private:
      BGModel* m_pBGModel;
      int sensitivity;
      int trainingSensitivity;
      int learningRate;
      int trainingLearningRate;
      int trainingSteps;

    public:
      LBFuzzyAdaptiveSOM();
      ~LBFuzzyAdaptiveSOM();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(LBFuzzyAdaptiveSOM);
  }
}

#endif
