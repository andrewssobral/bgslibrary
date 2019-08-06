#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class WeightedMovingVariance : public IBGS, public ILoadSaveConfig
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

    static BGS_Register<WeightedMovingVariance> register_WeightedMovingVariance("WeightedMovingVariance");
  }
}
