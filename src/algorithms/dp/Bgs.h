#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "Image.h"
#include "BgsParams.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
    class Bgs
    {
    public:
      static const int BACKGROUND = 0;
      static const int FOREGROUND = 255;

      virtual ~Bgs() {}

      // Initialize any data required by the BGS algorithm. Should be called once before calling
      // any of the following functions.
      virtual void Initalize(const BgsParams& param) = 0;

      // Initialize the background model. Typically, the background model is initialized using the first
      // frame of the incoming video stream, but alternatives are possible.
      virtual void InitModel(const RgbImage& data) = 0;

      // Subtract the current frame from the background model and produce a binary foreground mask using
      // both a low and high threshold value.
      virtual void Subtract(int frame_num, const RgbImage& data,
                            BwImage& low_threshold_mask, BwImage& high_threshold_mask) = 0;

      // Update the background model. Only pixels set to background in update_mask are updated.
      virtual void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask) = 0;

      // Return the current background model.
      virtual RgbImage *Background() = 0;
    };
    }
  }
}

#endif
