#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class MixtureOfGaussianV2 : public IBGS
    {
    private:
#if CV_MAJOR_VERSION == 2
      cv::BackgroundSubtractorMOG2 mog;
#elif CV_MAJOR_VERSION >= 3
      cv::Ptr<cv::BackgroundSubtractorMOG2> mog;
#endif
      double alpha;
      bool enableThreshold;
      int threshold;

    public:
      MixtureOfGaussianV2();
      ~MixtureOfGaussianV2();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(MixtureOfGaussianV2);
  }
}
