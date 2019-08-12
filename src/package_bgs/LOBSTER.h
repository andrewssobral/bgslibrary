#pragma once

#include "IBGS.h"

#include "LBSP/BackgroundSubtractorLOBSTER.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class LOBSTER : public IBGS
    {
    private:
      BackgroundSubtractorLOBSTER* pLOBSTER;

      float fRelLBSPThreshold;
      int nLBSPThresholdOffset;
      int nDescDistThreshold;
      int nColorDistThreshold;
      int nBGSamples;
      int nRequiredBGSamples;

    public:
      LOBSTER();
      ~LOBSTER();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(LOBSTER);
  }
}
