#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class FrameDifference : public IBGS, public ILoadSaveConfig
    {
    private:
      bool enableThreshold;
      int threshold;

    public:
      FrameDifference();
      ~FrameDifference();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(FrameDifference);
  }
}
