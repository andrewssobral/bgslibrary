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

// NPBGmodel.h: interface for the NPBGmodel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPBGMODEL_H__CCAF05D4_D06E_44C2_95D8_979E2249953A__INCLUDED_)
#define AFX_NPBGMODEL_H__CCAF05D4_D06E_44C2_95D8_979E2249953A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>

class NPBGmodel  
{
private:
  unsigned char *Sequence;
  unsigned int SampleSize;
  unsigned int TimeWindowSize;

  unsigned int rows,cols,color_channels;
  unsigned int imagesize;

  unsigned int Top;
  unsigned char *PixelQTop;

  //unsigned int *PixelUpdateCounter;

  unsigned char *SDbinsImage; 

  unsigned char *TemporalBuffer;
  unsigned char TemporalBufferLength;
  unsigned char TemporalBufferTop;
  unsigned char *TemporalBufferMask;

  unsigned char *TemporalMask;
  unsigned char TemporalMaskLength;
  unsigned char TemporalMaskTop;

  unsigned int *AccMask;
  unsigned int ResetMaskTh;	// Max continous duration a pixel can be detected before
  // it is forced to be updated...

  double *weights;

public:
  NPBGmodel();
  //~NPBGmodel();
  virtual ~NPBGmodel();

  NPBGmodel(unsigned int Rows,
    unsigned int Cols,
    unsigned int ColorChannels,
    unsigned int Length,
    unsigned int pTimeWindowSize,
    unsigned int bg_suppression_time);

  void AddFrame(unsigned char *ImageBuffer);

  friend class NPBGSubtractor;
};

#endif // !defined(AFX_NPBGMODEL_H__CCAF05D4_D06E_44C2_95D8_979E2249953A__INCLUDED_)
