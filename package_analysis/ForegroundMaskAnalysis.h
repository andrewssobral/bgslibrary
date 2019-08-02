#pragma once

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#include "../package_bgs/ILoadSaveConfig.h"

namespace bgslibrary
{
  class ForegroundMaskAnalysis: public ILoadSaveConfig
  {
  private:
    bool firstTime;
    bool showOutput;

  public:
    ForegroundMaskAnalysis();
    ~ForegroundMaskAnalysis();

    int stopAt;
    std::string img_ref_path;

    void process(const long &frameNumber, const std::string &name, const cv::Mat &img_input);

  private:
    void saveConfig();
    void loadConfig();
  };
}
