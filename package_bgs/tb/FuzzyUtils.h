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
#include "PixelUtils.h"

class FuzzyUtils
{
public:
  FuzzyUtils(void);
  ~FuzzyUtils(void);

  void LBP(IplImage* InputImage, IplImage* LBP);
  void getBinValue(float* neighberGrayPixel, float* BinaryValue, int m, int n);

  void SimilarityDegreesImage(IplImage* CurrentImage, IplImage* BGImage, IplImage* DeltaImage, int n, int color_space);
  void RatioPixels(float* CurrentPixel, float* BGPixel, float* DeltaPixel, int n);

  void getFuzzyIntegralSugeno(IplImage* H, IplImage* Delta, int n, float *MeasureG, IplImage* OutputImage);
  void getFuzzyIntegralChoquet(IplImage* H, IplImage* Delta, int n, float *MeasureG, IplImage* OutputImage);
  void FuzzyMeasureG(float g1, float g2, float g3, float *G);
  void Trier(float* g, int n, int* index);		
  float min(float *a, float *b);
  float max(float *g, int n);
  void gDeDeux(float* a, float* b, float* lambda);
  void getLambda(float* g);

  void AdaptativeSelectiveBackgroundModelUpdate(IplImage* CurrentImage, IplImage* BGImage, IplImage* OutputImage, IplImage* Integral, float seuil, float alpha);
};
