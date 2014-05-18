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
* PratiMediodBGS.h
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
******************************************************************************/

#include "PratiMediodBGS.h"

using namespace Algorithms::BackgroundSubtraction;

PratiMediodBGS::PratiMediodBGS()
{
	m_median_buffer = NULL;
}

PratiMediodBGS::~PratiMediodBGS()
{
	delete[] m_median_buffer;
}

void PratiMediodBGS::Initalize(const BgsParams& param)
{
	m_params = (PratiParams&)param;

	m_mask_low_threshold = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 1);
	m_mask_high_threshold = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 1);

	m_background = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 3);

	m_median_buffer = new MEDIAN_BUFFER[m_params.Size()];
}

void PratiMediodBGS::InitModel(const RgbImage& data)
{
	// there is no need to initialize the mode since it needs a buffer of frames
	// before it can performing background subtraction
}

void PratiMediodBGS::Update(int frame_num, const RgbImage& data,  const BwImage& update_mask)
{
	// update the image buffer with the new frame and calculate new median values
	if(frame_num % m_params.SamplingRate() == 0)
	{
		if(m_median_buffer[0].dist.size() == m_params.HistorySize())
		{
			// subtract distance to sample being removed from all distances
			for(unsigned int r = 0; r < m_params.Height(); ++r)
			{
				for(unsigned int c = 0; c < m_params.Width(); ++c)
				{	
					int i = r*m_params.Width()+c;

					if(update_mask(r,c) == BACKGROUND)
					{
						int oldPos = m_median_buffer[i].pos;
						for(unsigned int s = 0; s < m_median_buffer[i].pixels.size(); ++s)
						{
							int maxDist = 0;
							for(int ch = 0; ch < NUM_CHANNELS; ++ch)
							{
								int tempDist = abs(m_median_buffer[i].pixels.at(oldPos)(ch) 
																		- m_median_buffer[i].pixels.at(s)(ch));
								if(tempDist > maxDist)
									maxDist = tempDist;
							}

							m_median_buffer[i].dist.at(s) -= maxDist;
						}
				
						int dist;
						UpdateMediod(r, c, data, dist);
						m_median_buffer[i].dist.at(oldPos) = dist;
						m_median_buffer[i].pixels.at(oldPos) = data(r,c);
						m_median_buffer[i].pos++;
						if(m_median_buffer[i].pos >= m_params.HistorySize())
							m_median_buffer[i].pos = 0;
					}
				}
			}
		}
		else
		{
			// calculate sum of L-inf distances for new point and 
			// add distance from each sample point to this point to their L-inf sum
			int dist;
			for(unsigned int r = 0; r < m_params.Height(); ++r)
			{
				for(unsigned int c = 0; c < m_params.Width(); ++c)
				{	
					int index = r*m_params.Width()+c;
					UpdateMediod(r, c, data, dist);
					m_median_buffer[index].dist.push_back(dist);
					m_median_buffer[index].pos = 0;
					m_median_buffer[index].pixels.push_back(data(r,c)); 
				}
			}
		}
	}
}

void PratiMediodBGS::UpdateMediod(int r, int c, const RgbImage& new_frame, int& dist)
{
	// calculate sum of L-inf distances for new point and 
	// add distance from each sample point to this point to their L-inf sum
	unsigned int i = (r*m_params.Width()+c);

	m_median_buffer[i].medianDist = INT_MAX;

	int L_inf_dist = 0;
	for(unsigned int s = 0; s < m_median_buffer[i].dist.size(); ++s)
	{
		int maxDist = 0;
		for(int ch = 0; ch < NUM_CHANNELS; ++ch)
		{
			int tempDist = abs(m_median_buffer[i].pixels.at(s)(ch) - new_frame(r,c,ch));
			if(tempDist > maxDist)
				maxDist = tempDist;
		}

		// check if point from this frame in the image buffer is the median
		m_median_buffer[i].dist.at(s) += maxDist;
		if(m_median_buffer[i].dist.at(s) < m_median_buffer[i].medianDist)
		{
			m_median_buffer[i].medianDist = m_median_buffer[i].dist.at(s);
			m_median_buffer[i].median = m_median_buffer[i].pixels.at(s);
		}

		L_inf_dist += maxDist;
	}

	dist = L_inf_dist;

	// check if the new point is the median
	if(L_inf_dist < m_median_buffer[i].medianDist)
	{
		m_median_buffer[i].medianDist = L_inf_dist;
		m_median_buffer[i].median = new_frame(r,c);
	}
}

void PratiMediodBGS::Combine(const BwImage& low_mask, const BwImage& high_mask, BwImage& output)
{
	for(unsigned int r = 0; r < m_params.Height(); ++r)
	{
		for(unsigned int c = 0; c < m_params.Width(); ++c)
		{
			output(r,c) = BACKGROUND;

			if(r == 0 || c == 0 || r == m_params.Height()-1 || c == m_params.Width()-1)
				continue;	
			
			if(high_mask(r,c) == FOREGROUND)
			{
				output(r,c) = FOREGROUND;
			}
			else if(low_mask(r,c) == FOREGROUND)
			{
				// consider the pixel to be a F/G pixel if it is 8-connected to
				// a F/G pixel in the high mask
				// check if there is an 8-connected foreground pixel
				if(high_mask(r-1,c-1))	
					output(r,c) = FOREGROUND;
				else if(high_mask(r-1,c))	
					output(r,c) = FOREGROUND;
				else if(high_mask(r-1,c+1))	
					output(r,c) = FOREGROUND;
				else if(high_mask(r,c-1))	
					output(r,c) = FOREGROUND;
				else if(high_mask(r,c+1))	
					output(r,c) = FOREGROUND;
				else if(high_mask(r+1,c-1))	
					output(r,c) = FOREGROUND;
				else if(high_mask(r+1,c))	
					output(r,c) = FOREGROUND;
				else if(high_mask(r+1,c+1))	
					output(r,c) = FOREGROUND;
			}
		}
	}
}

void PratiMediodBGS::CalculateMasks(int r, int c, const RgbPixel& pixel)
{
	int pos = r*m_params.Width()+c;

	// calculate l-inf distance between current value and median value
	unsigned char dist = 0;
	for(int ch = 0; ch < NUM_CHANNELS; ++ch)
	{
		int tempDist = abs(pixel(ch) - m_median_buffer[pos].median(ch));
		if(tempDist > dist)
			dist = tempDist;
	}
	m_background(r,c) = m_median_buffer[pos].median;

	// check if pixel is a B/G or F/G pixel according to the low threshold B/G model
	m_mask_low_threshold(r,c) = BACKGROUND;
	if(dist > m_params.LowThreshold())
	{
		m_mask_low_threshold(r,c) = FOREGROUND;
	}

	// check if pixel is a B/G or F/G pixel according to the high threshold B/G model
	m_mask_high_threshold(r,c)= BACKGROUND;
	if(dist > m_params.HighThreshold())
	{
		m_mask_high_threshold(r,c) = FOREGROUND;
	}
}

///////////////////////////////////////////////////////////////////////////////
//Input:
//  data - a pointer to the data of a RGB image of the same size
//Output:
//  output - a pointer to the data of a gray value image of the same size 
//					values: 255-foreground, 0-background
///////////////////////////////////////////////////////////////////////////////
void PratiMediodBGS::Subtract(int frame_num, const RgbImage& data, 
																BwImage& low_threshold_mark, BwImage& high_threshold_mark)
{
	if(frame_num < m_params.HistorySize())
	{
		low_threshold_mark.Clear();
		high_threshold_mark.Clear();
		return;
	}

	// update each pixel of the image
	for(unsigned int r = 0; r < m_params.Height(); ++r)
	{
		for(unsigned int c = 0; c < m_params.Width(); ++c)
		{	
			// need at least one frame of data before we can start calculating the masks
			CalculateMasks(r, c, data(r,c));
		}
	}

	// combine low and high threshold masks
	Combine(m_mask_low_threshold, m_mask_high_threshold, low_threshold_mark);
	Combine(m_mask_low_threshold, m_mask_high_threshold, high_threshold_mark);
}




