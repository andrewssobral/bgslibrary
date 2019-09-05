#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 4

#include "IMBS/IMBS.hpp"

namespace bgslibrary
{
  namespace algorithms
  {
    class IndependentMultimodal : public IBGS
    {
    private:
      imbs::BackgroundSubtractorIMBS* pIMBS;
      int fps;

    public:
      IndependentMultimodal();
      ~IndependentMultimodal();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(IndependentMultimodal);
  }
}

#endif
