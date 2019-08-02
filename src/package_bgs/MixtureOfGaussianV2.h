#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

#include "IBGS.h"
#include "ILoadSaveConfig.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class MixtureOfGaussianV2 : public IBGS, public ILoadSaveConfig
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
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<MixtureOfGaussianV2> register_MixtureOfGaussianV2("MixtureOfGaussianV2");
  }
}
