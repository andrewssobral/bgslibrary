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
/*
*  TBackgroundVuMeter.cpp
*  Framework
*
*  Created by Robinault Lionel on 07/12/11.
*
*/
#include "TBackgroundVuMeter.h"

#define PROCESS_PAR_COUNT 3

TBackgroundVuMeter::TBackgroundVuMeter(void)
  : m_pHist(NULL)
  , m_nBinCount(0)
  , m_nBinSize(8)
  , m_nCount(0)
  , m_fAlpha(0.995)
  , m_fThreshold(0.03)
{
  std::cout << "TBackgroundVuMeter()" << std::endl;
}

TBackgroundVuMeter::~TBackgroundVuMeter(void)
{
  Clear();
  std::cout << "~TBackgroundVuMeter()" << std::endl;
}

void TBackgroundVuMeter::Clear(void)
{
  TBackground::Clear();

  if(m_pHist != NULL)
  {
    for(int i = 0; i < m_nBinCount; ++i)
    {
      if(m_pHist[i] != NULL)
        cvReleaseImage(&m_pHist[i]);
    }

    delete[] m_pHist;
    m_pHist = NULL;
    m_nBinCount = 0;
  }

  m_nCount = 0;
}

void TBackgroundVuMeter::Reset(void)
{
  float fVal = 0.0;

  TBackground::Reset();

  if(m_pHist != NULL)
  {
    //		fVal = (m_nBinCount != 0) ? (float)(1.0 / (double)m_nBinCount) : (float)0.0;
    fVal = 0.0;

    for(int i = 0; i < m_nBinCount; ++i)
    {
      if(m_pHist[i] != NULL)
      {
        cvSetZero(m_pHist[i]);
        cvAddS(m_pHist[i], cvScalar(fVal), m_pHist[i]);
      }
    }
  }

  m_nCount = 0;
}

int TBackgroundVuMeter::GetParameterCount(void)
{
  return TBackground::GetParameterCount() + PROCESS_PAR_COUNT;
}

std::string TBackgroundVuMeter::GetParameterName(int nInd)
{
  std::string csResult;
  int nNb;

  nNb = TBackground::GetParameterCount();

  if(nInd >= nNb)
  {
    nInd -= nNb;

    switch(nInd)
    {
    case 0 : csResult = "Bin size"; break;
    case 1 : csResult = "Alpha"; break;
    case 2 : csResult = "Threshold"; break;
    }
  }
  else 
    csResult = TBackground::GetParameterName(nInd);

  return csResult;
}

std::string TBackgroundVuMeter::GetParameterValue(int nInd)
{
  std::string csResult;
  int nNb;

  nNb = TBackground::GetParameterCount();

  if(nInd >= nNb)
  {
    nInd -= nNb;

    char buff[100];
    
    switch(nInd)
    {
      case 0 : sprintf(buff, "%d", m_nBinSize); break;
      case 1 : sprintf(buff, "%.3f", m_fAlpha); break;
      case 2 : sprintf(buff, "%.2f", m_fThreshold); break;
    }

    csResult = buff;
  }
  else 
    csResult = TBackground::GetParameterValue(nInd);

  return csResult;
}

int TBackgroundVuMeter::SetParameterValue(int nInd, std::string csNew)
{
  int nErr = 0;

  int nNb;

  nNb = TBackground::GetParameterCount();

  if(nInd >= nNb)
  {
    nInd -= nNb;

    switch(nInd)
    {
      case 0 : SetBinSize(atoi(csNew.c_str())); break;
      case 1 : SetAlpha(atof(csNew.c_str())); break;
      case 2 : SetThreshold(atof(csNew.c_str())); break;
      default : nErr = 1;
    }
  }
  else 
    nErr = TBackground::SetParameterValue(nInd, csNew);

  return nErr;
}

int TBackgroundVuMeter::Init(IplImage * pSource)
{
  int nErr = 0;
  int nbl, nbc;

  Clear();

  nErr = TBackground::Init(pSource);

  if(pSource == NULL)
    nErr = 1;

  // calcul le nb de bin
  if(!nErr)
  {
    nbl = pSource->height;
    nbc = pSource->width;
    m_nBinCount = (m_nBinSize != 0) ? 256 / m_nBinSize : 0;

    if(m_nBinCount <= 0 || m_nBinCount > 256)
      nErr = 1;
  }

  // creation du tableau de pointeur
  if(!nErr)
  {
    m_pHist = new IplImage *[m_nBinCount];

    if(m_pHist == NULL)
      nErr = 1;
  }

  // creation des images
  if(!nErr)
  {
    for(int i = 0; i < m_nBinCount; ++i)
    {
      m_pHist[i] = cvCreateImage(cvSize(nbc, nbl), IPL_DEPTH_32F, 1);

      if(m_pHist[i] == NULL)
        nErr = 1;
    }
  }

  if(!nErr) 
    Reset();
  else
    Clear();

  return nErr;
}

bool TBackgroundVuMeter::isInitOk(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask)
{
  bool bResult = true;
  int i;

  bResult = TBackground::isInitOk(pSource, pBackground, pMotionMask);

  if(pSource == NULL)
    bResult = false;

  if(m_nBinSize == 0)
    bResult = false;

  if(bResult)
  {
    i = (m_nBinSize != 0) ? 256 / m_nBinSize : 0;

    if(i != m_nBinCount || m_pHist == NULL)
      bResult = false;
  }

  if(bResult)
  {
    int nbl = pSource->height;
    int nbc = pSource->width;

    for(i = 0; i < m_nBinCount; ++i)
    {
      if(m_pHist[i] == NULL || m_pHist[i]->width != nbc || m_pHist[i]->height != nbl)
        bResult = false;
    }
  }

  return bResult;
}

int TBackgroundVuMeter::UpdateBackground(IplImage *pSource, IplImage *pBackground, IplImage *pMotionMask)
{
  int nErr = 0;
  unsigned char *ptrs, *ptrb, *ptrm;
  float *ptr1, *ptr2;

  if(!isInitOk(pSource, pBackground, pMotionMask))
    nErr = Init(pSource);
  
  if(!nErr)
  {
    m_nCount++;
    int nbc = pSource->width;
    int nbl = pSource->height;
    unsigned char v = m_nBinSize;

    // multiplie tout par alpha
    for(int i = 0; i < m_nBinCount; ++i)
      cvConvertScale(m_pHist[i], m_pHist[i], m_fAlpha, 0.0);

    for(int l = 0; l < nbl; ++l)
    {
      ptrs = (unsigned char *)(pSource->imageData + pSource->widthStep * l);
      ptrm = (unsigned char *)(pMotionMask->imageData + pMotionMask->widthStep * l);
      ptrb = (unsigned char *)(pBackground->imageData + pBackground->widthStep * l);

      for(int c = 0; c < nbc; ++c, ptrs++, ptrb++, ptrm++)
      {
        // recherche le bin à augmenter
        int i = *ptrs / v;
        
        if(i < 0 || i >= m_nBinCount)
          i = 0;
        
        ptr1 = (float *)(m_pHist[i]->imageData + m_pHist[i]->widthStep * l);
        ptr1 += c;

        *ptr1 += (float)(1.0 - m_fAlpha);
        *ptrm = (*ptr1 < m_fThreshold) ? 255 : 0;

        // recherche le bin du fond actuel
        i = *ptrb / v;
        
        if(i < 0 || i >= m_nBinCount)
          i = 0;
        
        ptr2 = (float *)(m_pHist[i]->imageData + m_pHist[i]->widthStep * l);
        ptr2 += c;

        if(*ptr2 < *ptr1)
          *ptrb = *ptrs;
      }
    }

    if(m_nCount < 5)
      cvSetZero(pMotionMask);
  }

  return nErr;
}

IplImage *TBackgroundVuMeter::CreateTestImg()
{
  IplImage *pImage = NULL;

  if(m_nBinCount > 0)
    pImage = cvCreateImage(cvSize(m_nBinCount, 100), IPL_DEPTH_8U, 3);

  if(pImage != NULL)
    cvSetZero(pImage);

  return pImage;
}

int TBackgroundVuMeter::UpdateTest(IplImage *pSource, IplImage *pBackground, IplImage *pTest, int nX, int nY, int nInd)
{
  int nErr = 0;
  float *ptrf;

  if(pTest == NULL || !isInitOk(pSource, pBackground, pSource)) 
    nErr = 1;

  if(!nErr)
  {
    int nbl = pTest->height;
    int nbc = pTest->width;

    if(nbl != 100 || nbc != m_nBinCount) 
      nErr = 1;

    if(nX < 0 || nX >= pSource->width || nY < 0 || nY >= pSource->height)
      nErr = 1;
  }

  if(!nErr)
  {
    cvSetZero(pTest);

    for(int i = 0; i < m_nBinCount; ++i)
    {
      ptrf = (float *)(m_pHist[i]->imageData + m_pHist[i]->widthStep * nY);
      ptrf += nX;

      if(*ptrf >= 0 || *ptrf <= 1.0) {
        cvLine(pTest, cvPoint(i, 100), cvPoint(i, (int)(100.0 * (1.0 - *ptrf))), cvScalar(0, 255, 0));
      }
    }

    cvLine(pTest, cvPoint(0, (int)(100.0 * (1.0 - m_fThreshold))), cvPoint(m_nBinCount, (int)(100.0 * (1.0 - m_fThreshold))), cvScalar(0, 128, 0));
  }

  return nErr;
}
