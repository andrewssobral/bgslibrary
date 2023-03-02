#pragma once

#include "BGModel.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lb
    {
      namespace BGModelFuzzySomParams {
        const int M = 3;      // width SOM (per pixel)
        const int N = 3;      // height SOM (per pixel)
        const int KERNEL = 3; // size Gaussian kernel
        const bool SPAN_NEIGHBORS = false; // true if update neighborhood spans different pixels			//
        const int TRAINING_STEPS = 100;    // number of training steps
        const double EPS1 = 100.0; // model match distance during training
        const double EPS2 = 20.0;  // model match distance
        const double C1 = 1.0;     // learning rate during training
        const double C2 = 0.05;    // learning rate
        const double FUZZYEXP = -5.0;
        const double FUZZYTHRESH = 0.8;
      }

      class BGModelFuzzySom : public BGModel
      {
      public:
        BGModelFuzzySom(int width, int height);
        ~BGModelFuzzySom();

        void setBGModelParameter(int id, int value);

      protected:
        int m_widthSOM;
        int m_heightSOM;
        int m_offset;
        int m_pad;
        int m_K;
        int m_TSteps;

        double m_Wmax;

        double m_epsilon1;
        double m_epsilon2;
        double m_alpha1;
        double m_alpha2;

        DBLRGB** m_ppSOM;  // SOM grid
        double** m_ppW;    // Weights

        void Init();
        void Update();
      };
    }
  }
}
