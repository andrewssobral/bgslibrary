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
* PratiMediodBGS.hpp
*
* Purpose: Implementation of the temporal median background 
*		  		 subtraction algorithm described in:
*
* [1] "Detecting Moving Objects, Shosts, and Shadows in Video Stream"
* 			by R. Cucchiara et al (2003)
*
* [2] "Reliable Background Suppression for Complex Scenes"
*				by S. Calderara et al (2006)
*
* Author: Donovan Parks, September 2007
*
* Please note that this is not an implementation of the complete system 
* given in the above papers. It simply implements the temporal media background
* subtraction algorithm.

Example:
Algorithms::BackgroundSubtraction::PratiParams params;
params.SetFrameSize(width, height);
params.LowThreshold() = 30;
params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
params.SamplingRate() = 5;
params.HistorySize() = 16;
params.Weight() = 5;

Algorithms::BackgroundSubtraction::PratiMediodBGS bgs;
bgs.Initalize(params);
******************************************************************************/

#ifndef PRATI_MEDIA_BGS_H
#define PRATI_MEDIA_BGS_H

#include <vector>
#include "Bgs.h"

namespace Algorithms
{
  namespace BackgroundSubtraction
  {
    // --- Parameters used by the Prati Mediod BGS algorithm ---
    class PratiParams : public BgsParams
    {
    public:
      unsigned int &LowThreshold() { return m_low_threshold; }
      unsigned int &HighThreshold() { return m_high_threshold; }

      int &Weight() { return m_weight; }
      int &SamplingRate() { return m_sampling_rate; }
      int &HistorySize() { return m_history_size; }

    private:
      // The low threshold is used to supress noise. The high thresohld is used 
      // to find pixels highly likely to be foreground. This implementation uses an L-inf 
      // distance measure and a pixel p is considered F/G if D(I(p), B(p)) > threshold. 
      // The two threshold maps are combined as in [2].
      unsigned int m_low_threshold;
      unsigned int m_high_threshold;

      // The weight parameter controls the amount of influence given to previous background samples
      // see w_b in equation (2) of [1]
      // in [2] this value is set to 1
      int m_weight;

      // Number of samples to consider when calculating temporal mediod value
      int m_history_size;

      // Rate at which to obtain new samples
      int m_sampling_rate;
    };

    // --- Prati Mediod BGS algorithm ---
    class PratiMediodBGS : public Bgs 
    {
    private:	
      // sum of L-inf distances from a sample point to all other sample points
      struct MEDIAN_BUFFER
      {
        std::vector<RgbPixel> pixels;		// vector of pixels at give location in image
        std::vector<int> dist;					// distance from pixel to all other pixels
        int pos;												// current position in circular buffer

        RgbPixel median;								// median at this pixel location
        int medianDist;									// distance from median pixel to all other pixels
      };

    public:
      PratiMediodBGS();
      ~PratiMediodBGS();

      void Initalize(const BgsParams& param);

      void InitModel(const RgbImage& data);
      void Subtract(int frame_num, const RgbImage& data,  
        BwImage& low_threshold_mask, BwImage& high_threshold_mask);	
      void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

      RgbImage* Background() { return &m_background; }

    private:	
      MEDIAN_BUFFER* m_median_buffer;

      void CalculateMasks(int r, int c, const RgbPixel& pixel);
      void Combine(const BwImage& low_mask, const BwImage& high_mask, BwImage& output);
      void UpdateMediod(int r, int c, const RgbImage& new_frame, int& dist);

      PratiParams m_params;

      RgbImage m_background;

      BwImage m_mask_low_threshold;
      BwImage m_mask_high_threshold;
    };

  }
}

#endif






