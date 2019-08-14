#pragma once

#include "IBGS.h"
#include "LBSP/BackgroundSubtractorSuBSENSE.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class SuBSENSE : public IBGS
    {
    private:
      lbsp::BackgroundSubtractorSuBSENSE* pSubsense;

      float fRelLBSPThreshold;
      int nDescDistThresholdOffset;
      int nMinColorDistThreshold;
      int nBGSamples;
      int nRequiredBGSamples;
      int nSamplesForMovingAvgs;

    public:
      SuBSENSE();
      ~SuBSENSE();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(SuBSENSE);
  }
}
