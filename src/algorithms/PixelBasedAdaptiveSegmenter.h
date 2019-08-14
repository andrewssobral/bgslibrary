#pragma once

#include "IBGS.h"
#include "PBAS/PBAS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class PixelBasedAdaptiveSegmenter : public IBGS
    {
    private:
      pbas::PBAS pbas;

      bool enableInputBlur;
      bool enableOutputBlur;

      float alpha;
      float beta;
      int N;
      int Raute_min;
      float R_incdec;
      int R_lower;
      int R_scale;
      float T_dec;
      int T_inc;
      int T_init;
      int T_lower;
      int T_upper;

    public:
      PixelBasedAdaptiveSegmenter();
      ~PixelBasedAdaptiveSegmenter();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(PixelBasedAdaptiveSegmenter);
  }
}
