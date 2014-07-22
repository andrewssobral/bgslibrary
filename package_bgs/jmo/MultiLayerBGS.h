/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>


#include "../IBGS.h"
#include "CMultiLayerBGS.h"

class MultiLayerBGS : public IBGS
{
public:
  enum Status
  {
    MLBGS_NONE = -1,
    MLBGS_LEARN = 0,
    MLBGS_DETECT = 1
  };

private:
  bool firstTime;
  long long frameNumber;
  cv::Mat img_foreground;
  cv::Mat img_merged;
  cv::Mat img_background;
  bool showOutput;
  bool saveModel;
  bool disableDetectMode;
  bool disableLearning;
  int detectAfter;
  CMultiLayerBGS* BGS;
  Status status;
  IplImage* img;
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
  MultiLayerBGS();
  ~MultiLayerBGS();

  void setStatus(Status status);
  void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
  void finish(void);
  void saveConfig();
  void loadConfig();
};