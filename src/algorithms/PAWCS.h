#pragma once

#include "IBGS.h"
#include "LBSP/BackgroundSubtractorPAWCS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class PAWCS : public IBGS
    {
    private:
      BackgroundSubtractorPAWCS* pPAWCS;

      float fRelLBSPThreshold;
      int nDescDistThresholdOffset;
      int nMinColorDistThreshold;
      int nMaxNbWords;
      int nSamplesForMovingAvgs;

    public:
      PAWCS();
      ~PAWCS();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(PAWCS);
  }
}
