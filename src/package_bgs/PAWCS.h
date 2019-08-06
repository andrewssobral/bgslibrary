#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"
#include "LBSP/BackgroundSubtractorPAWCS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class PAWCS : public IBGS, public ILoadSaveConfig
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

    static BGS_Register<PAWCS> register_PAWCS("PAWCS");
  }
}
