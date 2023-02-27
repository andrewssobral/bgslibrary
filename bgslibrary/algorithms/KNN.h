#pragma once

#include <iostream>

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 3

#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

namespace bgslibrary
{
  namespace algorithms
  {
    class KNN : public IBGS
    {
    private:
      cv::Ptr<cv::BackgroundSubtractorKNN> knn;
      int history;
      float dist2Threshold;
      float shadowThreshold;

    public:
      KNN();
      ~KNN();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(KNN);
  }
}

#endif
