#pragma once

#include "IBGS.h"
#include "ViBeBGS/VibeBGS.hpp"

namespace bgslibrary
{
  namespace algorithms
  {
    class VibeBGS 
        : public IBGS
    {
    private:
      static const int DEFAULT_NUM_SAMPLES = 20;
      static const int DEFAULT_MATCH_THRESH = 20;
      static const int DEFAULT_MATCH_NUM = 2;
      static const int DEFAULT_UPDATE_FACTOR = 16;

    private:
      //int numberOfSamples;
      int matchingThreshold;
      int matchingNumber;
      int updateFactor;
      std::unique_ptr<sky360::VibeBGS> model;

    public:
      VibeBGS();
      ~VibeBGS();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(VibeBGS);
  }
}
