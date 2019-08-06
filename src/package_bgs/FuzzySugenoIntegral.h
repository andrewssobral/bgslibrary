#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "T2F/FuzzyUtils.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class FuzzySugenoIntegral : public IBGS, public ILoadSaveConfig
    {
    private:
      long long frameNumber;

      int framesToLearn;
      double alphaLearn;
      double alphaUpdate;
      int colorSpace;
      int option;
      bool smooth;
      double threshold;

      FuzzyUtils fu;
      cv::Mat img_background_f3;

    public:
      FuzzySugenoIntegral();
      ~FuzzySugenoIntegral();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<FuzzySugenoIntegral> register_FuzzySugenoIntegral("FuzzySugenoIntegral");
  }
}

#endif
