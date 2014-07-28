/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MRF_H
#define MRF_H

#include "T2FMRF.h"

namespace Algorithms
{
  namespace BackgroundSubtraction
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
    class MRF_TC: public MRF
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

#endif
