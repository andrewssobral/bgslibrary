#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

#include "IBGS.h"
#include "ILoadSaveConfig.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class MixtureOfGaussianV1 : public IBGS, public ILoadSaveConfig
    {
    private:
      cv::BackgroundSubtractorMOG mog;
      double alpha;
      bool enableThreshold;
      int threshold;

    public:
      MixtureOfGaussianV1();
      ~MixtureOfGaussianV1();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<MixtureOfGaussianV1> register_MixtureOfGaussianV1("MixtureOfGaussianV1");
  }
}

#endif
