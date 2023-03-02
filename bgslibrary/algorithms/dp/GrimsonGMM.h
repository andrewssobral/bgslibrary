#pragma once

#include "Bgs.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
    typedef struct GMMGaussian
    {
      float variance;
      float muR;
      float muG;
      float muB;
      float weight;
      float significants;		// this is equal to weight / standard deviation and is used to
      // determine which Gaussians should be part of the background model
    } GMM;

    // --- User adjustable parameters used by the Grimson GMM BGS algorithm ---
    class GrimsonParams : public BgsParams
    {
    public:
      float &LowThreshold() { return m_low_threshold; }
      float &HighThreshold() { return m_high_threshold; }

      float &Alpha() { return m_alpha; }
      int &MaxModes() { return m_max_modes; }

    private:
      // Threshold on the squared dist. to decide when a sample is close to an existing
      // components. If it is not close to any a new component will be generated.
      // Smaller threshold values lead to more generated components and higher threshold values
      // lead to a small number of components but they can grow too large.
      //
      // It is usual easiest to think of these thresholds as being the number of variances away
      // from the mean of a pixel before it is considered to be from the foreground.
      float m_low_threshold;
      float m_high_threshold;

      // alpha - speed of update - if the time interval you want to average over is T
      // set alpha=1/T.
      float m_alpha;

      // Maximum number of modes (Gaussian components) that will be used per pixel
      int m_max_modes;
    };

    // --- Grimson GMM BGS algorithm ---
    class GrimsonGMM : public Bgs
    {
    public:
      GrimsonGMM();
      ~GrimsonGMM();

      void Initalize(const BgsParams& param);

      void InitModel(const RgbImage& data);
      void Subtract(int frame_num, const RgbImage& data,
        BwImage& low_threshold_mask, BwImage& high_threshold_mask);
      void Update(int frame_num, const RgbImage& data, const BwImage& update_mask);

      RgbImage* Background();

    private:
      void SubtractPixel(long posPixel, const RgbPixel& pixel, unsigned char& numModes,
        unsigned char& lowThreshold, unsigned char& highThreshold);

      // User adjustable parameters
      GrimsonParams m_params;

      // Threshold when the component becomes significant enough to be included into
      // the background model. It is the TB = 1-cf from the paper. So I use cf=0.1 => TB=0.9
      // For alpha=0.001 it means that the mode should exist for approximately 105 frames before
      // it is considered foreground
      float m_bg_threshold; //1-cf from the paper

      // Initial variance for the newly generated components.
      // It will will influence the speed of adaptation. A good guess should be made.
      // A simple way is to estimate the typical standard deviation from the images.
      float m_variance;

      // Dynamic array for the mixture of Gaussians
      GMM* m_modes;

      // Number of Gaussian components per pixel
      BwImage m_modes_per_pixel;

      // Current background model
      RgbImage m_background;
    };
    }
  }
}

#endif
