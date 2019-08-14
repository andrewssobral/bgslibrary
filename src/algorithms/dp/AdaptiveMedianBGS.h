#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include <iostream>
#include <stdlib.h>
#include <cmath>

#include "Bgs.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
    // --- Parameters used by the Adaptive Median BGS algorithm ---
    class AdaptiveMedianParams : public BgsParams
    {
    public:
      unsigned char &LowThreshold() { return m_low_threshold; }
      unsigned char &HighThreshold() { return m_high_threshold; }

      int &SamplingRate() { return m_samplingRate; }
      int &LearningFrames() { return m_learning_frames; }

    private:
      unsigned char m_low_threshold;
      unsigned char m_high_threshold;

      int m_samplingRate;
      int m_learning_frames;
    };


    // --- Adaptive Median BGS algorithm ---
    class AdaptiveMedianBGS : public Bgs
    {
    public:
      virtual ~AdaptiveMedianBGS() {}

      void Initalize(const BgsParams& param);

      void InitModel(const RgbImage& data);
      void Subtract(int frame_num, const RgbImage& data,
        BwImage& low_threshold_mask, BwImage& high_threshold_mask);
      void Update(int frame_num, const RgbImage& data, const BwImage& update_mask);

      RgbImage* Background();

    private:
      void SubtractPixel(int r, int c, const RgbPixel& pixel,
        unsigned char& low_threshold, unsigned char& high_threshold);

      AdaptiveMedianParams m_params;

      RgbImage m_median;
    };
    }
  }
}

#endif
