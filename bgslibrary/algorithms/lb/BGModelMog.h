#pragma once

#include "BGModel.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lb
    {
      namespace BGModelMogParams {
        const unsigned int NUMBERGAUSSIANS = 3;
        const float LEARNINGRATEMOG = 0.001f;
        const float THRESHOLDMOG = 2.5f;
        const float BGTHRESHOLDMOG = 0.5f;
        const float INITIALVARMOG = 50.0f;
      }

      typedef struct tagMOGDATA
      {
        DBLRGB mu;
        DBLRGB var;
        double w;
        double sortKey;
      } MOGDATA;

      class BGModelMog : public BGModel
      {
      public:
        BGModelMog(int width, int height);
        ~BGModelMog();

        void setBGModelParameter(int id, int value);

      protected:
        double m_alpha;
        double m_threshold;
        double m_noise;
        double m_T;

        MOGDATA* m_pMOG;
        int* m_pK;				// number of distributions per pixel

        void Init();
        void Update();
      };
    }
  }
}
