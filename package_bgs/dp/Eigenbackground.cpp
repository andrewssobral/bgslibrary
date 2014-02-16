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
* Eigenbackground.cpp
*
* Purpose: Implementation of the Eigenbackground background subtraction 
*					 algorithm developed by Oliver et al.
*
* Author: Donovan Parks, September 2007
*
* "A Bayesian Computer Vision System for Modeling Human Interactions"
*   Nuria Oliver, Barbara Rosario, Alex P. Pentland 2000
*
******************************************************************************/

#include "Eigenbackground.h"

using namespace Algorithms::BackgroundSubtraction;

Eigenbackground::Eigenbackground()
{
	m_pcaData = NULL;
	m_pcaAvg = NULL;
	m_eigenValues = NULL;
	m_eigenVectors = NULL;
}

Eigenbackground::~Eigenbackground()
{
	if(m_pcaData != NULL) cvReleaseMat(&m_pcaData);
	if(m_pcaAvg != NULL) cvReleaseMat(&m_pcaAvg);
	if(m_eigenValues != NULL) cvReleaseMat(&m_eigenValues);
	if(m_eigenVectors != NULL) cvReleaseMat(&m_eigenVectors);
}

void Eigenbackground::Initalize(const BgsParams& param)
{
	m_params = (EigenbackgroundParams&)param;
	
	m_background = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 3);
	m_background.Clear();
}

void Eigenbackground::InitModel(const RgbImage& data)
{
	if(m_pcaData != NULL) cvReleaseMat(&m_pcaData);
	if(m_pcaAvg != NULL) cvReleaseMat(&m_pcaAvg);
	if(m_eigenValues != NULL) cvReleaseMat(&m_eigenValues);
	if(m_eigenVectors != NULL) cvReleaseMat(&m_eigenVectors);

	m_pcaData = cvCreateMat(m_params.HistorySize(), m_params.Size()*3, CV_8UC1);

	m_background.Clear();
}

void Eigenbackground::Update(int frame_num, const RgbImage& data,  const BwImage& update_mask)
{
	// the eigenbackground model is not updated (serious limitation!)
}

void Eigenbackground::Subtract(int frame_num, const RgbImage& data,  
																BwImage& low_threshold_mask, BwImage& high_threshold_mask)
{
	// create eigenbackground
	if(frame_num == m_params.HistorySize())
	{
		// create the eigenspace
		m_pcaAvg = cvCreateMat( 1, m_pcaData->cols, CV_32F );
		m_eigenValues = cvCreateMat( m_pcaData->rows, 1, CV_32F ); 
		m_eigenVectors = cvCreateMat( m_pcaData->rows, m_pcaData->cols, CV_32F );
		cvCalcPCA(m_pcaData, m_pcaAvg, m_eigenValues, m_eigenVectors, CV_PCA_DATA_AS_ROW);

		int index = 0;
		for(unsigned int r = 0; r < m_params.Height(); ++r)
		{
			for(unsigned int c = 0; c < m_params.Width(); ++c)
			{
				for(int ch = 0; ch < m_background.Ptr()->nChannels; ++ch)
				{
					m_background(r,c,0) = static_cast<unsigned char>(cvmGet(m_pcaAvg,0,index)+0.5);
					index++;
				}
			}
		}
	}

	if(frame_num >= m_params.HistorySize())
	{
		// project new image into the eigenspace
		int w = data.Ptr()->width;
		int h = data.Ptr()->height;
		int ch = data.Ptr()->nChannels;
		CvMat* dataPt = cvCreateMat(1, w*h*ch, CV_8UC1); 
		CvMat data_row;
		cvGetRow(dataPt, &data_row, 0);
		cvReshape(&data_row, &data_row, 3, data.Ptr()->height); 
		cvCopy(data.Ptr(), &data_row); 

		CvMat* proj = cvCreateMat(1, m_params.EmbeddedDim(), CV_32F);
		cvProjectPCA(dataPt, m_pcaAvg, m_eigenVectors, proj);

		// reconstruct point
		CvMat* result = cvCreateMat(1, m_pcaData->cols, CV_32F);
		cvBackProjectPCA(proj, m_pcaAvg, m_eigenVectors, result);

		// calculate Euclidean distance between new image and its eigenspace projection
		int index = 0;
		for(unsigned int r = 0; r < m_params.Height(); ++r)
		{
			for(unsigned int c = 0; c < m_params.Width(); ++c)
			{
				double dist = 0;
				bool bgLow = true;
				bool bgHigh = true;
				for(int ch = 0; ch < 3; ++ch)
				{
					dist = (data(r,c,ch) - cvmGet(result,0,index))*(data(r,c,ch) - cvmGet(result,0,index));
					if(dist > m_params.LowThreshold())
						bgLow = false;
					if(dist > m_params.HighThreshold())
						bgHigh = false;
					index++;
				}
				
				if(!bgLow)
				{
					low_threshold_mask(r,c) = FOREGROUND;
				}
				else
				{
					low_threshold_mask(r,c) = BACKGROUND;
				}

				if(!bgHigh)
				{
					high_threshold_mask(r,c) = FOREGROUND;
				}
				else
				{
					high_threshold_mask(r,c) = BACKGROUND;
				}
			}
		}
		
		cvReleaseMat(&result);
		cvReleaseMat(&proj);		
		cvReleaseMat(&dataPt);
	}
	else 
	{
		// set entire image to background since there is not enough information yet
		// to start performing background subtraction
		for(unsigned int r = 0; r < m_params.Height(); ++r)
		{
			for(unsigned int c = 0; c < m_params.Width(); ++c)
			{
				low_threshold_mask(r,c) = BACKGROUND;
				high_threshold_mask(r,c) = BACKGROUND;
			}
		}
	}

	UpdateHistory(frame_num, data);
}

void Eigenbackground::UpdateHistory(int frame_num, const RgbImage& new_frame)
{
	if(frame_num < m_params.HistorySize())
	{
		CvMat src_row;
		cvGetRow(m_pcaData, &src_row, frame_num);
		cvReshape(&src_row, &src_row, 3, new_frame.Ptr()->height);
		cvCopy(new_frame.Ptr(), &src_row);
	}
}
