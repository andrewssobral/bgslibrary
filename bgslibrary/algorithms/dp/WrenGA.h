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
    class WrenParams : public BgsParams
    {
    public:
      float &LowThreshold() { return m_low_threshold; }
      float &HighThreshold() { return m_high_threshold; }

      float &Alpha() { return m_alpha; }
      int &LearningFrames() { return m_learning_frames; }

    private:
      // The threshold indicates the number of variances (not standard deviations) away
      // from the mean before a pixel is considered to be from the foreground.
      float m_low_threshold;
      float m_high_threshold;

      float m_alpha;
      int m_learning_frames;
    };

    // --- Mean BGS algorithm ---
    class WrenGA : public Bgs
    {
    private:
      struct GAUSSIAN
      {
        float mu[NUM_CHANNELS];
        float var[NUM_CHANNELS];
      };

    public:
      WrenGA();
      ~WrenGA();

      void Initalize(const BgsParams& param);

      void InitModel(const RgbImage& data);
      void Subtract(int frame_num, const RgbImage& data,
        BwImage& low_threshold_mask, BwImage& high_threshold_mask);
      void Update(int frame_num, const RgbImage& data, const BwImage& update_mask);

      RgbImage* Background() { return &m_background; }

    private:
      void SubtractPixel(int r, int c, const RgbPixel& pixel,
        unsigned char& lowThreshold, unsigned char& highThreshold);

      WrenParams m_params;

      // Initial variance for the newly generated components.
      float m_variance;

      // dynamic array for the mixture of Gaussians
      GAUSSIAN* m_gaussian;

      RgbImage m_background;
    };
    }
  }
}

#endif
