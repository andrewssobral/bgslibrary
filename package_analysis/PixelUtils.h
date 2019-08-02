#pragma once

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

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
