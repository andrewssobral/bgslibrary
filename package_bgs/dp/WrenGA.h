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
* WrenGA.hpp
*
* Purpose: Implementation of the running Gaussian average background 
*		  		 subtraction algorithm described in:
*	  			 "Pfinder: real-time tracking of the human body"
* 						by C. Wren et al (1997)
*
* Author: Donovan Parks, September 2007
*
* Please note that this is not an implementation of Pfinder. It implements
* a simple background subtraction algorithm where each pixel is represented
* by a single Gaussian and update using a simple weighting function.

Example:
Algorithms::BackgroundSubtraction::WrenParams params;
params.SetFrameSize(width, height);
params.LowThreshold() = 3.5f*3.5f;
params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
params.Alpha() = 0.005f;
params.LearningFrames() = 30;

Algorithms::BackgroundSubtraction::WrenGA bgs;
bgs.Initalize(params);
******************************************************************************/

#ifndef WREN_GA_H
#define WREN_GA_H

#include "Bgs.h"

namespace Algorithms
{
  namespace BackgroundSubtraction
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
      void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

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

#endif






