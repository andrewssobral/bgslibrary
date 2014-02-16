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

#include "KernelTable.h"
#include <math.h>

#define PI 3.14159

KernelLUTable::KernelLUTable()
{
  std::cout << "KernelLUTable()" << std::endl;
}

KernelLUTable::~KernelLUTable()
{
  delete kerneltable;
  delete kernelsums;
  std::cout << "~KernelLUTable()" << std::endl;
}

KernelLUTable::KernelLUTable(int KernelHalfWidth, double Segmamin, double Segmamax, int Segmabins)
{
  std::cout << "KernelLUTable()" << std::endl;

  double C1,C2,v,segma,sum;
  int bin,b;

  minsegma = Segmamin;
  maxsegma = Segmamax;
  segmabins = Segmabins;
  tablehalfwidth = KernelHalfWidth;

  // Generate the Kernel

  // allocate memory for the Kernal Table
  kerneltable = new double[segmabins*(2*KernelHalfWidth+1)];
  kernelsums = new double[segmabins];

  double segmastep = (maxsegma - minsegma) / segmabins;
  double y;

  for(segma = minsegma, bin = 0; bin < segmabins; segma += segmastep, bin++) 
  {
    C1 = 1/(sqrt(2*PI)*segma);
    C2 = -1/(2*segma*segma);

    b = (2*KernelHalfWidth+1)*bin;
    sum = 0;
    
    for(int x = 0; x <= KernelHalfWidth; x++)
    {
      y = x/1.0;
      v = C1*exp(C2*y*y);
      kerneltable[b+KernelHalfWidth+x]=v;
      kerneltable[b+KernelHalfWidth-x]=v;
      sum += 2*v;
    }

    sum -= C1;

    kernelsums[bin] = sum;

    // Normailization
    for(int x = 0; x <= KernelHalfWidth; x++)
    {
      v = kerneltable[b+KernelHalfWidth+x] / sum;
      kerneltable[b+KernelHalfWidth+x]=v;
      kerneltable[b+KernelHalfWidth-x]=v;
    }
  }
}
