#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "IMBS/IMBS.hpp"

namespace bgslibrary
{
  namespace algorithms
  {
    class IndependentMultimodal : public IBGS, public ILoadSaveConfig
    {
    private:
      BackgroundSubtractorIMBS* pIMBS;
      int fps;

    public:
      IndependentMultimodal();
      ~IndependentMultimodal();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<IndependentMultimodal> register_IndependentMultimodal("IndependentMultimodal");
  }
}

#endif
