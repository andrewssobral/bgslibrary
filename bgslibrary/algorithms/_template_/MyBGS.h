#pragma once

#include <opencv2/opencv.hpp>

#include "../IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class MyBGS : public IBGS
    {
    private:
      cv::Mat img_previous;

    public:
      MyBGS();
      ~MyBGS();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(MyBGS);
  }
}
