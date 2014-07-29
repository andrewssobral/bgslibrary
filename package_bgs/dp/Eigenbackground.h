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
/****************************************************************************
*
* Eigenbackground.hpp
*
* Purpose: Implementation of the Eigenbackground background subtraction 
*					 algorithm developed by Oliver et al.
*
* Author: Donovan Parks, September 2007
*
* "A Bayesian Computer Vision System for Modeling Human Interactions"
*   Nuria Oliver, Barbara Rosario, Alex P. Pentland 2000

Example:
Algorithms::BackgroundSubtraction::EigenbackgroundParams params;
params.SetFrameSize(width, height);
params.LowThreshold() = 15*15;
params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
params.HistorySize() = 100;
params.EmbeddedDim() = 20;

Algorithms::BackgroundSubtraction::Eigenbackground bgs;
bgs.Initalize(params);
******************************************************************************/

#ifndef _ELGAMMAL_H_
#define _ELGAMMAL_H_

#include "Bgs.h"

namespace Algorithms
{
  namespace BackgroundSubtraction
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
      void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

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

#endif
