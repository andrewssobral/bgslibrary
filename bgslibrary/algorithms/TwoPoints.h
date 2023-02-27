#pragma once

#include "IBGS.h"
#include "TwoPoints/two_points.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class TwoPoints : public IBGS
    {
    private:
      static const int DEFAULT_MATCH_THRESH = 20;
      static const int DEFAULT_UPDATE_FACTOR = 16;
      int matchingThreshold;
      int updateFactor;
      twopoints::twopointsModel_t* model;

    public:
      TwoPoints();
      ~TwoPoints();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(TwoPoints);
  }
}
