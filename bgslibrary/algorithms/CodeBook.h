#pragma once

#include <opencv2/opencv.hpp>

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace codebook {
      struct codeword {
        float min;
        float max;
        float f;
        float l;
        int first;
        int last;
        bool isStale;
      };
    }

    class CodeBook : public IBGS
    {
    public:
      typedef codebook::codeword codeword;

      CodeBook();
      ~CodeBook();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

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

      void initializeCodebook(int w, int h);
      void update_cb(const cv::Mat& frame);
      void fg_cb(const cv::Mat& frame, cv::Mat& fg);
      
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(CodeBook);
  }
}
