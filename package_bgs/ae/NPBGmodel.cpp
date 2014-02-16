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
*
* Copyright 2001 by Ahmed Elgammal All  rights reserved.
*
* Permission to use, copy,  or modify this software and  its documentation
* for  educational  and  research purposes only and without fee  is hereby
* granted, provided  that this copyright notice and the original authors's
* name appear  on all copies and supporting documentation.  If individual
* files are  separated from  this  distribution directory  structure, this
* copyright notice must be included.  For any other uses of this software,
* in original or  modified form, including but not limited to distribution
* in whole or in  part, specific  prior permission  must be  obtained from
* Author or UMIACS.  These programs shall not  be  used, rewritten, or  
* adapted as  the basis  of  a commercial  software  or  hardware product 
* without first obtaining appropriate licenses  from Author. 
* Other than these cases, no part of this software may be used or
* distributed without written permission of the author.
*
* Neither the author nor UMIACS make any representations about the 
* suitability of this software for any purpose.  It is provided 
* "as is" without express or implied warranty.
*
* Ahmed Elgammal
* 
* University of Maryland at College Park
* UMIACS
* A.V. Williams Bldg. 
* CollegePark, MD 20742
* E-mail:  elgammal@umiacs.umd.edu
*
**/

// NPBGmodel.cpp: implementation of the NPBGmodel class.
//
//////////////////////////////////////////////////////////////////////

#include "NPBGmodel.h"
#include "memory.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NPBGmodel::NPBGmodel()
{
  std::cout << "NPBGmodel()" << std::endl;
}

NPBGmodel::~NPBGmodel()
{
  delete Sequence;
  delete PixelQTop;
  delete TemporalBuffer;
  delete TemporalMask;
  delete AccMask;
  //delete SDbinsImage;
  std::cout << "~NPBGmodel()" << std::endl;
}

NPBGmodel::NPBGmodel(unsigned int Rows,
                     unsigned int Cols,
                     unsigned int ColorChannels,
                     unsigned int Length,
                     unsigned int pTimeWindowSize,
                     unsigned int bg_suppression_time)
{
  std::cout << "NPBGmodel()" << std::endl;

  imagesize = Rows*Cols*ColorChannels;

  rows = Rows;
  cols = Cols;
  color_channels = ColorChannels;

  SampleSize = Length;

  TimeWindowSize = pTimeWindowSize;

  Sequence	= new unsigned char[imagesize*Length];
  Top = 0;
  memset(Sequence,0,imagesize*Length);

  PixelQTop = new unsigned char[rows*cols];

  // temporalBuffer
  TemporalBufferLength = (TimeWindowSize/Length > 2 ? TimeWindowSize/Length:2);
  TemporalBuffer = new unsigned char[imagesize*TemporalBufferLength];
  TemporalMask = new unsigned char[rows*cols*TemporalBufferLength];

  TemporalBufferTop = 0;

  AccMask = new unsigned int[rows*cols];

  ResetMaskTh = bg_suppression_time;
}

void NPBGmodel::AddFrame(unsigned char *ImageBuffer)		
{
  memcpy(Sequence+Top*imagesize,ImageBuffer,imagesize);
  Top = (Top + 1) % SampleSize;		

  memset(PixelQTop, (unsigned char) Top, rows*cols);

  memcpy(TemporalBuffer,ImageBuffer,imagesize);
}
