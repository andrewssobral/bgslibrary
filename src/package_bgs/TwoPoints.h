#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"
#include "TwoPoints/two_points.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class TwoPoints : public IBGS, public ILoadSaveConfig
    {
    private:
      static const int DEFAULT_MATCH_THRESH = 20;
      static const int DEFAULT_UPDATE_FACTOR = 16;
      int matchingThreshold;
      int updateFactor;
      twopointsModel_t* model;

    public:
      TwoPoints();
      ~TwoPoints();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    bgs_register(TwoPoints);
  }
}
