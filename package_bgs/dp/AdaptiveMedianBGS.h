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
* AdaptiveMedianBGS.hpp
*
* Purpose: Implementation of the simple adaptive median background 
*		  		 subtraction algorithm described in:
*	  			 "Segmentation and tracking of piglets in images"
* 						by McFarlane and Schofield
*
* Author: Donovan Parks, September 2007

Example:
		Algorithms::BackgroundSubtraction::AdaptiveMedianParams params;
		params.SetFrameSize(width, height);
		params.LowThreshold() = 40;
		params.HighThreshold() = 2*params.LowThreshold();
		params.SamplingRate() = 7;
		params.LearningFrames() = 30;

		Algorithms::BackgroundSubtraction::AdaptiveMedianBGS bgs;
		bgs.Initalize(params);
******************************************************************************/

#include "Bgs.h"

namespace Algorithms
{
	namespace BackgroundSubtraction
	{
		// --- Parameters used by the Adaptive Median BGS algorithm ---
		class AdaptiveMedianParams : public BgsParams
		{
		public:
			unsigned char &LowThreshold() { return m_low_threshold; }
			unsigned char &HighThreshold() { return m_high_threshold; }

			int &SamplingRate() { return m_samplingRate; }
			int &LearningFrames() { return m_learning_frames; }

		private:
			unsigned char m_low_threshold;
			unsigned char m_high_threshold;

			int m_samplingRate;
			int m_learning_frames;
		};


		// --- Adaptive Median BGS algorithm ---
		class AdaptiveMedianBGS : public Bgs 
		{
		public:
			virtual ~AdaptiveMedianBGS() {}

			void Initalize(const BgsParams& param);

			void InitModel(const RgbImage& data);
			void Subtract(int frame_num, const RgbImage& data,  
											BwImage& low_threshold_mask, BwImage& high_threshold_mask);	
			void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

			RgbImage* Background();

		private:	
			void SubtractPixel(int r, int c, const RgbPixel& pixel, 
													unsigned char& low_threshold, unsigned char& high_threshold);

			AdaptiveMedianParams m_params;

			RgbImage m_median;
		};
	}
}
