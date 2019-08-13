#pragma once

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class FrameDifference : public IBGS
    {
    private:
      bool enableThreshold;
      int threshold;

    public:
      FrameDifference();
      ~FrameDifference();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(FrameDifference);
  }
}
