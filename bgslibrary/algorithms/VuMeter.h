#pragma once

#include "IBGS.h"
#include "VuMeter/TBackgroundVuMeter.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class VuMeter : public IBGS
    {
    private:
      vumeter::TBackgroundVuMeter bgs;
      IplImage *frame;
      IplImage *gray;
      IplImage *background;
      IplImage *mask;
      bool enableFilter;
      int binSize;
      double alpha;
      double threshold;

    public:
      VuMeter();
      ~VuMeter();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(VuMeter);
  }
}
