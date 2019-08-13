#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3

namespace bgslibrary
{
  namespace algorithms
  {
    class GMG : public IBGS
    {
    private:
      cv::Ptr<cv::BackgroundSubtractorGMG> fgbg;
      int initializationFrames;
      double decisionThreshold;
      cv::Mat img_segmentation;

    public:
      GMG();
      ~GMG();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(GMG);
  }
}

#endif
