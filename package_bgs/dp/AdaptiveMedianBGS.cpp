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
* AdaptiveMedianBGS.cpp
*
* Purpose: Implementation of the simple adaptive median background 
*		  		 subtraction algorithm described in:
*	  			 "Segmentation and tracking of piglets in images"
* 						by McFarlane and Schofield
*
* Author: Donovan Parks, September 2007
*
******************************************************************************/

#include <iostream>
#include <stdlib.h>
#include <cmath>

#include "AdaptiveMedianBGS.h"

using namespace Algorithms::BackgroundSubtraction;

void AdaptiveMedianBGS::Initalize(const BgsParams& param)
{
	m_params = (AdaptiveMedianParams&)param;

	m_median = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 3);
	cvSet(m_median.Ptr(), CV_RGB(BACKGROUND,BACKGROUND,BACKGROUND));
}

RgbImage* AdaptiveMedianBGS::Background()
{
	return &m_median;
}

void AdaptiveMedianBGS::InitModel(const RgbImage& data)
{
	// initialize the background model
	for (unsigned int r = 0; r < m_params.Height(); ++r)
	{
		for(unsigned int c = 0; c < m_params.Width(); ++c)
		{
			m_median(r,c) = data(r,c);
		}
	}
}

void AdaptiveMedianBGS::Update(int frame_num, const RgbImage& data,  const BwImage& update_mask)
{
	if(frame_num % m_params.SamplingRate() == 1)
	{
		// update background model
		for (unsigned int r = 0; r < m_params.Height(); ++r)
		{
			for(unsigned int c = 0; c < m_params.Width(); ++c)
			{
				// perform conditional updating only if we are passed the learning phase
				if(update_mask(r,c) == BACKGROUND || frame_num < m_params.LearningFrames())
				{
					for(int ch = 0; ch < NUM_CHANNELS; ++ch)
					{
						if(data(r,c,ch) > m_median(r,c,ch))
						{
							m_median(r,c,ch)++;
						}
						else if(data(r,c,ch) < m_median(r,c,ch))
						{
							m_median(r,c,ch)--;
						}
					}
				}
			}
		}
	}
}

void AdaptiveMedianBGS::SubtractPixel(int r, int c, const RgbPixel& pixel, 
																			unsigned char& low_threshold, unsigned char& high_threshold)
{
	// perform background subtraction
	low_threshold = high_threshold = FOREGROUND;
	
	int diffR = abs(pixel(0) - m_median(r,c,0));
	int diffG = abs(pixel(1) - m_median(r,c,1));
	int diffB = abs(pixel(2) - m_median(r,c,2));
	
	if(diffR <= m_params.LowThreshold() && diffG <= m_params.LowThreshold() &&  diffB <= m_params.LowThreshold())
	{
		low_threshold = BACKGROUND;
	}

	if(diffR <= m_params.HighThreshold() && diffG <= m_params.HighThreshold() &&  diffB <= m_params.HighThreshold())
	{
		high_threshold = BACKGROUND;
	}
}

///////////////////////////////////////////////////////////////////////////////
//Input:
//  data - a pointer to the image data
//Output:
//  output - a pointer to the data of a gray value image
//					(the memory should already be reserved) 
//					values: 255-foreground, 0-background
///////////////////////////////////////////////////////////////////////////////
void AdaptiveMedianBGS::Subtract(int frame_num, const RgbImage& data, 
																	BwImage& low_threshold_mask, BwImage& high_threshold_mask)
{
	unsigned char low_threshold, high_threshold;

	// update each pixel of the image
	for(unsigned int r = 0; r < m_params.Height(); ++r)
	{
		for(unsigned int c = 0; c < m_params.Width(); ++c)
		{	
			// perform background subtraction
			SubtractPixel(r, c, data(r,c), low_threshold, high_threshold);

			// setup silhouette mask
			low_threshold_mask(r,c) = low_threshold;
			high_threshold_mask(r,c) = high_threshold;
		}
	}
}

