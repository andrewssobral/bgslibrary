#pragma once

#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "../IBGS.h"

//extern "C" {
#include "sdLaMa091.h"
//}

class SigmaDeltaBGS : public IBGS {
private:

  bool firstTime;
  unsigned int ampFactor;
  unsigned int minVar;
  unsigned int maxVar;
  sdLaMa091_t* algorithm;
  bool showOutput;

public:

  SigmaDeltaBGS();

  ~SigmaDeltaBGS();

  void process(
    const cv::Mat &img_input,
    cv::Mat &img_output,
    cv::Mat &img_bgmodel
    );

private:

  void saveConfig();

  void loadConfig();

  void applyParams();
};
