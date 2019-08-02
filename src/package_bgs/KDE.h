#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "KDE/NPBGSubtractor.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class KDE : public IBGS, public ILoadSaveConfig
    {
    private:
      NPBGSubtractor *p;
      int rows;
      int cols;
      int color_channels;
      int SequenceLength;
      int TimeWindowSize;
      int SDEstimationFlag;
      int lUseColorRatiosFlag;
      double th;
      double alpha;
      int framesToLearn;
      int frameNumber;

      unsigned char *FGImage;
      unsigned char *FilteredFGImage;
      unsigned char **DisplayBuffers;

    public:
      KDE();
      ~KDE();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<KDE> register_KDE("KDE");
  }
}

#endif
