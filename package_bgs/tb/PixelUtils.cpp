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
#include "PixelUtils.h"

PixelUtils::PixelUtils(void){}
PixelUtils::~PixelUtils(void){}

void PixelUtils::ColorConversion(IplImage* RGBImage, IplImage* ConvertedImage, int color_space)
{
  // Space Color RGB - Nothing to do!
  if(color_space == 1)
    cvCopy(RGBImage, ConvertedImage);

  // Space Color Ohta
  if(color_space == 2) 
    cvttoOTHA(RGBImage, ConvertedImage);

  // Space Color HSV - V Intensity - (H,S) Chromaticity
  if(color_space == 3)
    cvCvtColor(RGBImage, ConvertedImage, CV_BGR2HSV);

  // Space Color YCrCb - Y Intensity - (Cr,Cb) Chromaticity
  if(color_space == 4)
    cvCvtColor(RGBImage,ConvertedImage,CV_BGR2YCrCb);
}

void PixelUtils::cvttoOTHA(IplImage* RGBImage, IplImage* OthaImage)
{
  float* OhtaPixel = (float*) malloc(3*(sizeof(float)));
  float* RGBPixel = (float*) malloc(3*(sizeof(float)));

  for(int i = 0; i < RGBImage->width; i++)
  {
    for(int j = 0;j < RGBImage->height; j++)
    {
      GetPixel(RGBImage, i, j, RGBPixel);

      // I1 = (R + G + B) / 3
      *OhtaPixel = (*(RGBPixel) + (*(RGBPixel + 1)) + (*(RGBPixel + 2))) / 3.0;

      // I2 = (R - B) / 2
      *(OhtaPixel+1) = (*RGBPixel - (*(RGBPixel + 2))) / 2.0;

      // I3 = (2G - R - B) / 4
      *(OhtaPixel+2) = (2 * (*(RGBPixel + 1)) - (*RGBPixel) - (*(RGBPixel + 2))) / 4.0;		
      
      PutPixel(OthaImage, i, j, OhtaPixel);
    }
  }

  free(OhtaPixel);
  free(RGBPixel);
}

void PixelUtils::PostProcessing(IplImage *InputImage)
{
  IplImage *ResultImage = cvCreateImage(cvSize(InputImage->width, InputImage->height), IPL_DEPTH_32F, 3);

  cvErode(InputImage, ResultImage, NULL, 1);
  cvDilate(ResultImage, InputImage, NULL, 0);

  cvReleaseImage(&ResultImage);
}	

void PixelUtils::GetPixel(IplImage *image, int m, int n, unsigned char *pixelcourant)
{	
  for(int k = 0; k < 3; k++)
    pixelcourant[k] = ((unsigned char*)(image->imageData + image->widthStep*n))[m*3 + k];
}

void PixelUtils::GetGrayPixel(IplImage *image, int m, int n, unsigned char *pixelcourant)
{	
  *pixelcourant = ((unsigned char*)(image->imageData + image->widthStep*n))[m];
}

void PixelUtils::PutPixel(IplImage *image,int p,int q,unsigned char *pixelcourant)
{	
  for(int r = 0; r < 3; r++)
    ((unsigned char*)(image->imageData + image->widthStep*q))[p*3 + r] = pixelcourant[r];
}

void PixelUtils::PutGrayPixel(IplImage *image, int p, int q, unsigned char pixelcourant)
{	
  ((unsigned char*)(image->imageData + image->widthStep*q))[p] = pixelcourant; 
}

void PixelUtils::GetPixel(IplImage *image, int m, int n, float *pixelcourant)
{	
  for(int k = 0; k < 3; k++)
    pixelcourant[k] = ((float*)(image->imageData + image->widthStep*n))[m*3 + k];
}

void PixelUtils::GetGrayPixel(IplImage *image, int m, int n, float *pixelcourant)
{	
  *pixelcourant = ((float*)(image->imageData + image->widthStep*n))[m];
}

void PixelUtils::PutPixel(IplImage *image, int p, int q, float *pixelcourant)
{	
  for(int r = 0; r < 3; r++)
    ((float*)(image->imageData + image->widthStep*q))[p*3 + r] = pixelcourant[r];
}

void PixelUtils::PutGrayPixel(IplImage *image,int p,int q,float pixelcourant)
{	
  ((float*)(image->imageData + image->widthStep*q))[p] = pixelcourant;
}

void PixelUtils::getNeighberhoodGrayPixel(IplImage* InputImage, int x, int y, float* neighberPixel)
{
  int i,j,k;
  
  float* pixelCourant = (float*) malloc(1*(sizeof(float)));
  
  //le calcul de voisinage pour les 4 coins;
  /* 1.*/
  if(x==0 && y==0)
  {
    k = 0;
    for(i = x; i < x+2; i++)
      for(j = y; j < y+2; j++)
      {  
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }

  /* 2.*/
  if(x==0 && y==InputImage->width)
  {
    k = 0;
    for(i = x; i < x+2; i++)
      for(j = y-1; j < y+1; j++)
      {  
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }

  /* 3.*/
  if(x==InputImage->height && y==0)
  {
    k = 0;
    for(i = x-1; i < x+1; i++)
      for(j = y; j < y+2; j++)
      {  
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }	

  /* 4.*/
  if(x==InputImage->height && y==InputImage->width)
  {
    k = 0;
    for(i = x-1; i <x+1; i++)
      for(j = y-1; j < y+1; j++)
      {  
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }	

  // Voisinage de la premiere ligne : L(0) 
  if(x==0 && (y!=0 && y!=InputImage->width)) 
  {
    k = 0;
    for(i = x+1; i >= x; i--)
      for(j = y-1; j < y+2; j++)
      {
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }

  // Voisinage de la dernière colonne : C(w)
  if((x!=0 && x!=InputImage->height) && y==InputImage->width) 
  {
    k = 0;
    for(i = x+1; i > x-2; i--)
      for(j = y-1; j < y+1; j++)
      {  
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }

  // Voisinage de la dernière ligne : L(h)    
  if(x==InputImage->height && (y!=0 && y!=InputImage->width)) 
  {
    k = 0;
    for(i = x; i > x-2; i--)
      for(j = y-1; j < y+2; j++)
      {  
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }

  // Voisinage de la premiere colonne : C(0) 
  if((x!=0 && x!=InputImage->height) && y==0) 
  {
    k = 0;
    for(i = x-1; i < x+2; i++)
      for(j = y; j < y+2; j++)
      {  
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }  

  //le calcul du voisinage pour le reste des elementes d'image
  if((x!=0 && x!=InputImage->height)&&(y!=0 && y!=InputImage->width))
  {
    k = 0; 
    for(i = x+1;i > x-2; i--)
      for(j = y-1; j < y+2; j++)
      {  
        GetGrayPixel(InputImage,i,j,pixelCourant);
        *(neighberPixel+k) = *pixelCourant;
        k++;
      }
  }

  free(pixelCourant);
}

void PixelUtils::ForegroundMinimum(IplImage *Foreground, float *Minimum, int n)
{
  int i,j,k;
  float *pixelcourant;
  
  pixelcourant = (float *) malloc(n*sizeof(float));
  
  for(k = 0; k < n; k++)
    *(Minimum + k) = 255;

  for(i = 0; i < Foreground->width; i++)
    for(j = 0; j < Foreground->height; j++)
    {
      if(n == 3)
      {
        GetPixel(Foreground,i,j,pixelcourant);

        for(k = 0; k < n; k++)
          if(*(pixelcourant + k) < *(Minimum + k))
            *(Minimum + k) = *(pixelcourant + k);
      }

      if(n==1)
      {
        GetGrayPixel(Foreground,i,j,pixelcourant);

        if(*pixelcourant < *Minimum)
          *Minimum = *pixelcourant;
      }
    }

    free(pixelcourant);
}

void PixelUtils::ForegroundMaximum(IplImage *Foreground, float *Maximum, int n)
{
  int i,j,k;
  float *pixelcourant;
  
  pixelcourant = (float *) malloc(n*sizeof(float));
  
  for(k = 0; k < n; k++)
    *(Maximum + k) = 0;
  
  for(i = 0; i < Foreground->width; i++)
    for(j = 0; j < Foreground->height; j++)
    {
      if(n == 3)
      {
        GetPixel(Foreground,i,j,pixelcourant);

        for(k = 0; k < n; k++)
          if(*(pixelcourant + k) > *(Maximum + k))
            *(Maximum + k) = *(pixelcourant + k);
      }

      if(n == 1)
      {
        GetGrayPixel(Foreground,i,j,pixelcourant);

        if(*pixelcourant > *Maximum)
          *Maximum = *pixelcourant;
      }
    }

    free(pixelcourant);
}

void PixelUtils::ComplementaryAlphaImageCreation(IplImage *AlphaImage, IplImage *ComplementaryAlphaImage, int n)
{
  int i,j,k;
  float *pixelcourant, *pixelcourant1;
  
  pixelcourant = (float *) malloc(n * sizeof(float));
  pixelcourant1 = (float *) malloc(n * sizeof(float));

  for(i = 0; i < AlphaImage->width; i++)
    for(j = 0; j < AlphaImage->height; j++)
    {
      if(n == 1)
      {
        GetGrayPixel(AlphaImage,i,j,pixelcourant);
        *pixelcourant1 = 1 - *(pixelcourant);
        PutGrayPixel(ComplementaryAlphaImage,i,j,*pixelcourant1);
      }

      if(n == 3)
      {
        GetPixel(AlphaImage,i,j,pixelcourant);
        for(k = 0; k < 3; k++)
        {
          *pixelcourant1 = 1.0 - *(pixelcourant);
          *(pixelcourant1+1) = 1.0 - *(pixelcourant+1);
          *(pixelcourant1+2) = 1.0 - *(pixelcourant+2);
        }
        PutPixel(ComplementaryAlphaImage,i,j,pixelcourant1);
      }
    }

    free(pixelcourant);
    free(pixelcourant1);
}
