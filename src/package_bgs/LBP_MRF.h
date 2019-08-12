#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "LBP_MRF/MotionDetection.hpp"

namespace bgslibrary
{
  namespace algorithms
  {
    class LBP_MRF : public IBGS, public ILoadSaveConfig
    {
    private:
      MotionDetection* Detector;
      cv::Mat img_segmentation;

    public:
      LBP_MRF();
      ~LBP_MRF();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(LBP_MRF);
  }
}

#endif
