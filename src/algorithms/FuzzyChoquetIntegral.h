#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 4

#include "../tools/FuzzyUtils.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class FuzzyChoquetIntegral : public IBGS
    {
    public:
      FuzzyChoquetIntegral();
      ~FuzzyChoquetIntegral();

      typedef bgslibrary::tools::FuzzyUtils FuzzyUtils;
      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      long frameNumber;
      int framesToLearn;
      double alphaLearn;
      double alphaUpdate;
      int colorSpace;
      int option;
      bool smooth;
      double threshold;
      FuzzyUtils fu;
      cv::Mat img_background_f3;
      
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(FuzzyChoquetIntegral);
  }
}

#endif
