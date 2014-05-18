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
* ZivkovicAGMM.cpp
*
* Purpose: Implementation of the Gaussian mixture model (GMM) background 
*		  		 subtraction algorithm developed by Z. Zivkovic.
*
* Author: Donovan Parks, September 2007
*
* This code is based on code by Z. Zivkovic's. I have changed it from a pure
* C implementation to a cleaner (IMHO) C++ implementation. It is based on the
* following papers:
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
******************************************************************************/

#include "ZivkovicAGMM.h"

using namespace Algorithms::BackgroundSubtraction;

ZivkovicAGMM::ZivkovicAGMM()
{
	m_modes = NULL;
	m_modes_per_pixel = NULL;
}

ZivkovicAGMM::~ZivkovicAGMM()
{
	delete[] m_modes;
	delete[] m_modes_per_pixel;
}

void ZivkovicAGMM::Initalize(const BgsParams& param)
{
	m_params = (ZivkovicParams&)param;

	m_num_bands = 3;							//always 3 - not implemented for other values!
	m_bg_threshold = 0.75f;				//1-cf from the paper 
	m_variance = 36.0f;						// variance for the new mode
	m_complexity_prior = 0.05f;		// complexity reduction prior constant

	// GMM for each pixel
	m_modes = new GMM[m_params.Size()*m_params.MaxModes()];

	// used modes per pixel
	m_modes_per_pixel = new unsigned char[m_params.Size()];

	m_background = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 3);
}

void ZivkovicAGMM::InitModel(const RgbImage& data)
{
	for(unsigned int i = 0; i < m_params.Size(); ++i)
	{
		m_modes_per_pixel[i] = 0;
	}

	for(unsigned int i = 0; i < m_params.Size()*m_params.MaxModes(); ++i)
	{
		m_modes[i].weight = 0;
		m_modes[i].sigma = 0;
		m_modes[i].muR = 0;
		m_modes[i].muG = 0;
		m_modes[i].muB = 0;
	}
}

void ZivkovicAGMM::Update(int frame_num, const RgbImage& data,  const BwImage& update_mask)
{
	// it doesn't make sense to have conditional updates in the GMM framework
}

void ZivkovicAGMM::SubtractPixel(long posPixel, const RgbPixel& pixel, unsigned char* pModesUsed, 
																	unsigned char& low_threshold, unsigned char& high_threshold)
{
	//calculate distances to the modes (+ sort???)
	//here we need to go in descending order!!!
	long pos;
	bool bFitsPDF=0;
	bool bBackgroundLow=false;
	bool bBackgroundHigh=false;

	float fOneMinAlpha = 1-m_params.Alpha();

	float prune = -m_params.Alpha()*m_complexity_prior;

	int nModes =* pModesUsed;
	float totalWeight = 0.0f;

	// calculate number of Gaussians to include in the background model
	int backgroundGaussians = 0;
	double sum = 0.0;
	for(int i = 0; i < nModes; ++i)
	{
		if(sum < m_bg_threshold)
		{
			backgroundGaussians++;
			sum += m_modes[posPixel+i].weight;
		}
		else
		{
			break;
		}
	}

	// update all distributions and check for match with current pixel
	for (int iModes = 0; iModes < nModes; iModes++)
	{
		pos=posPixel+iModes;
		float weight = m_modes[pos].weight;

		//fit not found yet
		if (!bFitsPDF)
		{
			//check if it belongs to some of the modes
			//calculate distance
			float var = m_modes[pos].sigma;
			float muR = m_modes[pos].muR;
			float muG = m_modes[pos].muG;
			float muB = m_modes[pos].muB;
		
			float dR=muR - pixel(0);
			float dG=muG - pixel(1);
			float dB=muB - pixel(2);
			
			// calculate the squared distance
			float dist = (dR*dR + dG*dG + dB*dB);

			if(dist < m_params.HighThreshold()*var && iModes < backgroundGaussians)
				bBackgroundHigh = true;
			
			//check fit
			if (dist < m_params.LowThreshold()*var)
			{
				/////
				//belongs to the mode
				bFitsPDF = true;

				// check if this Gaussian is part of the background model
				if(iModes < backgroundGaussians) 
					bBackgroundLow = true;

				//update distribution
				float k = m_params.Alpha()/weight;
				weight = fOneMinAlpha*weight+prune;
				weight += m_params.Alpha();
				m_modes[pos].weight = weight;
				m_modes[pos].muR = muR - k*(dR);
				m_modes[pos].muG = muG - k*(dG);
				m_modes[pos].muB = muB - k*(dB);

				//limit update speed for cov matrice
				//not needed
				//k=k>20*m_m_params.Alpha()?20*m_m_params.Alpha():k;
				//float sigmanew = var + k*((0.33*(dR*dR+dG*dG+dB*dB))-var);
				//float sigmanew = var + k*((dR*dR+dG*dG+dB*dB)-var);
				//float sigmanew = var + k*((0.33*dist)-var);
				float sigmanew = var + k*(dist-var);

				//limit the variance
				m_modes[pos].sigma = sigmanew < 4 ? 4 : sigmanew > 5*m_variance ? 5*m_variance : sigmanew;

				// Sort weights so they are in desending order. Note that only the weight for this
				// mode will increase and that the weight for all modes that were previously larger than
				// this one have already been modified and will not be modified again. Thus, we just need to 
				// the correct position of this mode in the already sorted list.

				// Zivkovic implementation has been modified for clarity, but the results are equivalent
				/*
				for (int iLocal = iModes;iLocal>0;iLocal--)
				{
					long posLocal=posPixel + iLocal;
					if (weight < (m_modes[posLocal-1].weight))
					{
						break;
					}
					else
					{
						//swap
						GMM temp = m_modes[posLocal];
						m_modes[posLocal] = m_modes[posLocal-1];
						m_modes[posLocal-1] = temp;
					}
				}
				*/

				for (int iLocal = iModes; iLocal > 0; iLocal--)
				{
					long posLocal = posPixel + iLocal;
					if (m_modes[posLocal].weight > m_modes[posLocal-1].weight)
					{
						//swap
						GMM temp = m_modes[posLocal];
						m_modes[posLocal] = m_modes[posLocal-1];
						m_modes[posLocal-1] = temp;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				weight = fOneMinAlpha*weight+prune;
				//check prune
				if (weight < -prune)
				{
					weight=0.0;
					nModes--;
				}
				m_modes[pos].weight = weight;
			}
			//check if it fits the current mode (2.5 sigma)
			///////
		}
		//fit not found yet
		/////
		else
		{
			weight = fOneMinAlpha*weight + prune;
			//check prune
			if (weight < -prune)
			{
				weight=0.0;
				nModes--;
			}
			m_modes[pos].weight = weight;
		}
		totalWeight += weight;
	}

	//renormalize weights so they sum to 1
	for (int iLocal = 0; iLocal < nModes; iLocal++)
	{
		m_modes[posPixel+ iLocal].weight = m_modes[posPixel+ iLocal].weight/totalWeight;
	}
	
	//make new mode if needed and exit
	if (!bFitsPDF)
	{
		if (nModes == m_params.MaxModes())
		{
			//replace the weakest
		}
		else
		{
			nModes++;
		}
		pos = posPixel + nModes-1;

    if (nModes==1)
			m_modes[pos].weight=1;
		else
			m_modes[pos].weight=m_params.Alpha();

		// Zivkovic implementation changes as this will not result in the
		// weights adding to 1
		/*
		int iLocal;
		for (iLocal = 0; iLocal < m_params.MaxModes()odes-1; iLocal++)
		{
			m_modes[posPixel+ iLocal].weight *= fOneMinAlpha;
		}
		*/

		// Revised implementation:
		//renormalize weights
		int iLocal;
		float sum = 0.0;
		for (iLocal = 0; iLocal < nModes; iLocal++)
		{
			sum += m_modes[posPixel+ iLocal].weight;
		}

		float invSum = 1.0f/sum;
		for (iLocal = 0; iLocal < nModes; iLocal++)
		{
			m_modes[posPixel+ iLocal].weight *= invSum;
		}

		m_modes[pos].muR=pixel(0);
		m_modes[pos].muG=pixel(1);
		m_modes[pos].muB=pixel(2);
		m_modes[pos].sigma=m_variance;

		// Zivkovic implementation to sort GMM so they are sorted in descending order according to their weight.
		// It has been revised for clarity, but the results are equivalent
		/*
		for (iLocal = m_params.MaxModes()odes-1; iLocal > 0; iLocal--)
		{
			long posLocal = posPixel + iLocal;
			if (m_params.Alpha() < (m_modes[posLocal-1].weight))
			{
				break;
			}
			else
			{
				//swap
				GMM temp = m_modes[posLocal];
				m_modes[posLocal] = m_modes[posLocal-1];
				m_modes[posLocal-1] = temp;
			}
		}
		*/

		// sort GMM so they are sorted in descending order according to their weight
		for (iLocal = nModes-1; iLocal > 0; iLocal--)
		{
			long posLocal = posPixel + iLocal;
			if (m_modes[posLocal].weight > m_modes[posLocal-1].weight)
			{
				//swap
				GMM temp = m_modes[posLocal];
				m_modes[posLocal] = m_modes[posLocal-1];
				m_modes[posLocal-1] = temp;
			}
			else
			{
				break;
			}
		}
	}

	//set the number of modes
	*pModesUsed=nModes;

	if(bBackgroundLow)
	{
		low_threshold = BACKGROUND;
	}
	else
	{
		low_threshold = FOREGROUND;
	}

	if(bBackgroundHigh)
	{
		high_threshold = BACKGROUND;
	}
	else
	{
		high_threshold = FOREGROUND;
	}
}


///////////////////////////////////////////////////////////////////////////////
//Input:
//  data - a pointer to the data of a RGB image of the same size
//Output:
//  output - a pointer to the data of a gray value image of the same size 
//					(the memory should already be reserved) 
//					values: 255-foreground, 125-shadow, 0-background
///////////////////////////////////////////////////////////////////////////////
void ZivkovicAGMM::Subtract(int frame_num, const RgbImage& data,  
															BwImage& low_threshold_mask, BwImage& high_threshold_mask)
{
	unsigned char low_threshold, high_threshold;

	// update each pixel of the image
	long posPixel;
	unsigned char* pUsedModes=m_modes_per_pixel;
	for(unsigned int r = 0; r < m_params.Height(); ++r)
	{
		for(unsigned int c = 0; c < m_params.Width(); ++c)
		{
			//update model+ background subtract
			posPixel=(r*m_params.Width()+c)*m_params.MaxModes();
			SubtractPixel(posPixel, data(r,c), pUsedModes, low_threshold, high_threshold);
			low_threshold_mask(r,c) = low_threshold;
			high_threshold_mask(r,c) = high_threshold;

			m_background(r,c,0) = (unsigned char)m_modes[posPixel].muR;
			m_background(r,c,1) = (unsigned char)m_modes[posPixel].muG;
			m_background(r,c,2) = (unsigned char)m_modes[posPixel].muB;

			pUsedModes++;
		}
	}
}

