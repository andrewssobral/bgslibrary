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
/* --- --- ---
* Copyright (C) 2008--2010 Idiap Research Institute (.....@idiap.ch)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// OpencvDataConversion.h: interface for the COpencvDataConversion class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_OPENCV_DATA_CONVERSION_H_)
#define _OPENCV_DATA_CONVERSION_H_

#include <opencv2/opencv.hpp>
#include <stdio.h>

template <class TI, class TM>		/* class TI - the type of image data, class TM - the type of matrix data */
class COpencvDataConversion
{
public:

  /* get the image data */
  TI * GetImageData(IplImage *img)
  {
    if (!img->roi) {	/* no ROI used, i.e. the whole image */
      int y; //, x;
      TI* img_data = new TI[img->width*img->height*img->nChannels];
      TI* temp = img_data;
      TI* x_data;

      for (y = 0; y < img->height; y++) {
        x_data = (TI*)(img->imageData + img->widthStep*y);
        int row_length = img->width*img->nChannels;
        memcpy(temp, x_data, sizeof(TI)*row_length);
        temp += row_length;
        /*
        for ( x = 0 ; x < img->width*img->nChannels ; x++ )
        *temp++ = *x_data++;
        */
      }

      return img_data;
    }
    else {	/* get image data only in ROI */
      int y;//, x;
      TI* img_data = new TI[img->roi->width*img->roi->height*img->nChannels];
      TI* temp = img_data;
      TI* x_data;
      for (y = img->roi->yOffset; y < img->roi->yOffset + img->roi->height; y++) {
        x_data = (TI*)(img->imageData + img->widthStep*y + img->roi->xOffset*sizeof(TI)*img->nChannels);
        int row_length = img->roi->width*img->nChannels;
        memcpy(temp, x_data, sizeof(TI)*row_length);
        temp += row_length;
        /*
        for ( x = 0 ; x < img->roi->width*img->nChannels ; x++ )
        *temp++ = *x_data++;
        */
      }
      return img_data;
    }
  };

  /* set the image data */
  void SetImageData(IplImage *img, TI *img_data)
  {
    if (!img->roi) {	/* no ROI used, i.e. the whole image */
      int y;//, x;
      TI* temp = img_data;
      TI* x_data;
      for (y = 0; y < img->height; y++) {
        x_data = (TI*)(img->imageData + img->widthStep*y);
        int row_length = img->width*img->nChannels;
        memcpy(x_data, temp, sizeof(TI)*row_length);
        temp += row_length;
        /*
        for ( x = 0 ; x < img->width*img->nChannels ; x++ )
        *x_data++ = *temp++;
        */
      }
    }
    else {	/* set image data only in ROI */
      int y;//, x;
      TI* temp = img_data;
      TI* x_data;
      for (y = img->roi->yOffset; y < img->roi->yOffset + img->roi->height; y++) {
        x_data = (TI*)(img->imageData + img->widthStep*y + img->roi->xOffset*sizeof(TI)*img->nChannels);
        int row_length = img->roi->width*img->nChannels;
        memcpy(x_data, temp, sizeof(TI)*row_length);
        temp += row_length;
        /*
        for ( x = 0 ; x < img->roi->width*img->nChannels ; x++ )
        *x_data++ = *temp++;
        */
      }
    }
  }

  /* get the matrix data */
  TM * GetMatData(CvMat *mat)
  {
    TM* mat_data = new TM[mat->width*mat->height];
    memcpy(mat_data, mat->data.ptr, sizeof(TM)*mat->width*mat->height);
    return mat_data;

    /*
    int y, x;
    TM* mat_data = new TM[mat->width*mat->height];
    TM* temp = mat_data;
    TM* x_data;
    for ( y = 0 ; y < mat->height ; y++ ) {
    x_data = (TM*)(mat->data.ptr + mat->step*y);
    for ( x = 0 ; x < mat->width ; x++ )
    *temp++ = *x_data++;
    }
    return mat_data;
    */
  };

  /* set the matrix data */
  void SetMatData(CvMat *mat, TM *mat_data)
  {
    memcpy(mat->data.ptr, mat_data, sizeof(TM)*mat->width*mat->height);

    /*
    int y, x;
    TM* temp = mat_data;
    TM* x_data;
    for ( y = 0 ; y < mat->height ; y++ ) {
    x_data = (TM*)(mat->data.ptr + mat->step*y);
    for ( x = 0 ; x < mat->width ; x++ )
    *x_data++ = *temp++;
    }
    */
  }

  /* convert the image data to the matrix data */
  void ConvertData(IplImage *img_src, CvMat *mat_dst)
  {
    if (img_src->nChannels > 1) {
      printf("Must be one-channel image for ConvertImageData!\n");
      exit(1);
    }

    TI* _img_data = GetImageData(img_src);
    TM* _mat_data = new TM[img_src->width*img_src->height];

    TI* img_data = _img_data;
    TM* mat_data = _mat_data;
    int i;
    for (i = 0; i < img_src->width*img_src->height; i++)
      *mat_data++ = (TM)(*img_data++);

    SetMatData(mat_dst, _mat_data);

    delete[] _img_data;
    delete[] _mat_data;
  }

  /* convert the matrix data to the image data */
  void ConvertData(CvMat *mat_src, IplImage *img_dst)
  {
    if (img_dst->nChannels > 1) {
      printf("Must be one-channel image for ConvertImageData!\n");
      exit(1);
    }

    TM* _mat_data = GetMatData(mat_src);
    TI* _img_data = new TI[mat_src->width*mat_src->height];

    TM* mat_data = _mat_data;
    TI* img_data = _img_data;

    int i;
    for (i = 0; i < mat_src->width*mat_src->height; i++)
      *img_data++ = (TI)(*mat_data++);

    SetImageData(img_dst, _img_data);

    delete[] _img_data;
    delete[] _mat_data;
  }

  COpencvDataConversion() {};
  virtual ~COpencvDataConversion() {};
};

#endif // !defined(_OPENCV_DATA_CONVERSION_H_)

