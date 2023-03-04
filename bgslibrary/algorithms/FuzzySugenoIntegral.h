#pragma once

#include "IBGS.h"
#include "../tools/FuzzyUtils.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class FuzzySugenoIntegral : public IBGS
    {
    public:
      FuzzySugenoIntegral();
      ~FuzzySugenoIntegral();

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
      cv::Mat img_background_f3;
      FuzzyUtils fu;

      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(FuzzySugenoIntegral);
  }
}
