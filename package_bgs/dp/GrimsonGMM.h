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
* GrimsonGMM.cpp
*
* Purpose: Implementation of the Gaussian mixture model (GMM) background 
*		  		 subtraction described in:
*	  			 "Adaptive background mixture models for real-time tracking"
* 						by Chris Stauffer and W.E.L Grimson
*
* Author: Donovan Parks, September 2007
*
* This code is based on code by Z. Zivkovic's written for his enhanced GMM
* background subtraction algorithm: 
*
*	"Improved adaptive Gausian mixture model for background subtraction"
*		Z.Zivkovic 
*		International Conference Pattern Recognition, UK, August, 2004
*
*
* "Efficient Adaptive Density Estimapion per Image Pixel for the 
*			Task of Background Subtraction"
*		Z.Zivkovic, F. van der Heijden 
*		Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006.
*
* Zivkovic's code can be obtained at: www.zoranz.net

Example:
		Algorithms::BackgroundSubtraction::GrimsonParams params;
		params.SetFrameSize(width, height);
		params.LowThreshold() = 3.0f*3.0f;
		params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
		params.Alpha() = 0.001f;
		params.MaxModes() = 3;

		Algorithms::BackgroundSubtraction::GrimsonGMM bgs;
		bgs.Initalize(params);
******************************************************************************/

#ifndef GRIMSON_GMM_
#define GRIMSON_GMM_

#include "Bgs.h"

namespace Algorithms
{
	namespace BackgroundSubtraction
	{
		typedef struct GMMGaussian
		{
			float variance;
			float muR;
			float muG;
			float muB;
			float weight;
			float significants;		// this is equal to weight / standard deviation and is used to
														// determine which Gaussians should be part of the background model
		} GMM;

		// --- User adjustable parameters used by the Grimson GMM BGS algorithm ---
		class GrimsonParams : public BgsParams
		{
		public:
			float &LowThreshold() { return m_low_threshold; }
			float &HighThreshold() { return m_high_threshold; }

			float &Alpha() { return m_alpha; }
			int &MaxModes() { return m_max_modes; }

		private:
			// Threshold on the squared dist. to decide when a sample is close to an existing 
			// components. If it is not close to any a new component will be generated. 
			// Smaller threshold values lead to more generated components and higher threshold values 
			// lead to a small number of components but they can grow too large.
			//
			// It is usual easiest to think of these thresholds as being the number of variances away
			// from the mean of a pixel before it is considered to be from the foreground.
			float m_low_threshold;
			float m_high_threshold;

			// alpha - speed of update - if the time interval you want to average over is T
			// set alpha=1/T. 
			float m_alpha;

			// Maximum number of modes (Gaussian components) that will be used per pixel
			int m_max_modes;
		};

		// --- Grimson GMM BGS algorithm ---
		class GrimsonGMM : public Bgs
		{
		public:
			GrimsonGMM();
			~GrimsonGMM();

			void Initalize(const BgsParams& param);

			void InitModel(const RgbImage& data);
			void Subtract(int frame_num, const RgbImage& data,  
											BwImage& low_threshold_mask, BwImage& high_threshold_mask);	
			void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

			RgbImage* Background();

		private:	
			void SubtractPixel(long posPixel, const RgbPixel& pixel, unsigned char& numModes, 
													unsigned char& lowThreshold, unsigned char& highThreshold);

			// User adjustable parameters
			GrimsonParams m_params;

			// Threshold when the component becomes significant enough to be included into
			// the background model. It is the TB = 1-cf from the paper. So I use cf=0.1 => TB=0.9
			// For alpha=0.001 it means that the mode should exist for approximately 105 frames before
			// it is considered foreground
			float m_bg_threshold; //1-cf from the paper

			// Initial variance for the newly generated components. 
			// It will will influence the speed of adaptation. A good guess should be made. 
			// A simple way is to estimate the typical standard deviation from the images.
			float m_variance;

			// Dynamic array for the mixture of Gaussians
			GMM* m_modes;

			// Number of Gaussian components per pixel
			BwImage m_modes_per_pixel;

			// Current background model
			RgbImage m_background;
		};
	}
}

#endif
