#pragma once

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class StaticFrameDifference : public IBGS
    {
    private:
      bool enableThreshold;
      int threshold;

    public:
      StaticFrameDifference();
      ~StaticFrameDifference();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(StaticFrameDifference);
  }
}
