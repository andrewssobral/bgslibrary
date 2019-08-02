#pragma once

#include <opencv2/opencv.hpp>

#include "../IBGS.h"
#include "../ILoadSaveConfig.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class MyBGS : public IBGS, public ILoadSaveConfig
    {
    private:
      cv::Mat img_previous;

    public:
      MyBGS();
      ~MyBGS();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig() {}
      void loadConfig() {}
    };

    static BGS_Register<MyBGS> register_MyBGS("MyBGS");
  }
}
