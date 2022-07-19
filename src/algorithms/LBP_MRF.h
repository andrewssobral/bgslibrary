#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if (CV_MAJOR_VERSION == 2) || (CV_MAJOR_VERSION == 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7)

#include "LBP_MRF/MotionDetection.hpp"

namespace bgslibrary
{
  namespace algorithms
  {
    class LBP_MRF : public IBGS
    {
    private:
      lbp_mrf::MotionDetection* Detector;
      cv::Mat img_segmentation;

    public:
      LBP_MRF();
      ~LBP_MRF();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(LBP_MRF);
  }
}

#endif
