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
* Image.hpp
*
* Purpose:  C++ wrapper for OpenCV IplImage which supports simple and 
*						efficient access to the image data
*
* Author: Donovan Parks, September 2007
*
* Based on code from: 
*  http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.hpptml
******************************************************************************/

#include "Image.h"

ImageBase::~ImageBase()
{ 
  if(imgp != NULL && m_bReleaseMemory)
    cvReleaseImage(&imgp);
  imgp = NULL;	
}

void DensityFilter(BwImage& image, BwImage& filtered, int minDensity, unsigned char fgValue)
{
  for(int r = 1; r < image.Ptr()->height-1; ++r)
  {
    for(int c = 1; c < image.Ptr()->width-1; ++c)
    {
      int count = 0;
      if(image(r,c) == fgValue)
      {
        if(image(r-1,c-1) == fgValue)
          count++;
        if(image(r-1,c) == fgValue)
          count++;
        if(image(r-1,c+1) == fgValue)
          count++;
        if(image(r,c-1) == fgValue)
          count++;
        if(image(r,c+1) == fgValue)
          count++;
        if(image(r+1,c-1) == fgValue)
          count++;
        if(image(r+1,c) == fgValue)
          count++;
        if(image(r+1,c+1) == fgValue)
          count++;

        if(count < minDensity)
          filtered(r,c) = 0;
        else
          filtered(r,c) = fgValue;
      }
      else
      {
        filtered(r,c) = 0;
      }
    }
  }
}