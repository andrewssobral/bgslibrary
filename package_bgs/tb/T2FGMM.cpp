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
* T2FGMM.cpp
* 
* Purpose: Implementation of the T2 Fuzzy Gaussian Mixture Models (T2GMMs) 
* "Modeling of Dynamic Backgrounds by Type-2 Fuzzy Gaussians Mixture Models"
* Author: Fida El Baf, Thierry Bouwmans, September 2008.

******************************************************************************/

#include "T2FGMM.h"

using namespace Algorithms::BackgroundSubtraction;

int compareT2FGMM(const void* _gmm1, const void* _gmm2)
{
  GMM gmm1 = *(GMM*)_gmm1;
  GMM gmm2 = *(GMM*)_gmm2;

  if(gmm1.significants < gmm2.significants)
    return 1;
  else if(gmm1.significants == gmm2.significants)
    return 0;
  else
    return -1;
}

T2FGMM::T2FGMM()
{
  m_modes = NULL;
}

T2FGMM::~T2FGMM()
{
  delete[] m_modes;
}

void T2FGMM::Initalize(const BgsParams& param)
{
  m_params = (T2FGMMParams&) param;

  // Tbf - the threshold
  m_bg_threshold = 0.75f;	// 1-cf from the paper 

  // Tgenerate - the threshold
  m_variance = 36.0f;		// sigma for the new mode

  // GMM for each pixel
  m_modes = new GMM[m_params.Size()*m_params.MaxModes()];

  // used modes per pixel
  m_modes_per_pixel = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 1);

  m_background = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 3);

  // Factor control for the T2FGMM-UM [0,3]
  //km = (float) 1.5;
  km = (float) m_params.KM();

  // Factor control for the T2FGMM-UV [0.3,1]
  //kv = (float) 0.6;
  kv = (float) m_params.KV();
}

RgbImage* T2FGMM::Background()
{
  return &m_background;
}

void T2FGMM::InitModel(const RgbImage& data)
{
  m_modes_per_pixel.Clear();

  for(unsigned int i = 0; i < m_params.Size()*m_params.MaxModes(); ++i)
  {
    m_modes[i].weight = 0;
    m_modes[i].variance = 0;
    m_modes[i].muR = 0;
    m_modes[i].muG = 0;
    m_modes[i].muB = 0;
    m_modes[i].significants = 0;
  }
}

void T2FGMM::Update(int frame_num, const RgbImage& data,  const BwImage& update_mask)
{
  // it doesn't make sense to have conditional updates in the GMM framework
}

void T2FGMM::SubtractPixel(long posPixel, const RgbPixel& pixel, unsigned char& numModes, 
                           unsigned char& low_threshold, unsigned char& high_threshold)
{
  // calculate distances to the modes (+ sort???)
  // here we need to go in descending order!!!
  long pos;
  bool bFitsPDF = false;
  bool bBackgroundLow = false;
  bool bBackgroundHigh = false;

  float fOneMinAlpha = 1 - m_params.Alpha();
  float totalWeight = 0.0f;

  // calculate number of Gaussians to include in the background model
  int backgroundGaussians = 0;
  double sum = 0.0;
  for(int i = 0; i < numModes; ++i)
  {
    if(sum < m_bg_threshold)
    {
      backgroundGaussians++;
      sum += m_modes[posPixel+i].weight;
    }
    else
      break;
  }

  // update all distributions and check for match with current pixel
  for(int iModes = 0; iModes < numModes; iModes++)
  {
    pos = posPixel + iModes;
    float weight = m_modes[pos].weight;

    // fit not found yet
    if (!bFitsPDF)
    {
      //check if it belongs to some of the modes
      //calculate distance
      float var = m_modes[pos].variance;
      float muR = m_modes[pos].muR;
      float muG = m_modes[pos].muG;
      float muB = m_modes[pos].muB;

      //float km = 2;
      //float kv = 0.9;

      float dR = fabs(muR - pixel(0));
      float dG = fabs(muG - pixel(1));
      float dB = fabs(muB - pixel(2));

      // calculate the squared distance
      float HR;
      float HG;
      float HB;

      // T2FGMM-UM
      if(m_params.Type() == TYPE_T2FGMM_UM)
      {
        if((pixel(0)<muR-km*var)|| (pixel(0)>muR+km*var))
          HR=2*km*dR/var;
        else
          HR=dR*dR/(2*var*var)+km*dR/var+km*km/2;

        if((pixel(1)<muG-km*var)|| (pixel(1)>muG+km*var))
          HG=2*km*dG/var;
        else
          HG=dG*dG/(2*var*var)+km*dG/var+km*km/2;

        if((pixel(2)<muB-km*var)|| (pixel(2)>muB+km*var))
          HB=2*km*dB/var;
        else
          HB=dB*dB/(2*var*var)+km*dB/var+km*km/2;
      }

      // T2FGMM-UV
      if(m_params.Type() == TYPE_T2FGMM_UV)
      {
        HR = (1/(kv*kv)-kv*kv) * (pixel(0)-muR) * (pixel(0)-muR)/(2*var);
        HG = (1/(kv*kv)-kv*kv) * (pixel(1)-muG) * (pixel(1)-muG)/(2*var);
        HB = (1/(kv*kv)-kv*kv) * (pixel(2)-muB) * (pixel(2)-muB)/(2*var);
      }
      
      // calculate the squared distance
      float dist = (HR*HR + HG*HG + HB*HB);

      if(dist < m_params.HighThreshold()*var && iModes < backgroundGaussians)
        bBackgroundHigh = true;

      // a match occurs when the pixel is within sqrt(fTg) standard deviations of the distribution
      if(dist < m_params.LowThreshold()*var)
      {
        bFitsPDF = true;

        // check if this Gaussian is part of the background model
        if(iModes < backgroundGaussians) 
          bBackgroundLow = true;

        //update distribution
        float k = m_params.Alpha() / weight;
        weight = fOneMinAlpha*weight + m_params.Alpha();
        m_modes[pos].weight = weight;
        m_modes[pos].muR = muR - k*(dR);
        m_modes[pos].muG = muG - k*(dG);
        m_modes[pos].muB = muB - k*(dB);

        //limit the variance
        float sigmanew = var + k*(dist-var);
        m_modes[pos].variance = sigmanew < 4 ? 4 : sigmanew > 5*m_variance ? 5*m_variance : sigmanew;
        m_modes[pos].significants = m_modes[pos].weight / sqrt(m_modes[pos].variance);
      }
      else
      {
        weight = fOneMinAlpha*weight;
        if(weight < 0.0)
        {
          weight=0.0;
          numModes--;
        }

        m_modes[pos].weight = weight;
        m_modes[pos].significants = m_modes[pos].weight / sqrt(m_modes[pos].variance);
      }
    }
    else
    {
      weight = fOneMinAlpha*weight;
      if(weight < 0.0)
      {
        weight=0.0;
        numModes--;
      }
      m_modes[pos].weight = weight;
      m_modes[pos].significants = m_modes[pos].weight / sqrt(m_modes[pos].variance);
    }

    totalWeight += weight;
  }

  // renormalize weights so they add to one
  double invTotalWeight = 1.0 / totalWeight;
  for(int iLocal = 0; iLocal < numModes; iLocal++)
  {
    m_modes[posPixel + iLocal].weight *= (float)invTotalWeight;
    m_modes[posPixel + iLocal].significants = m_modes[posPixel + iLocal].weight 
      / sqrt(m_modes[posPixel + iLocal].variance);
  }

  // Sort significance values so they are in desending order. 
  qsort(&m_modes[posPixel],  numModes, sizeof(GMM), compareT2FGMM);

  // make new mode if needed and exit
  if(!bFitsPDF)
  {
    if(numModes < m_params.MaxModes())
      numModes++;
    //else
      // the weakest mode will be replaced
    
    pos = posPixel + numModes-1;

    m_modes[pos].muR = pixel.ch[0];
    m_modes[pos].muG = pixel.ch[1];
    m_modes[pos].muB = pixel.ch[2];
    m_modes[pos].variance = m_variance;
    m_modes[pos].significants = 0;			// will be set below

    if (numModes == 1)
      m_modes[pos].weight = 1;
    else
      m_modes[pos].weight = m_params.Alpha();

    //renormalize weights
    int iLocal;
    float sum = 0.0;
    for(iLocal = 0; iLocal < numModes; iLocal++)
      sum += m_modes[posPixel+ iLocal].weight;
    
    double invSum = 1.0/sum;
    for(iLocal = 0; iLocal < numModes; iLocal++)
    {
      m_modes[posPixel + iLocal].weight *= (float)invSum;
      m_modes[posPixel + iLocal].significants = m_modes[posPixel + iLocal].weight / sqrt(m_modes[posPixel + iLocal].variance);
    }
  }

  // Sort significance values so they are in desending order. 
  qsort(&(m_modes[posPixel]), numModes, sizeof(GMM), compareT2FGMM);

  if(bBackgroundLow)
    low_threshold = BACKGROUND;
  else
    low_threshold = FOREGROUND;
  
  if(bBackgroundHigh)
    high_threshold = BACKGROUND;
  else
    high_threshold = FOREGROUND;
}

///////////////////////////////////////////////////////////////////////////////
//Input:
//  data - a pointer to the data of a RGB image of the same size
//Output:
//  output - a pointer to the data of a gray value image of the same size 
//					(the memory should already be reserved) 
//					values: 255-foreground, 125-shadow, 0-background
///////////////////////////////////////////////////////////////////////////////
void T2FGMM::Subtract(int frame_num, const RgbImage& data, BwImage& low_threshold_mask, BwImage& high_threshold_mask)
{
  unsigned char low_threshold, high_threshold;
  long posPixel;

  // update each pixel of the image
  for(unsigned int r = 0; r < m_params.Height(); ++r)
  {
    for(unsigned int c = 0; c < m_params.Width(); ++c)
    {		
      // update model + background subtract
      posPixel = (r*m_params.Width() + c) * m_params.MaxModes();

      SubtractPixel(posPixel, data(r,c), m_modes_per_pixel(r,c), low_threshold, high_threshold);

      low_threshold_mask(r,c) = low_threshold;
      high_threshold_mask(r,c) = high_threshold;

      m_background(r,c,0) = (unsigned char) m_modes[posPixel].muR;
      m_background(r,c,1) = (unsigned char) m_modes[posPixel].muG;
      m_background(r,c,2) = (unsigned char) m_modes[posPixel].muB;
    }
  }
}
