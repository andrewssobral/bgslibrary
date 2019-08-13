#pragma once

#include "IBGS.h"

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "dp/TextureBGS.h"
//#include "ConnectedComponents.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class DPTexture : public IBGS
    {
    private:
      int width;
      int height;
      int size;
      TextureBGS bgs;
      IplImage* frame;
      RgbImage image;
      BwImage fgMask;
      BwImage tempMask;
      TextureArray* bgModel;
      RgbImage texture;
      unsigned char* modeArray;
      TextureHistogram* curTextureHist;
      //ConnectedComponents cc;
      //CBlobResult largeBlobs;
      //IplConvKernel* dilateElement;
      //IplConvKernel* erodeElement;
      //bool enableFiltering;

    public:
      DPTexture();
      ~DPTexture();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);
    };

    bgs_register(DPTexture);
  }
}

#endif
