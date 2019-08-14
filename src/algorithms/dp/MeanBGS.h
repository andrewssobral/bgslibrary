#pragma once

#include "Bgs.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
    // --- Parameters used by the Mean BGS algorithm ---
    class MeanParams : public BgsParams
    {
    public:
      unsigned int &LowThreshold() { return m_low_threshold; }
      unsigned int &HighThreshold() { return m_high_threshold; }

      float &Alpha() { return m_alpha; }
      int &LearningFrames() { return m_learning_frames; }

    private:
      // A pixel is considered to be from the background if the squared distance between
      // it and the background model is less than the threshold.
      unsigned int m_low_threshold;
      unsigned int m_high_threshold;

      float m_alpha;
      int m_learning_frames;
    };


    // --- Mean BGS algorithm ---
    class MeanBGS : public Bgs
    {
    public:
      virtual ~MeanBGS() {}

      void Initalize(const BgsParams& param);

      void InitModel(const RgbImage& data);
      void Subtract(int frame_num, const RgbImage& data,
        BwImage& low_threshold_mask, BwImage& high_threshold_mask);
      void Update(int frame_num, const RgbImage& data, const BwImage& update_mask);

      RgbImage* Background() { return &m_background; }

    private:
      void SubtractPixel(int r, int c, const RgbPixel& pixel,
        unsigned char& lowThreshold, unsigned char& highThreshold);

      MeanParams m_params;

      RgbImageFloat m_mean;
      RgbImage m_background;
    };
    }
  }
}

#endif
