#pragma once

#include <opencv2/opencv.hpp>

#include "IBGS.h"
#include "ILoadSaveConfig.h"

namespace bgslibrary
{
  namespace algorithms
  {
    struct codeword {
      float min;
      float max;
      float f;
      float l;
      int first;
      int last;
      bool isStale;
    };

    class CodeBook : public IBGS, public ILoadSaveConfig
    {
    private:
      static const int Tdel = 200;
      static const int Tadd = 150;
      static const int Th = 200;
      const int DEFAULT_ALPHA = 10;
      const float DEFAULT_BETA = 1.;
      const int DEFAULT_LEARNFRAMES = 10;
      int t = 0;
      int learningFrames = 10;
      int alpha = 10;
      float beta = 1;
      std::vector<codeword> **cbMain;
      std::vector<codeword> **cbCache;

    public:
      CodeBook();
      ~CodeBook();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void initializeCodebook(int w, int h);
      void update_cb(const cv::Mat& frame);
      void fg_cb(const cv::Mat& frame, cv::Mat& fg);

      void saveConfig();
      void loadConfig();
    };

    bgs_register(CodeBook);
  }
}
