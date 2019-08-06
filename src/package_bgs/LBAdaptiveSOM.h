#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "lb/BGModelSom.h"

using namespace lb_library;
using namespace lb_library::AdaptiveSOM;

namespace bgslibrary
{
  namespace algorithms
  {
    class LBAdaptiveSOM : public IBGS, public ILoadSaveConfig
    {
    private:
      BGModel* m_pBGModel;
      int sensitivity;
      int trainingSensitivity;
      int learningRate;
      int trainingLearningRate;
      int trainingSteps;

    public:
      LBAdaptiveSOM();
      ~LBAdaptiveSOM();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<LBAdaptiveSOM> register_LBAdaptiveSOM("LBAdaptiveSOM");
  }
}

#endif
