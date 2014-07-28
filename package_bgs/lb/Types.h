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
/*  Scene 1.0.1 -- Background subtraction and object tracking for complex environments  
Types.h

Copyright (C) 2011 Laurence Bender <lbender@untref.edu.ar>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef TYPES_H
#define TYPES_H

#include <opencv2/opencv.hpp>

namespace lb_library
{
  template<class T> class Image
  {
  private:
    IplImage* imgp;

  public:
    Image(IplImage* img=0) {imgp=img;}
    ~Image(){imgp=0;}
  
    void operator=(IplImage* img) {imgp=img;}
  
    inline T* operator[](const int rowIndx)
    {
      return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));
    }
  };

  typedef struct{
    unsigned char b,g,r;
  } RgbPixel;

  typedef struct{
    unsigned char Blue,Green,Red;
  } BYTERGB;

  typedef struct{
    unsigned int Blue,Green,Red;
  } INTRGB;

  typedef struct{
    float b,g,r;
  }RgbPixelFloat;

  typedef struct{
    double Blue,Green,Red;
  } DBLRGB;

  typedef Image<RgbPixel>       RgbImage;
  typedef Image<RgbPixelFloat>  RgbImageFloat;
  typedef Image<unsigned char>  BwImage;
  typedef Image<float>          BwImageFloat;

  /*
  IplImage* img = cvCreateImage(cvSize(640,480), IPL_DEPTH_32F, 3);
  RgbImageFloat imgA(img);
  for(int i = 0; i < m_height; i++)
    for(int j = 0; j < m_width; j++)
      imgA[i][j].b = 111;
      imgA[i][j].g = 111;
      imgA[i][j].r = 111;
  */
}

//---------------------------------------------

#endif
