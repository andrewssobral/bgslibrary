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
#include "FuzzyUtils.h"

FuzzyUtils::FuzzyUtils(void){}

FuzzyUtils::~FuzzyUtils(void){}

void FuzzyUtils::LBP(IplImage* InputImage, IplImage* LBPimage)
{
  PixelUtils p;

  float* neighberPixel = (float*) malloc(9*sizeof(float));   
  float* BinaryValue = (float*) malloc(9*sizeof(float));
  float* CarreExp = (float*) malloc(9*sizeof(float));
  float* valLBP = (float*) malloc(1*sizeof(float));

  *valLBP = 0;

  int x = 0, y = 0;

  // on implemente les 8 valeurs puissance de 2 qui correspondent aux 8 elem. d'image voisins au elem. d'image central
  *(CarreExp+0)=1.0;
  *(CarreExp+1)=2.0;
  *(CarreExp+2)=4.0;
  *(CarreExp+3)=8.0;
  *(CarreExp+4)=0.0;
  *(CarreExp+5)=16.0;
  *(CarreExp+6)=32.0;
  *(CarreExp+7)=64.0;
  *(CarreExp+8)=128.0;

  //le calcule de LBP
  //pour les 4 coins
  /* 1.*/
  if(x==0 && y==0)
  {
    p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
    getBinValue(neighberPixel,BinaryValue,4,0);
    *valLBP=*valLBP+((*(BinaryValue+1))*(*(CarreExp+1))+(*(BinaryValue+2))*(*(CarreExp+2))+(*(BinaryValue+3))*(*(CarreExp+3)))/255.0;
    p.PutGrayPixel(LBPimage,x,y,*valLBP);	
  }

  /* 2.*/
  if(x==0 && y==InputImage->width)
  {
    *valLBP=0;
    p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
    getBinValue(neighberPixel,BinaryValue,4,1);
    *valLBP=*valLBP+((*(BinaryValue))*(*(CarreExp))+(*(BinaryValue+2))*(*(CarreExp+2))+(*(BinaryValue+3))*(*(CarreExp+3)))/255.0;
    p.PutGrayPixel(LBPimage,x,y,*valLBP);	
  }

  /* 3.*/
  if(x==InputImage->height && y==0)
  {
    *valLBP=0;
    p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
    getBinValue(neighberPixel,BinaryValue,4,2);
    *valLBP=*valLBP+((*(BinaryValue))*(*(CarreExp))+(*(BinaryValue+1))*(*(CarreExp+1))+(*(BinaryValue+3))*(*(CarreExp+3)))/255.0;
    p.PutGrayPixel(LBPimage,x,y,*valLBP);	
  }

  /* 4.*/
  if(x==InputImage->height && y==InputImage->width)
  {
    *valLBP=0;
    p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
    getBinValue(neighberPixel,BinaryValue,4,3);
    *valLBP=*valLBP+((*(BinaryValue))*(*(CarreExp))+(*(BinaryValue+1))*(*(CarreExp+1))+(*(BinaryValue+2))*(*(CarreExp+2)))/255.0;
    p.PutGrayPixel(LBPimage,x,y,*valLBP);	
  }

  //le calcul de LBP pour la première ligne : L(0)
  if(x==0 && (y!=0 && y!=InputImage->width))
  {
    for(int y = 1; y < InputImage->width-1; y++)
    {
      p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
      getBinValue(neighberPixel,BinaryValue,6,4);
      *valLBP=0;
      *valLBP=*valLBP+((*(BinaryValue))*(*(CarreExp))+(*(BinaryValue+1))*(*(CarreExp+1))+(*(BinaryValue+2))*(*(CarreExp+2))+(*(BinaryValue+3))*(*(CarreExp+3))+(*(BinaryValue+5))*(*(CarreExp+5)))/255.0;
      p.PutGrayPixel(LBPimage,x,y,*valLBP);
    }
  }

  //le calcul de LBP pour la dernière colonne : C(w)
  if((x!=0 && x!=InputImage->height) && y==InputImage->width) 
  {
    for(int x = 1; x < InputImage->height-1; x++)
    {
      p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
      getBinValue(neighberPixel,BinaryValue,6,4);
      *valLBP=0;
      *valLBP=*valLBP+((*(BinaryValue))*(*(CarreExp))+(*(BinaryValue+1))*(*(CarreExp+1))+(*(BinaryValue+2))*(*(CarreExp+2))+(*(BinaryValue+3))*(*(CarreExp+3))+(*(BinaryValue+5))*(*(CarreExp+5)))/255.0;
      p.PutGrayPixel(LBPimage,x,y,*valLBP);
    }
  }

  //le calcul de LBP pour la dernière ligne : L(h)
  if(x==InputImage->height && (y!=0 && y!=InputImage->width))
  {
    for(int y = 1; y < InputImage->width-1; y++)
    {
      p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
      getBinValue(neighberPixel,BinaryValue,6,1);
      *valLBP=0;
      *valLBP=*valLBP+((*(BinaryValue))*(*(CarreExp))+(*(BinaryValue+2))*(*(CarreExp+2))+(*(BinaryValue+3))*(*(CarreExp+3))+(*(BinaryValue+4))*(*(CarreExp+4))+(*(BinaryValue+5))*(*(CarreExp+5)))/255.0;
      p.PutGrayPixel(LBPimage,x,y,*valLBP);
    }
  }

  //le calcul de LBP pour la première colonne : C(0)
  if((x!=0 && x!=InputImage->height) && y==0)
  {
    for(int x = 1; x <InputImage->height-1; x++)
    {
      p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
      getBinValue(neighberPixel,BinaryValue,6,2);
      *valLBP=0;
      *valLBP=*valLBP+((*(BinaryValue))*(*(CarreExp+5))+(*(BinaryValue+1))*(*(CarreExp+6))+(*(BinaryValue+3))*(*(CarreExp+3))+(*(BinaryValue+4))*(*(CarreExp))+(*(BinaryValue+5))*(*(CarreExp+1)))/255.0;
      p.PutGrayPixel(LBPimage,x,y,*valLBP);
    }
  }

  //pour le reste des elements d'image
  for(int y = 1; y < InputImage->height-1; y++)
  {
    for(int x = 1; x < InputImage->width-1; x++)
    {
      p.getNeighberhoodGrayPixel(InputImage, x,y,neighberPixel);
      getBinValue(neighberPixel,BinaryValue,9,4);
      //le calcul de la valeur du LBP pour chaque elem. d'im.
      *valLBP=0;
      for(int l = 0; l < 9; l++)
        *valLBP = *valLBP + ((*(BinaryValue+l)) * (*(CarreExp+l))) / 255.0;
      //printf("\nvalLBP(%d,%d)=%f",x,y,*valLBP);
      p.PutGrayPixel(LBPimage,x,y,*valLBP);	
    }
  }

  free(neighberPixel);
  free(BinaryValue);
  free(CarreExp);
  free(valLBP);
}

void FuzzyUtils::getBinValue(float* neighberGrayPixel, float* BinaryValue, int m, int n)
{
  // la comparaison entre la valeur d'elem d'image central et les valeurs des elem. d'im. voisins
  // m = le numero des elements (4, 6 ou 9);
  // n = la position de l'element central; 

  int h = 0;
  for(int k = 0; k < m; k++)
  {
    if(*(neighberGrayPixel+k) >= *(neighberGrayPixel+n))
    {
      *(BinaryValue+h)=1;
      h++;
    }
    else
    {
      *(BinaryValue+h)=0;
      h++;
    }	
  }
}

void FuzzyUtils::SimilarityDegreesImage(IplImage* CurrentImage, IplImage* BGImage, IplImage* DeltaImage, int n, int color_space)
{
  PixelUtils p;
  int i, j;

  if(n == 1)
  {
    float* CurrentGrayPixel = (float*) malloc (1*(sizeof(float)));
    float* BGGrayPixel = (float*) malloc (1*(sizeof(float)));
    float* DeltaGrayPixel = (float*) malloc (1*(sizeof(float)));

    for(i = 0; i < CurrentImage->width; i++)
    {
      for(j = 0; j < CurrentImage->height; j++)
      {
        p.GetGrayPixel(CurrentImage,i,j,CurrentGrayPixel);
        p.GetGrayPixel(BGImage,i,j,BGGrayPixel);
        RatioPixels(CurrentGrayPixel,BGGrayPixel,DeltaGrayPixel,1);
        p.PutGrayPixel(DeltaImage,i,j,*DeltaGrayPixel);
      }
    }

    free(CurrentGrayPixel);
    free(BGGrayPixel);
    free(DeltaGrayPixel);
  }

  if(n != 1)
  {   
    IplImage* ConvertedCurrentImage = cvCreateImage(cvSize(CurrentImage->width, CurrentImage->height), IPL_DEPTH_32F, 3);
    IplImage* ConvertedBGImage = cvCreateImage(cvSize(CurrentImage->width, CurrentImage->height), IPL_DEPTH_32F, 3);

    float* ConvertedCurrentPixel = (float*) malloc(3*(sizeof(float)));
    float* ConvertedBGPixel = (float*) malloc(3*(sizeof(float)));
    float* DeltaConvertedPixel = (float*) malloc(3*(sizeof(float)));

    p.ColorConversion(CurrentImage,ConvertedCurrentImage,color_space);
    p.ColorConversion(BGImage,ConvertedBGImage,color_space);

    for(i = 0; i < CurrentImage->width; i++)
    {
      for(j = 0; j < CurrentImage->height; j++)
      {
        p.GetPixel(ConvertedCurrentImage,i,j,ConvertedCurrentPixel);
        p.GetPixel(ConvertedBGImage,i,j,ConvertedBGPixel);
        RatioPixels(ConvertedCurrentPixel,ConvertedBGPixel,DeltaConvertedPixel,3);
        p.PutPixel(DeltaImage,i,j,DeltaConvertedPixel);
      }
    }

    free(ConvertedCurrentPixel);
    free(ConvertedBGPixel);
    free(DeltaConvertedPixel);

    cvReleaseImage(&ConvertedCurrentImage);
    cvReleaseImage(&ConvertedBGImage);
  }
}

void FuzzyUtils::RatioPixels(float* CurrentPixel, float* BGPixel, float* DeltaPixel, int n)
{
  if(n == 1)
  {
    if(*CurrentPixel < *BGPixel)
      *DeltaPixel = *CurrentPixel / *BGPixel;

    if(*CurrentPixel > *BGPixel)
      *DeltaPixel = *BGPixel / *CurrentPixel;

    if(*CurrentPixel == *BGPixel)
      *DeltaPixel = 1.0;
  }

  if(n == 3)
    for(int i = 0; i < 3; i++)
    {
      if(*(CurrentPixel+i) < *(BGPixel+i))
        *(DeltaPixel+i) = *(CurrentPixel+i) / *(BGPixel+i);

      if(*(CurrentPixel+i) > *(BGPixel+i))
        *(DeltaPixel+i) = *(BGPixel+i) / *(CurrentPixel+i);

      if(*(CurrentPixel+i) == *(BGPixel+i))
        *(DeltaPixel+i) = 1.0;			
    }
}

void FuzzyUtils::getFuzzyIntegralSugeno(IplImage* H, IplImage* Delta, int n, float *MeasureG, IplImage* OutputImage)
{
  // MeasureG : est un vecteur contenant 3 mesure g (g1,g2,g3) tel que : g1+g2+g3=1
  // n : =2 cad aggreger les 2 images "H" et "Delta" 
  //	   =1 cad aggreger uniquement les valeurs des composantes couleurs de l'image "Delta"

  PixelUtils p;

  float* HTexturePixel = (float*) malloc(1*sizeof(float));   
  float* DeltaOhtaPixel = (float*) malloc(3*(sizeof(float)));
  int *Indice = (int*) malloc(3*(sizeof(int)));
  float *HI = (float*) malloc(3*(sizeof(float)));
  float *Integral = (float*) malloc(3*(sizeof(float)));
  float* X = (float*) malloc(1*sizeof(float));
  float* XiXj = (float*) malloc(1*sizeof(float));
  float IntegralFlou;

  *Indice = 0;
  *(Indice+1) = 1;
  *(Indice+2) = 2;
  *X = 1.0;

  for(int i = 0; i < H->width; i++)
  {
    for(int j = 0; j < H->height; j++)
    {	
      p.GetGrayPixel(H,i,j,HTexturePixel);
      p.GetPixel(Delta,i,j,DeltaOhtaPixel);

      *(HI+0) = *(HTexturePixel+0);
      *(HI+1) = *(DeltaOhtaPixel+0);
      *(HI+2) = *(DeltaOhtaPixel+1);

      Trier(HI,3,Indice);

      *XiXj = *(MeasureG + (*(Indice+1))) + (*(MeasureG + (*(Indice+2))));			

      *(Integral+0) = min((HI + (*(Indice+0))), X);
      *(Integral+1) = min((HI + (*(Indice+1))), XiXj);
      *(Integral+2) = min((HI + (*(Indice+2))), ((MeasureG+(*(Indice+2)))));

      IntegralFlou = max(Integral,3);
      p.PutGrayPixel(OutputImage,i,j,IntegralFlou);
    }
  }

  free(HTexturePixel);
  free(DeltaOhtaPixel);
  free(Indice);
  free(HI);
  free(X);
  free(XiXj);
  free(Integral);
}

void FuzzyUtils::getFuzzyIntegralChoquet(IplImage* H, IplImage* Delta, int n, float *MeasureG, IplImage* OutputImage)
{
  // MeasureG : est un vecteur contenant 3 mesure g (g1,g2,g3) tel que : g1+g2+g3=1
  // n : =2 cad aggreger les 2 images "H" et "Delta" 
  //	   =1 cad aggreger uniquement les valeurs des composantes couleurs de l'image "Delta"

  PixelUtils p;

  float* HTexturePixel = (float*) malloc(1*sizeof(float));   
  float* DeltaOhtaPixel = (float*) malloc(3*(sizeof(float)));
  int *Indice = (int*) malloc(3*(sizeof(int)));
  float *HI = (float*) malloc(3*(sizeof(float)));
  float *Integral = (float*) malloc(3*(sizeof(float)));
  float* X = (float*) malloc(1*sizeof(float));
  float* XiXj = (float*) malloc(1*sizeof(float));
  float IntegralFlou;

  *Indice = 0;
  *(Indice+1) = 1;
  *(Indice+2) = 2;
  *X = 1.0;

  for(int i = 0; i < Delta->width; i++)
  {
    for(int j = 0; j < Delta->height; j++)
    {
      if(n == 2)
      {
        p.GetGrayPixel(H,i,j,HTexturePixel);
        p.GetPixel(Delta,i,j,DeltaOhtaPixel);

        *(HI+0) = *(HTexturePixel+0);
        *(HI+1) = *(DeltaOhtaPixel+0);
        *(HI+2) = *(DeltaOhtaPixel+1);
      }

      if(n==1)
      {
        //remplir HI par les valeurs des 3 composantes couleurs uniquement
        p.GetPixel(Delta,i,j,DeltaOhtaPixel);

        *(HI+0) = *(DeltaOhtaPixel+0);
        //*(HI+0) = *(DeltaOhtaPixel+2);
        *(HI+1) = *(DeltaOhtaPixel+1);
        *(HI+2) = *(DeltaOhtaPixel+2);
      }

      Trier(HI,3,Indice);
      *XiXj = *(MeasureG + (*(Indice+1))) + (*(MeasureG + (*(Indice+2))));

      *(Integral+0) = *(HI+(*(Indice+0)))* (*X-*XiXj);
      *(Integral+1) = *(HI+(*(Indice+1)))* (*XiXj-*(MeasureG+(*(Indice+2))));
      *(Integral+2) = *(HI+(*(Indice+2)))* (*(MeasureG+(*(Indice+2))));

      IntegralFlou = *(Integral+0) + *(Integral+1) + *(Integral+2);
      p.PutGrayPixel(OutputImage,i,j,IntegralFlou);
    }
  }

  free(HTexturePixel);
  free(DeltaOhtaPixel);
  free(Indice);
  free(HI);
  free(X);
  free(XiXj);
  free(Integral);
}

void FuzzyUtils::FuzzyMeasureG(float g1, float g2, float g3, float *G)
{
  *(G+0) = g1;
  *(G+1) = g2;
  *(G+2) = g3;
}

void FuzzyUtils::Trier(float* g,int n,int* index)
{
  // Cette fonction trie un vecteur g par ordre croissant et 
  // sort egalement l'indice des elements selon le trie dans le vecteur "index" supposé initialisé par des valeurs de 1 a n

  float t;	
  int r,a,b;

  for(a = 1; a <= n; a++)
  {
    for(b = n-1; b >= a; b--)
      if(*(g + b-1) < (*(g + b))) 
      {
        // ordre croissant des élements
        t = *(g + b-1);
        *(g + b-1) = *(g + b);
        *(g + b) = t;

        // ordre des indices des élements du vecteur g
        r = *(index + b-1);
        *(index + b-1) = *(index + b);
        *(index + b) = r;
      }		
  }
}

float FuzzyUtils::min(float *a,float *b)
{
  float min = 0;

  if(*a >= (*b)) 
    min = *b;
  else
    min = *a;

  return min;
}

float FuzzyUtils::max(float* g , int n)
{
  float max = 0;

  for(int i = 0; i < n; i++)
  {
    if(*(g+i) >= max)
      max = *(g+i);
  }
  
  return max;
}

void FuzzyUtils::gDeDeux(float* a, float* b, float* lambda)
{
  float* c = (float*) malloc(1*sizeof(float));
  *c = *a + (*b) + (*lambda) * (*a) * (*b);
}

void FuzzyUtils::getLambda(float* g)
{
  float a,b;
  float* lambda = (float*) malloc(1*sizeof(float)); 

  a = (*(g+0) * (*(g+1)) + (*(g+1)) * (*(g+2)) + (*(g+0)) * (*(g+2)));
  *lambda = -(*(g+0) * (*(g+1)) + (*(g+1)) * (*(g+2)) + (*(g+0)) * (*(g+2))) / (*(g+0) * (*(g+1)) * (*(g+2)));
  b = (*(g+0) * (*(g+1)) * (*(g+2)));
  
  //printf("\na:%f",a);
  //printf("\nb:%f",b);
  //printf("\nlambda:%f", *lambda);

  free(lambda);
}

void FuzzyUtils::AdaptativeSelectiveBackgroundModelUpdate(IplImage* CurrentImage, IplImage* BGImage, IplImage* OutputImage, IplImage* Integral, float seuil, float alpha)
{
  PixelUtils p;

  float beta = 0.0;
  float* CurentImagePixel = (float*) malloc(3*sizeof(float));
  float* BGImagePixel = (float*) malloc(3*sizeof(float));
  float* OutputImagePixel = (float*) malloc(3*sizeof(float));
  float* IntegralImagePixel = (float*) malloc(1*sizeof(float));
  float *Maximum = (float*) malloc(1*sizeof(float));
  float *Minimum = (float*) malloc(1*sizeof(float));

  p.ForegroundMaximum(Integral, Maximum, 1);
  p.ForegroundMinimum(Integral, Minimum, 1);

  for(int i = 0; i < CurrentImage->width; i++)
  {
    for(int j = 0; j < CurrentImage->height; j++)
    {
      p.GetPixel(CurrentImage, i, j, CurentImagePixel);
      p.GetPixel(BGImage, i, j, BGImagePixel);
      p.GetGrayPixel(Integral, i, j, IntegralImagePixel);
      
      beta = 1 - ((*IntegralImagePixel) - ((*Minimum / (*Minimum - *Maximum)) * (*IntegralImagePixel) - (*Minimum * (*Maximum) / (*Minimum - *Maximum))));
      
      for(int k = 0; k < 3; k++)
        *(OutputImagePixel + k) = beta * (*(BGImagePixel + k)) + (1 - beta) * (alpha * (*(CurentImagePixel+k)) + (1-alpha) * (*(BGImagePixel+k)));
      
      p.PutPixel(OutputImage, i, j, OutputImagePixel);
    }
  }

  free(CurentImagePixel);
  free(BGImagePixel);
  free(OutputImagePixel);
  free(IntegralImagePixel);
  free(Maximum);
  free(Minimum);
}
