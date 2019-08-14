#pragma once

#include "BGModel.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lb
    {
      namespace BGModelFuzzyGaussParams {
        const float ALPHAFUZZYGAUSS = 0.02f;
        const float THRESHOLDFUZZYGAUSS = 3.5f;
        const float THRESHOLDBG = 0.5f;
        const float NOISEFUZZYGAUSS = 50.0f;
        const float FUZZYEXP = -5.0f;
      }

      class BGModelFuzzyGauss : public BGModel
      {
      public:
        BGModelFuzzyGauss(int width, int height);
        ~BGModelFuzzyGauss();

        void setBGModelParameter(int id, int value);

      protected:
        double m_alphamax;
        double m_threshold;
        double m_threshBG;
        double m_noise;

        DBLRGB* m_pMu;
        DBLRGB* m_pVar;

        void Init();
        void Update();
      };
    }
  }
}
