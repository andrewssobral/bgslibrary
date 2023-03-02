#pragma once

#include "PixelUtils.h"

namespace bgslibrary
{
  namespace tools
  {
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
      // void getLambda(float* g);

      void AdaptativeSelectiveBackgroundModelUpdate(IplImage* CurrentImage, IplImage* BGImage, IplImage* OutputImage, IplImage* Integral, float seuil, float alpha);
    };
  }
}
