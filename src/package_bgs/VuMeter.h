#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "IBGS.h"
#include "ILoadSaveConfig.h"
#include "VuMeter/TBackgroundVuMeter.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class VuMeter : public IBGS, public ILoadSaveConfig
    {
    private:
      TBackgroundVuMeter bgs;

      IplImage *frame;
      IplImage *gray;
      IplImage *background;
      IplImage *mask;

      bool enableFilter;
      int binSize;
      double alpha;
      double threshold;

    public:
      VuMeter();
      ~VuMeter();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<VuMeter> register_VuMeter("VuMeter");
  }
}

#endif
