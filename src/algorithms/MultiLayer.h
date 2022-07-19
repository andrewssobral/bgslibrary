#pragma once

#include "opencv2/core/version.hpp"
#if (CV_MAJOR_VERSION == 2) || (CV_MAJOR_VERSION == 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7)

#include "IBGS.h"
#include "MultiLayer/CMultiLayerBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class MultiLayer : public IBGS
    {
    public:
      enum Status
      {
        MLBGS_NONE = -1,
        MLBGS_LEARN = 0,
        MLBGS_DETECT = 1
      };

    private:
      long frameNumber;
      cv::Mat img_merged;
      bool saveModel;
      bool disableDetectMode;
      bool disableLearning;
      int detectAfter;
      multilayer::CMultiLayerBGS* BGS;
      Status status;
      //IplImage* img;
      IplImage* org_img;
      IplImage* fg_img;
      IplImage* bg_img;
      IplImage* fg_prob_img;
      IplImage* fg_mask_img;
      IplImage* fg_prob_img3;
      IplImage* merged_img;
      std::string bg_model_preload;

      bool loadDefaultParams;

      int max_mode_num;
      float weight_updating_constant;
      float texture_weight;
      float bg_mode_percent;
      int pattern_neig_half_size;
      float pattern_neig_gaus_sigma;
      float bg_prob_threshold;
      float bg_prob_updating_threshold;
      int robust_LBP_constant;
      float min_noised_angle;
      float shadow_rate;
      float highlight_rate;
      float bilater_filter_sigma_s;
      float bilater_filter_sigma_r;

      float frame_duration;

      float mode_learn_rate_per_second;
      float weight_learn_rate_per_second;
      float init_mode_weight;

      float learn_mode_learn_rate_per_second;
      float learn_weight_learn_rate_per_second;
      float learn_init_mode_weight;

      float detect_mode_learn_rate_per_second;
      float detect_weight_learn_rate_per_second;
      float detect_init_mode_weight;

    public:
      MultiLayer();
      ~MultiLayer();

      void setStatus(Status status);
      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void finish();
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(MultiLayer);
  }
}

#endif
