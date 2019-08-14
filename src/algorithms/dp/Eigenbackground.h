#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "Bgs.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
    // --- Parameters used by the Mean BGS algorithm ---
    class EigenbackgroundParams : public BgsParams
    {
    public:
      float &LowThreshold() { return m_low_threshold; }
      float &HighThreshold() { return m_high_threshold; }

      int &HistorySize() { return m_history_size; }
      int &EmbeddedDim() { return m_dim; }

    private:
      // A pixel will be classified as foreground if the squared distance of any
      // color channel is greater than the specified threshold
      float m_low_threshold;
      float m_high_threshold;

      int m_history_size;			// number frames used to create eigenspace
      int m_dim;							// eigenspace dimensionality
    };

    // --- Eigenbackground BGS algorithm ---
    class Eigenbackground : public Bgs
    {
    public:
      Eigenbackground();
      ~Eigenbackground();

      void Initalize(const BgsParams& param);

      void InitModel(const RgbImage& data);
      void Subtract(int frame_num, const RgbImage& data,
        BwImage& low_threshold_mask, BwImage& high_threshold_mask);
      void Update(int frame_num, const RgbImage& data, const BwImage& update_mask);

      RgbImage* Background() { return &m_background; }

    private:
      void UpdateHistory(int frameNum, const RgbImage& newFrame);

      EigenbackgroundParams m_params;

      CvMat* m_pcaData;
      CvMat* m_pcaAvg;
      CvMat* m_eigenValues;
      CvMat* m_eigenVectors;

      RgbImage m_background;
    };
    }
  }
}

#endif
