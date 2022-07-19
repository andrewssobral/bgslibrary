#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3

#include "IBGS.h"
#include "T2F/MRF.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class T2FMRF_UV : public IBGS
    {
    private:
      long frameNumber;
      double threshold;
      double alpha;
      float km;
      float kv;
      int gaussians;
      IplImage *old_labeling;
      IplImage *old;
      dp::RgbImage frame_data;
      dp::T2FMRFParams params;
      dp::T2FMRF bgs;
      dp::BwImage lowThresholdMask;
      dp::BwImage highThresholdMask;
      dp::MRF_TC mrf;
      dp::GMM *gmm;
      dp::HMM *hmm;

    public:
      T2FMRF_UV();
      ~T2FMRF_UV();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(T2FMRF_UV);
  }
}

#endif
