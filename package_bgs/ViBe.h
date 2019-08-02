#pragma once

#include "IBGS.h"
#include "ILoadSaveConfig.h"
#include "ViBe/vibe-background-sequential.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class ViBe : public IBGS, public ILoadSaveConfig
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
      vibeModel_Sequential_t* model;

    public:
      ViBe();
      ~ViBe();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<ViBe> register_ViBe("ViBe");
  }
}
