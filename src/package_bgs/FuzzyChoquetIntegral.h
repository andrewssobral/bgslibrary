#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "T2F/FuzzyUtils.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class FuzzyChoquetIntegral : public IBGS
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
      FuzzyChoquetIntegral();
      ~FuzzyChoquetIntegral();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(FuzzyChoquetIntegral);
  }
}

#endif
