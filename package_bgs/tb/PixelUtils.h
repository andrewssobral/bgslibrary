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
#pragma once
/*
Code provided by Thierry BOUWMANS

Maitre de Conférences
Laboratoire MIA
Université de La Rochelle
17000 La Rochelle
France
tbouwman@univ-lr.fr

http://sites.google.com/site/thierrybouwmans/
*/
#include <stdio.h>
#include <opencv2/opencv.hpp>


class PixelUtils
{
public:
  PixelUtils(void);
  ~PixelUtils(void);

  void ColorConversion(IplImage* RGBImage, IplImage* ConvertedImage, int color_space);
  void cvttoOTHA(IplImage* RGBImage, IplImage* OthaImage);
  
  void PostProcessing(IplImage *InputImage);
  
  void GetPixel(IplImage *image, int m, int n, unsigned char *pixelcourant);
  void GetGrayPixel(IplImage *image, int m, int n, unsigned char *pixelcourant);

  void PutPixel(IplImage *image, int p, int q, unsigned char *pixelcourant);
  void PutGrayPixel(IplImage *image, int p, int q, unsigned char pixelcourant);

  void GetPixel(IplImage *image, int m, int n, float *pixelcourant);
  void GetGrayPixel(IplImage *image, int m, int n, float *pixelcourant);

  void PutPixel(IplImage *image, int p, int q, float *pixelcourant);
  void PutGrayPixel(IplImage *image, int p, int q, float pixelcourant);

  void getNeighberhoodGrayPixel(IplImage* InputImage, int x, int y, float* neighberPixel);
  void ForegroundMaximum(IplImage *Foreground, float *Maximum, int n);
  void ForegroundMinimum(IplImage *Foreground, float *Minimum, int n);
  void ComplementaryAlphaImageCreation(IplImage *AlphaImage, IplImage *ComplementaryAlphaImage, int n);
};