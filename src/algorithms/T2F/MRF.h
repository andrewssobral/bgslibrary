#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#include "T2FMRF.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
      // base class
      class MRF
      {
      public:
        IplImage *in_image, *out_image;
        //image's width and height
        int width, height;

      public:
        MRF();

      protected:

        //////////////////////////////////////////////////////////////////////////
        //the number of labeling
        int no_regions;
        //potential of Space  Constraint
        double beta;
        //terminal condition when (deltaE < t)
        double t;

        //////////////////////////////////////////////////////////////////////////
        //for gibbs
        double T0;
        //current temperature
        double T;
        double c;

        //////////////////////////////////////////////////////////////////////////
        // alpha value for MMD
        double alpha;

        //////////////////////////////////////////////////////////////////////////
        //current global energy
        double E;
        //old global energy
        double E_old;
        //number of iteration
        int K;

        //////////////////////////////////////////////////////////////////////////
        //labeling image
        int **classes;
        //input image
        int **in_image_data;
        //evidence
        float ** local_evidence;
      };

      /************************************************************************/
      /* the Markov Random Field with time constraints for T2FGMM   */
      /************************************************************************/
      class MRF_TC : public MRF
      {
      private:
        double beta_time;

      public:
        IplImage *background2;
        RgbImage background;
        int **old_labeling;

      public:
        MRF_TC();
        ~MRF_TC();
        double TimeEnergy2(int i, int j, int label);
        void OnIterationOver2(void);
        void Build_Classes_OldLabeling_InImage_LocalEnergy();
        void InitEvidence2(GMM *gmm, HMM *hmm, IplImage *labeling);
        void CreateOutput2();
        double CalculateEnergy2();
        double LocalEnergy2(int i, int j, int label);
        double Doubleton2(int i, int j, int label);

        void Gibbs2();
        void ICM2();
        void Metropolis2(bool mmd);
      };
    }
  }
}

#endif
