#pragma once

#include <iostream>

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 3

#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

namespace bgslibrary
{
  namespace algorithms
  {
    class KNN : public IBGS, public ILoadSaveConfig
    {
    private:
      cv::Ptr<cv::BackgroundSubtractorKNN> knn;
      int history;
      int nSamples;
      float dist2Threshold;
      int knnSamples;
      bool doShadowDetection;
      int shadowValue;
      float shadowThreshold;

    public:
      KNN();
      ~KNN();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(KNN);
  }
}

#endif
