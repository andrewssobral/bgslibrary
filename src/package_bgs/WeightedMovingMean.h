#pragma once

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class WeightedMovingMean : public IBGS
    {
    private:
      cv::Mat img_input_prev_1;
      cv::Mat img_input_prev_2;
      bool enableWeight;
      bool enableThreshold;
      int threshold;

    public:
      WeightedMovingMean();
      ~WeightedMovingMean();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(WeightedMovingMean);
  }
}
