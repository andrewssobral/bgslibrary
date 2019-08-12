#pragma once

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class WeightedMovingVariance : public IBGS
    {
    private:
      cv::Mat img_input_prev_1;
      cv::Mat img_input_prev_2;
      bool enableWeight;
      bool enableThreshold;
      int threshold;

    public:
      WeightedMovingVariance();
      ~WeightedMovingVariance();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);
      cv::Mat computeWeightedVariance(const cv::Mat &img_input_f, const cv::Mat &img_mean_f, const double weight);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(WeightedMovingVariance);
  }
}
