#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
// opencv legacy includes
#include <opencv2/imgproc/types_c.h>
//#include <opencv2/imgproc/imgproc_c.h>
//#include <opencv2/highgui/highgui_c.h>

#include "utils/ILoadSaveConfig.h"

namespace bgslibrary
{
  class PreProcessor : public ILoadSaveConfig
  {
  private:
    bool firstTime;
    bool equalizeHist;
    bool gaussianBlur;
    cv::Mat img_gray;
    bool enableShow;

  public:
    PreProcessor();
    ~PreProcessor();

    void setEqualizeHist(bool value);
    void setGaussianBlur(bool value);
    cv::Mat getGrayScale();

    void process(const cv::Mat &img_input, cv::Mat &img_output);

    //void rotate(const cv::Mat &img_input, cv::Mat &img_output, float angle);
    void applyCanny(const cv::Mat &img_input, cv::Mat &img_output);

  private:
    void save_config(cv::FileStorage &fs);
    void load_config(cv::FileStorage &fs);
  };
}
