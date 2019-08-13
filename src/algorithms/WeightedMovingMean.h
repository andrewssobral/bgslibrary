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
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(WeightedMovingMean);
  }
}
