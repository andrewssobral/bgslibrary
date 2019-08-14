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
      lbsp::BackgroundSubtractorPAWCS* pPAWCS;

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
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(PAWCS);
  }
}
