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
* MeanBGS.h
*
* Purpose: Implementation of a simple temporal mean background 
*		  		 subtraction algorithm.
*
* Author: Donovan Parks, September 2007
*
******************************************************************************/

#include "MeanBGS.h"

using namespace Algorithms::BackgroundSubtraction;

void MeanBGS::Initalize(const BgsParams& param)
{
	m_params = (MeanParams&)param;

	m_mean = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_32F, 3);
	m_background = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 3);
}

void MeanBGS::InitModel(const RgbImage& data)
{
	for (unsigned int r = 0; r < m_params.Height(); ++r)
	{
		for(unsigned int c = 0; c < m_params.Width(); ++c)
		{
			for(int ch = 0; ch < NUM_CHANNELS; ++ch)
			{
				m_mean(r,c,ch) = (float)data(r,c,ch);
			}
		}
	}
}

void MeanBGS::Update(int frame_num, const RgbImage& data,  const BwImage& update_mask)
{
	// update background model
	for (unsigned int r = 0; r < m_params.Height(); ++r)
	{
		for(unsigned int c = 0; c < m_params.Width(); ++c)
		{
			// perform conditional updating only if we are passed the learning phase
			if(update_mask(r,c) == BACKGROUND || frame_num < m_params.LearningFrames())
			{
				// update B/G model
				float mean;
				for(int ch = 0; ch < NUM_CHANNELS; ++ch)
				{
					mean = m_params.Alpha() * m_mean(r,c,ch) + (1.0f-m_params.Alpha()) * data(r,c,ch);
					m_mean(r,c,ch) = mean;
					m_background(r,c,ch) = (unsigned char)(mean + 0.5);
				}
			}
		}
	}
}

void MeanBGS::SubtractPixel(int r, int c, const RgbPixel& pixel, 
															unsigned char& low_threshold, 
															unsigned char& high_threshold)
{
	// calculate distance to sample point
	float dist = 0;
	for(int ch = 0; ch < NUM_CHANNELS; ++ch)
	{
		dist += (pixel(ch)-m_mean(r,c,ch))*(pixel(ch)-m_mean(r,c,ch));
	}

	// determine if sample point is F/G or B/G pixel
	low_threshold = BACKGROUND;
	if(dist > m_params.LowThreshold())
	{
		low_threshold = FOREGROUND;
	}

	high_threshold = BACKGROUND;
	if(dist > m_params.HighThreshold())
	{
		high_threshold = FOREGROUND;
	}
}

///////////////////////////////////////////////////////////////////////////////
//Input:
//  data - a pointer to the data of a RGB image of the same size
//Output:
//  output - a pointer to the data of a gray value image of the same size 
//					values: 255-foreground, 0-background
///////////////////////////////////////////////////////////////////////////////
void MeanBGS::Subtract(int frame_num, const RgbImage& data, 
												BwImage& low_threshold_mask, BwImage& high_threshold_mask)
{
	unsigned char low_threshold, high_threshold;

	// update each pixel of the image
	for(unsigned int r = 0; r < m_params.Height(); ++r)
	{
		for(unsigned int c = 0; c < m_params.Width(); ++c)
		{	
			// perform background subtraction + update background model
			SubtractPixel(r, c, data(r,c), low_threshold, high_threshold);

			// setup silhouette mask
			low_threshold_mask(r,c) = low_threshold;
			high_threshold_mask(r,c) = high_threshold;
		}
	}
}




