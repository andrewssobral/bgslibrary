#pragma once

#include "BGModel.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lb
    {
      namespace BGModelGaussParams {
        const double THRESHGAUSS = 2.5;   // Threshold
        const double ALPHAGAUSS = 0.0001; // Learning rate
        const double NOISEGAUSS = 50.0;   // Minimum variance (noise)
      }
      
      class BGModelGauss : public BGModel
      {
      public:
        BGModelGauss(int width, int height);
        ~BGModelGauss();

        void setBGModelParameter(int id, int value);

      protected:
        double m_alpha;
        double m_threshold;
        double m_noise;

        DBLRGB* m_pMu;
        DBLRGB* m_pVar;

        void Init();
        void Update();
      };
    }
  }
}
