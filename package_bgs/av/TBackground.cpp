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
*  TBackground.cpp
*  Framework
*
*  Created by Robinault Lionel on 07/12/11.
*
*/

#include "TBackground.h"

TBackground::TBackground(void)
{
  std::cout << "TBackground()" << std::endl;
}

TBackground::~TBackground(void)
{
  Clear();
  std::cout << "~TBackground()" << std::endl;
}

void TBackground::Clear(void)
{
}

void TBackground::Reset(void)
{
}

int TBackground::GetParameterCount(void)
{
  return 0;
}

std::string TBackground::GetParameterName(int nInd)
{
  return "";
}

std::string TBackground::GetParameterValue(int nInd)
{
  return "";
}

int TBackground::SetParameterValue(int nInd, std::string csNew)
{
  return 0;
}

int TBackground::Init(IplImage * pSource)
{
  return 0;
}

bool TBackground::isInitOk(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask)
{
  bool bResult = true;

  if(pSource == NULL || pSource->nChannels != 1 || pSource->depth != IPL_DEPTH_8U)
    bResult = false;

  if(bResult)
  {
    int nbl, nbc;
    nbl = pSource->height;
    nbc = pSource->width;
    
    if(pBackground == NULL || pBackground->width != nbc || pBackground->height != nbl || pBackground->imageSize != pSource->imageSize)
      bResult = false;
    
    if(pMotionMask == NULL || pMotionMask->width != nbc || pMotionMask->height != nbl || pMotionMask->imageSize != pSource->imageSize)
      bResult = false;
  }

  return bResult;
}

int TBackground::UpdateBackground(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask)
{
  return 0;
}

IplImage *TBackground::CreateTestImg()
{
  IplImage *pImage = cvCreateImage(cvSize(256, 256), IPL_DEPTH_8U, 3);

  if(pImage != NULL)
    cvSetZero(pImage);

  return pImage;
}

int TBackground::UpdateTest(IplImage *pSource, IplImage *pBackground, IplImage *pTest, int nX, int nY, int nInd)
{
  int nErr = 0;
  CvScalar Color;
  unsigned char *ptr;

  if(pTest == NULL || !isInitOk(pSource, pBackground, pSource))
    nErr = 1;

  if(!nErr)
  {
    if(pTest->width != 256 || pTest->height != 256 || pTest->nChannels != 3)
      nErr = 1;

    if(nX < 0 || nX > pSource->width || nY < 0 || nY > pSource->height)
      nErr = 1;

    switch(nInd)
    {
      case 0 : Color = cvScalar(128, 0, 0); break;
      case 1 : Color = cvScalar(0, 128, 0); break;
      case 2 : Color = cvScalar(0, 0, 128); break;
      default : nErr = 1;
    }
  }

  if(!nErr)
  {
    int l, c;
    // recupere l'indice de la colonne
    ptr = (unsigned char *)(pTest->imageData);
    c = *ptr;
    
    // efface la colonne
    cvLine(pTest, cvPoint(c, 0), cvPoint(c, 255), cvScalar(0));
    *ptr += 1;

    //recupere la couleur du fond
    ptr = (unsigned char *)(pBackground->imageData + pBackground->widthStep * nY);
    ptr += nX;
    l = *ptr;

    // dessine la couleur
    cvLine(pTest, cvPoint(c, l - 5), cvPoint(c, l + 5), Color);

    //recupere la couleur du point
    ptr = (unsigned char *)(pSource->imageData + pSource->widthStep * nY);
    ptr += nX;
    l = *ptr;

    // dessine la couleur
    ptr = (unsigned char *)(pTest->imageData + pTest->widthStep * l);
    ptr += (c * 3) + nInd;
    *ptr = 255;
  }

  return nErr;
}
