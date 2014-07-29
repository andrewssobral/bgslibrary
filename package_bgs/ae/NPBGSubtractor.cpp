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

// NPBGSubtractor.cpp: implementation of the NPBGSubtractor class.
//
//////////////////////////////////////////////////////////////////////

#include "NPBGSubtractor.h"
#include <assert.h>
#include <math.h>
#include <string.h>

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[]=__FILE__;
//#define new DEBUG_NEW
//#endif

void BGR2SnGnRn(unsigned char * in_image,
                unsigned char * out_image,
                unsigned int rows,
                unsigned int cols)
{
  unsigned int i;
  unsigned int r2,r3;
  unsigned int r,g,b;
  double s;

  for(i = 0; i < rows*cols*3; i += 3)
  {
    b=in_image[i];
    g=in_image[i+1];
    r=in_image[i+2];

    // calculate color ratios
    s = (double) 255 / (double) (b+g+r+30);

    r2 =(unsigned int) ((g+10) * s );
    r3 =(unsigned int) ((r+10) * s );

    out_image[i]   = (unsigned char) (((unsigned int) b+g+r) / 3);
    out_image[i+1] = (unsigned char) (r2 > 255 ? 255 : r2) ;
    out_image[i+2] = (unsigned char) (r3 > 255 ? 255 : r3) ;
  }
}

void UpdateDiffHist(unsigned char * image1, unsigned char * image2, DynamicMedianHistogram * pHist)
{
  unsigned int j;
  int bin,diff;

  unsigned int  imagesize	= pHist->imagesize;
  unsigned char histbins	= pHist->histbins;
  unsigned char *pAbsDiffHist = pHist->Hist;

  int histbins_1 = histbins-1;
  
  for(j = 0; j < imagesize; j++)
  {
    diff = (int) image1[j] - (int) image2[j];
    diff = abs(diff);
    // update histogram
    bin = (diff < histbins ? diff : histbins_1);
    pAbsDiffHist[j*histbins+bin]++;
  }
}

void FindHistMedians(DynamicMedianHistogram * pAbsDiffHist)
{
  unsigned char * Hist		   = pAbsDiffHist->Hist;
  unsigned char * MedianBins = pAbsDiffHist->MedianBins;
  unsigned char * AccSum	   = pAbsDiffHist->AccSum;
  unsigned char histsum		   = pAbsDiffHist->histsum;
  unsigned char histbins		 = pAbsDiffHist->histbins;
  unsigned int imagesize		 = pAbsDiffHist->imagesize;

  int sum;
  int bin;
  unsigned int histindex;
  unsigned char medianCount=histsum/2;
  unsigned int j;

  // find medians
  for(j = 0; j < imagesize; j++)
  {
    // find the median
    bin=0;
    sum=0;

    histindex=j*histbins;

    while(sum < medianCount)
    {
      sum+=Hist[histindex+bin];
      bin++;
    }

    bin--;

    MedianBins[j]=bin;
    AccSum[j]=sum;
  }
}

DynamicMedianHistogram BuildAbsDiffHist(unsigned char * pSequence,
  unsigned int rows,
  unsigned int cols,
  unsigned int color_channels,
  unsigned int SequenceLength,
  unsigned int histbins)
{

  unsigned int imagesize=rows*cols*color_channels;
  unsigned int i;

  DynamicMedianHistogram Hist;

  unsigned char *pAbsDiffHist = new unsigned char[rows*cols*color_channels*histbins];
  unsigned char *pMedianBins = new unsigned char[rows*cols*color_channels];
  unsigned char *pMedianFreq = new unsigned char[rows*cols*color_channels];
  unsigned char *pAccSum = new unsigned char[rows*cols*color_channels];

  memset(pAbsDiffHist,0,rows*cols*color_channels*histbins);

  Hist.Hist = pAbsDiffHist;
  Hist.MedianBins = pMedianBins;
  Hist.MedianFreq = pMedianFreq;
  Hist.AccSum = pAccSum;
  Hist.histbins = histbins;
  Hist.imagesize = rows*cols*color_channels;
  Hist.histsum = SequenceLength-1;

  unsigned char *image1, *image2;
  for(i = 1; i < SequenceLength; i++)
  {
    // find diff between frame i,i-1;
    image1 = pSequence+(i-1)*imagesize;
    image2 = pSequence+(i)*imagesize;

    UpdateDiffHist(image1,image2,&Hist);
  }

  FindHistMedians(&Hist);

  return Hist;
}

void EstimateSDsFromAbsDiffHist(DynamicMedianHistogram * pAbsDiffHist,
                                unsigned char * pSDs,
                                unsigned int imagesize,
                                double MinSD,
                                double MaxSD,
                                unsigned int kernelbins)
{
  double v;
  double kernelbinfactor=(kernelbins-1)/(MaxSD-MinSD);
  int medianCount; 
  int sum;
  int bin;
  unsigned int histindex;
  unsigned int j;
  unsigned int x1,x2;

  unsigned char *Hist		    =pAbsDiffHist->Hist;
  unsigned char *MedianBins	=pAbsDiffHist->MedianBins;
  unsigned char *AccSum		  =pAbsDiffHist->AccSum;
  unsigned char histsum		  =pAbsDiffHist->histsum;
  unsigned char histbins		=pAbsDiffHist->histbins;

  medianCount=(histsum)/2 ;

  for(j = 0; j < imagesize; j++)
  {
    histindex=j*histbins;

    bin=MedianBins[j];
    sum=AccSum[j];

    x1=sum-Hist[histindex+bin];
    x2=sum;

    // interpolate to get the median
    // x1 < 50 % < x2

    v =1.04 * ((double) bin-(double) (x2-medianCount)/ (double) (x2-x1));
    v=( v <= MinSD ? MinSD : v);

    // convert sd to kernel table bin

    bin=(int) (v>=MaxSD ? kernelbins-1 : floor((v-MinSD)*kernelbinfactor+.5));

    assert(bin>=0 && bin < kernelbins );

    pSDs[j]=bin;
  }
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NPBGSubtractor::NPBGSubtractor(){}

NPBGSubtractor::~NPBGSubtractor()
{
  delete AbsDiffHist.Hist;
  delete AbsDiffHist.MedianBins;
  delete AbsDiffHist.MedianFreq;
  delete AbsDiffHist.AccSum;
  delete KernelTable;
  delete BGModel->SDbinsImage;
  delete BGModel;
  delete Pimage1;
  delete Pimage2;
  delete tempFrame;
  delete imageindex->List;
  delete imageindex;
}

int NPBGSubtractor::Intialize(unsigned int prows,
                              unsigned int pcols,
                              unsigned int pcolor_channels,
                              unsigned int SequenceLength,
                              unsigned int pTimeWindowSize,
                              unsigned char pSDEstimationFlag,
                              unsigned char pUseColorRatiosFlag)
{

  rows=prows;
  cols=pcols;
  color_channels=pcolor_channels;
  imagesize=rows*cols*color_channels;
  SdEstimateFlag = pSDEstimationFlag;
  UseColorRatiosFlag=pUseColorRatiosFlag;
  //SampleSize = SequenceLength;

  AdaptBGFlag = FALSE;
  //
  SubsetFlag = TRUE;

  UpdateSDRate = 0;

  BGModel = new NPBGmodel(rows,cols,color_channels,SequenceLength,pTimeWindowSize,500);

  Pimage1= new double[rows*cols];
  Pimage2= new double[rows*cols];

  tempFrame= new unsigned char[rows*cols*3];

  imageindex = new ImageIndex;
  imageindex->List= new unsigned int [rows*cols];

  // error checking
  if (BGModel==NULL)
    return 0;

  return 1;
}

void NPBGSubtractor::AddFrame(unsigned char *ImageBuffer)
{
  if(UseColorRatiosFlag && color_channels==3)
    BGR2SnGnRn(ImageBuffer,ImageBuffer,rows,cols);
  
  BGModel->AddFrame(ImageBuffer);
}

void NPBGSubtractor::Estimation()
{
  int SampleSize=BGModel->SampleSize;

  memset(BGModel->TemporalMask,0,rows*cols*BGModel->TemporalBufferLength);

  //BGModel->AccMask= new unsigned int [rows*cols];
  memset(BGModel->AccMask,0,rows*cols*sizeof(unsigned int));

  unsigned char *pSDs = new unsigned char[rows*cols*color_channels];

  //DynamicMedianHistogram AbsDiffHist;

  int Abshistbins = 20;

  TimeIndex=0;

  // estimate standard deviations 

  if(SdEstimateFlag)
  {
    AbsDiffHist = BuildAbsDiffHist(BGModel->Sequence,rows,cols,color_channels,SampleSize,Abshistbins);
    EstimateSDsFromAbsDiffHist(&AbsDiffHist,pSDs,imagesize,SEGMAMIN,SEGMAMAX,SEGMABINS);
  }
  else
  {
    unsigned int bin;
    bin = (unsigned int) floor(((DEFAULTSEGMA-SEGMAMIN)*SEGMABINS)/(SEGMAMAX-SEGMAMIN));
    memset(pSDs,bin,rows*cols*color_channels*sizeof(unsigned char));
  }

  BGModel->SDbinsImage=pSDs;

  // Generate the Kernel
  KernelTable = new KernelLUTable(KERNELHALFWIDTH,SEGMAMIN,SEGMAMAX,SEGMABINS);
}

/*********************************************************************/

void BuildImageIndex(unsigned char * Image,
                     ImageIndex * imageIndex,
                     unsigned int rows,
                     unsigned int cols)
{
  unsigned int i,j;
  unsigned int r,c;
  unsigned int * image_list;

  j=cols+1;
  i=0;
  image_list=imageIndex->List;

  for(r = 1; r < rows-1; r++)
  {
    for(c = 1; c < cols-1; c++)
    {
      if(Image[j])
        image_list[i++]=j;
      
      j++;
    }
    j+=2;
  }

  imageIndex->cnt = i;
}

/*********************************************************************/

void HystExpandOperatorIndexed(unsigned char * inImage,
                               ImageIndex * inIndex,
                               double * Pimage,
                               double hyst_th,
                               unsigned char * outImage,
                               ImageIndex * outIndex,
                               unsigned int urows,
                               unsigned int ucols)
{
  unsigned int * in_list;
  unsigned int in_cnt;
  unsigned int * out_list;

  int rows,cols;

  int Nbr[9];
  unsigned int i,j;
  unsigned int k;
  unsigned int idx;

  rows=(int)  urows;
  cols=(int)  ucols;

  in_cnt=inIndex->cnt;
  in_list=inIndex->List;

  Nbr[0]=-cols-1;
  Nbr[1]=-cols;
  Nbr[2]=-cols+1;
  Nbr[3]=-1;
  Nbr[4]=0;
  Nbr[5]=1;
  Nbr[6]=cols-1;
  Nbr[7]=cols;
  Nbr[8]=cols+1;

  memset(outImage,0,rows*cols);

  out_list=outIndex->List;
  k=0;
  
  for(i = 0; i < in_cnt; i++)
  {
    for(j = 0; j < 9; j++)
    {
      idx = in_list[i] + Nbr[j];

      if(Pimage[idx] < hyst_th)
        outImage[idx] = 255;
    }
  }

  // build index for out image
  BuildImageIndex(outImage,outIndex,urows,ucols);
}

/*********************************************************************/

void HystShrinkOperatorIndexed(unsigned char * inImage,
                               ImageIndex * inIndex,
                               double * Pimage,
                               double hyst_th,
                               unsigned char * outImage,
                               ImageIndex * outIndex,
                               unsigned int urows,
                               unsigned int ucols)
{
  unsigned int * in_list;
  unsigned int in_cnt;
  unsigned int * out_list;

  int rows,cols;

  int Nbr[9];
  unsigned int i,j;
  unsigned int k,idx;

  rows=(int) urows;
  cols=(int) ucols;

  in_cnt=inIndex->cnt;
  in_list=inIndex->List;

  Nbr[0]=-cols-1;
  Nbr[1]=-cols;
  Nbr[2]=-cols+1;
  Nbr[3]=-1;
  Nbr[4]=0;
  Nbr[5]=1;
  Nbr[6]=cols-1;
  Nbr[7]=cols;
  Nbr[8]=cols+1;

  memset(outImage,0,rows*cols);

  out_list=outIndex->List;
  k=0;
  
  for(i = 0; i < in_cnt; i++)
  {
    idx = in_list[i];
    j = 0;

    while(j < 9 && inImage[idx+Nbr[j]])
      j++;
    
    if(j >= 9 || Pimage[idx] <= hyst_th)
      outImage[idx]=255;
  }

  BuildImageIndex(outImage,outIndex,rows,cols);
}

/*********************************************************************/

void ExpandOperatorIndexed(unsigned char * inImage,
                           ImageIndex * inIndex,
                           unsigned char * outImage,
                           ImageIndex * outIndex,
                           unsigned int urows,
                           unsigned int ucols)
{
  unsigned int * in_list;
  unsigned int in_cnt;
  unsigned int * out_list;

  int rows,cols;

  int Nbr[9];
  unsigned int i,j;
  unsigned int k;
  unsigned int idx;

  rows=(int)  urows;
  cols=(int)  ucols;

  in_cnt=inIndex->cnt;
  in_list=inIndex->List;

  Nbr[0]=-cols-1;
  Nbr[1]=-cols;
  Nbr[2]=-cols+1;
  Nbr[3]=-1;
  Nbr[4]=0;
  Nbr[5]=1;
  Nbr[6]=cols-1;
  Nbr[7]=cols;
  Nbr[8]=cols+1;


  memset(outImage,0,rows*cols);


  out_list=outIndex->List;
  k=0;
  for (i=0; i<in_cnt;i++)
    for (j=0;j<9;j++) {
      idx=in_list[i]+Nbr[j];
      outImage[idx]=255;
    }


    // build index for out image

    BuildImageIndex(outImage,outIndex,rows,cols);

}

/*********************************************************************/

void ShrinkOperatorIndexed(unsigned char * inImage,
                           ImageIndex * inIndex,
                           unsigned char * outImage,
                           ImageIndex * outIndex,
                           unsigned int urows,
                           unsigned int ucols)
{

  unsigned int * in_list;
  unsigned int in_cnt;
  unsigned int * out_list;

  int rows,cols;

  int Nbr[9];
  unsigned int i,j;
  unsigned int k,idx;

  rows=(int) urows;
  cols=(int) ucols;

  in_cnt=inIndex->cnt;
  in_list=inIndex->List;

  Nbr[0]=-cols-1;
  Nbr[1]=-cols;
  Nbr[2]=-cols+1;
  Nbr[3]=-1;
  Nbr[4]=0;
  Nbr[5]=1;
  Nbr[6]=cols-1;
  Nbr[7]=cols;
  Nbr[8]=cols+1;


  memset(outImage,0,rows*cols);

  out_list=outIndex->List;
  k=0;
  for (i=0; i<in_cnt;i++) {
    idx=in_list[i];
    j=0;

    while ( j<9 && inImage[idx+Nbr[j]]){
      j++;
    }

    if (j>=9) {
      outImage[idx]=255;
    }
  }

  BuildImageIndex(outImage,outIndex,rows,cols);
}

/*********************************************************************/

void NoiseFilter_o(unsigned char * Image,
                   unsigned char * ResultIm,
                   int rows,
                   int cols,
                   unsigned char th)
{
  /* assuming input is 1 for on, 0 for off */


  int r,c;
  unsigned char *p,*n,*nw,*ne,*e,*w,*s,*sw,*se;
  unsigned int v;
  unsigned int TH;

  unsigned char * ResultPtr;

  TH=255*th;

  memset(ResultIm,0,rows*cols);

  p=Image+cols+1;
  ResultPtr=ResultIm+cols+1;

  for(r=1;r<rows-1;r++)
  {
    for(c=1;c<cols-1;c++)
    {
      if (*p)
      {
        n=p-cols;
        ne=n+1;
        nw=n-1;
        e=p+1;
        w=p-1;
        s=p+cols;
        se=s+1;
        sw=s-1;

        v=(unsigned int) *nw+*n+*ne+*w+*e+*sw+*s+*se;

        if (v>=TH)
          *ResultPtr=255;
        else
          *ResultPtr=0;
      }
      p++;
      ResultPtr++;
    }
    p+=2;
    ResultPtr+=2;
  }
}

/*********************************************************************/

void NPBGSubtractor::SequenceBGUpdate_Pairs(unsigned char * image,
                                            unsigned char * Mask)
{
  unsigned int i,ic;
  unsigned char * pSequence 	=BGModel->Sequence;
  unsigned char * PixelQTop		=BGModel->PixelQTop;
  unsigned int Top						=BGModel->Top;
  unsigned int rate;

  int TemporalBufferTop						=(int) BGModel->TemporalBufferTop;
  unsigned char * pTemporalBuffer	= BGModel->TemporalBuffer;
  unsigned char * pTemporalMask		= BGModel->TemporalMask;
  int TemporalBufferLength				= BGModel->TemporalBufferLength;

  unsigned int * AccMask		=	BGModel->AccMask;
  unsigned int ResetMaskTh	= BGModel->ResetMaskTh;

  unsigned char *pAbsDiffHist = AbsDiffHist.Hist;
  unsigned char histbins = AbsDiffHist.histbins;
  int histbins_1=histbins-1;

  int TimeWindowSize = BGModel->TimeWindowSize;
  int SampleSize = BGModel->SampleSize;

  int TemporalBufferNext;

  unsigned int imagebuffersize=rows*cols*color_channels;
  unsigned int imagespatialsize=rows*cols;

  unsigned char mask;

  unsigned int histindex;
  unsigned char diff;
  unsigned char bin;

  static int TBCount=0;

  unsigned char * pTBbase1, * pTBbase2;
  unsigned char * pModelbase1, * pModelbase2;

  rate=TimeWindowSize/SampleSize;
  rate=(rate > 2) ? rate : 2;


  TemporalBufferNext=(TemporalBufferTop+1) 
    % TemporalBufferLength;

  // pointers to Masks : Top and Next
  unsigned char * pTMaskTop=pTemporalMask+TemporalBufferTop*imagespatialsize;
  unsigned char * pTMaskNext=pTemporalMask+TemporalBufferNext*imagespatialsize;

  // pointers to TB frames: Top and Next
  unsigned char * pTBTop=pTemporalBuffer+TemporalBufferTop*imagebuffersize;
  unsigned char * pTBNext=pTemporalBuffer+TemporalBufferNext*imagebuffersize;

  if ( ((TimeIndex) % rate == 0)  && TBCount >= TemporalBufferLength )
  {
    for(i=0,ic=0;i<imagespatialsize;i++,ic+=color_channels)
    {
      mask= * (pTMaskTop+i) || * (pTMaskNext+i);

      if(!mask)
      {
        // pointer to TB pixels to be added to the model
        pTBbase1=pTBTop+ic;
        pTBbase2=pTBNext+ic;

        // pointers to Model pixels to be replaced
        pModelbase1=pSequence+PixelQTop[i]*imagebuffersize+ic;
        pModelbase2=pSequence+((PixelQTop[i]+1)% SampleSize)*imagebuffersize+ic;

        // update Deviation Histogram
        if(SdEstimateFlag)
        {
          if(color_channels==1)
          {
            histindex=i*histbins;	

            // add new pair from temporal buffer
            diff=(unsigned char) abs((int) *pTBbase1 - (int) *pTBbase2);
            bin=(diff < histbins ? diff : histbins_1);
            pAbsDiffHist[histindex+bin]++;


            // remove old pair from the model
            diff=(unsigned char) abs((int) *pModelbase1-(int) *pModelbase2);
            bin=(diff < histbins ? diff : histbins_1);
            pAbsDiffHist[histindex+bin]--;
          }
          else
          {
            // color

            // add new pair from temporal buffer
            histindex=ic*histbins;	
            diff=abs(*pTBbase1 -
              *pTBbase2);
            bin=(diff < histbins ? diff : histbins_1);
            pAbsDiffHist[histindex+bin]++;

            histindex+=histbins;	
            diff=abs(*(pTBbase1+1) -
              *(pTBbase2+1));
            bin=(diff < histbins ? diff : histbins_1);
            pAbsDiffHist[histindex+bin]++;

            histindex+=histbins;	
            diff=abs(*(pTBbase1+2) -
              *(pTBbase2+2));
            bin=(diff < histbins ? diff : histbins_1);
            pAbsDiffHist[histindex+bin]++;

            // remove old pair from the model
            histindex=ic*histbins;	

            diff=abs(*pModelbase1-
              *pModelbase2);
            bin=(diff < histbins ? diff : histbins_1);
            pAbsDiffHist[histindex+bin]--;

            histindex+=histbins;	
            diff=abs(*(pModelbase1+1)-
              *(pModelbase2+1));
            bin=(diff < histbins ? diff : histbins_1);
            pAbsDiffHist[histindex+bin]--;

            histindex+=histbins;	
            diff=abs(*(pModelbase1+2)-
              *(pModelbase2+2));
            bin=(diff < histbins ? diff : histbins_1);
            pAbsDiffHist[histindex+bin]--;
          }
        }

        // add new pair into the model
        memcpy(pModelbase1,pTBbase1, color_channels*sizeof(unsigned char));

        memcpy(pModelbase2,pTBbase2, color_channels*sizeof(unsigned char));

        PixelQTop[i]=(PixelQTop[i]+2) % SampleSize;
      }
    }
  } // end if (sampling event)

  // update temporal buffer
  // add new frame to Temporal buffer.
  memcpy(pTBTop,image,imagebuffersize);

  // update AccMask
  // update new Mask with information in AccMask

  for (i=0;i<rows*cols;i++)
  {
    if (Mask[i])
      AccMask[i]++;
    else
      AccMask[i]=0;

    if (AccMask[i] > ResetMaskTh)
      Mask[i]=0;
  }

  // add new mask
  memcpy(pTMaskTop,Mask,imagespatialsize);

  // advance Temporal buffer pointer
  TemporalBufferTop=(TemporalBufferTop+1) % TemporalBufferLength;

  BGModel->TemporalBufferTop=TemporalBufferTop;

  TBCount++;

  // estimate SDs

  if (SdEstimateFlag && UpdateSDRate && ((TimeIndex) % UpdateSDRate == 0))
  {
    double MaxSD = KernelTable->maxsegma;
    double MinSD = KernelTable->minsegma;
    int KernelBins = KernelTable->segmabins;

    unsigned char * pSDs= BGModel->SDbinsImage;

    FindHistMedians(&(AbsDiffHist));
    EstimateSDsFromAbsDiffHist(&(AbsDiffHist),pSDs,imagebuffersize,MinSD,MaxSD,KernelBins);
  }

  TimeIndex++;
}

/*********************************************************************/

void DisplayPropabilityImageWithThresholding(double * Pimage,
                                             unsigned char * DisplayImage,
                                             double Threshold,
                                             unsigned int rows,
                                             unsigned int cols)
{
  double p;

  for(unsigned int i=0;i<rows*cols;i++)
  {
    p = Pimage[i];

    DisplayImage[i]=(p > Threshold) ?  0 : 255;
  }
}

/*********************************************************************/

void NPBGSubtractor::NPBGSubtraction_Subset_Kernel(
  unsigned char * image,
  unsigned char * FGImage,
  unsigned char * FilteredFGImage)
{
  unsigned int i,j;
  unsigned char *pSequence 	=BGModel->Sequence;

  unsigned int SampleSize			= BGModel->SampleSize;

  double *kerneltable	= KernelTable->kerneltable;
  int KernelHalfWidth		= KernelTable->tablehalfwidth;
  double *KernelSum			= KernelTable->kernelsums;
  double KernelMaxSigma = KernelTable->maxsegma;
  double KernelMinSigma = KernelTable->minsegma;
  int KernelBins				= KernelTable->segmabins;
  unsigned char * SDbins= BGModel->SDbinsImage;

  unsigned char * SaturationImage=FilteredFGImage;

  // default sigmas .. to be removed.
  double sigma1;
  double sigma2;
  double sigma3;

  sigma1=2.25;
  sigma2=2.25;
  sigma3=2.25;

  double p;
  double th;

  double alpha;

  alpha= AlphaValue;

  /* intialize FG image */

  memset(FGImage,0,rows*cols);

  //Threshold=1;
  th = Threshold * SampleSize;

  double sum=0,kernel1,kernel2,kernel3;
  int k,g;


  if (color_channels==1) 
  {
    // gray scale

    int kernelbase;
    
    for (i=0;i<rows*cols;i++)
    {
      kernelbase=SDbins[i]*(2*KernelHalfWidth+1);
      sum=0;
      j=0;

      while (j<SampleSize && sum < th)
      {
        g=pSequence[j*imagesize+i];
        k= g-  image[i] +KernelHalfWidth;
        sum+=kerneltable[kernelbase+k];
        j++;
      }

      p=sum/j;
      Pimage1[i]=p;
    }
  }
  else if (UseColorRatiosFlag && SubsetFlag)
  {
    // color ratios

    unsigned int ig;
    int base;

    int kernelbase1;
    int kernelbase2;
    int kernelbase3;

    unsigned int kerneltablewidth=2*KernelHalfWidth+1;

    double beta=3.0;    // minimum bound on the range.
    double betau=100.0;

    double beta_over_alpha = beta / alpha;
    double betau_over_alpha = betau / alpha;


    double brightness_lowerbound = 1-alpha;
    double brightness_upperbound = 1+alpha;
    int x1,x2;
    unsigned int SubsampleCount;

    for (i=0,ig=0;i<imagesize;i+=3,ig++)
    {
      kernelbase1=SDbins[i]*kerneltablewidth;
      kernelbase2=SDbins[i+1]*kerneltablewidth;
      kernelbase3=SDbins[i+2]*kerneltablewidth;

      sum=0;
      j=0;
      SubsampleCount=0;

      while (j<SampleSize && sum < th)
      {
        base=j*imagesize+i;
        g=pSequence[base];
        
        if (g < beta_over_alpha)
        {
          x1=(int) (g-beta);
          x2=(int) (g+beta);
        }
        else if (g > betau_over_alpha)
        {
          x1=(int) (g-betau);
          x2=(int) (g+betau);
        }
        else
        {
          x1=(int) (g*brightness_lowerbound+0.5);
          x2=(int) (g*brightness_upperbound+0.5);
        }

        if(x1<image[i] && image[i] <x2)
        {
          g=pSequence[base+1];
          k= (g-  image[i+1]) +KernelHalfWidth;
          kernel2=kerneltable[kernelbase2+k];

          g=pSequence[base+2];
          k= (g-  image[i+2]) +KernelHalfWidth;
          kernel3=kerneltable[kernelbase3+k];

          sum+=kernel2*kernel3;

          SubsampleCount++;
        }
        j++;
      }

      p=sum / j;
      Pimage1[ig]=p;
    }	
  }
  else if (UseColorRatiosFlag && ! SubsetFlag)
  {
    // color ratios

    unsigned int ig;
    int base;
    int bin;

    int kernelbase1;
    int kernelbase2;
    int kernelbase3;

    unsigned int kerneltablewidth=2*KernelHalfWidth+1;

    int gmin,gmax;
    double gfactor;

    gmax=200;
    gmin=10;

    gfactor = (KernelMaxSigma-KernelMinSigma) / (double) (gmax - gmin);

    for (i=0,ig=0;i<imagesize;i+=3,ig++)
    {

      bin=(int) floor(((alpha*16-KernelMinSigma)*KernelBins)/(KernelMaxSigma-KernelMinSigma));

      kernelbase1=bin*kerneltablewidth;
      kernelbase2=SDbins[i+1]*kerneltablewidth;
      kernelbase3=SDbins[i+2]*kerneltablewidth;

      sum=0;
      j=0;

      while (j<SampleSize && sum < th)
      {
        base=j*imagesize+i;
        g=pSequence[base];

        if (g < gmin )
          bin=0;
        else if (g > gmax)
          bin = KernelBins -1 ;
        else
          bin= (int) ((g-gmin) * gfactor + 0.5);
        
        kernelbase1=bin*kerneltablewidth;

        k= (g-  image[i]) +KernelHalfWidth;
        kernel1=kerneltable[kernelbase1+k];

        g=pSequence[base+1];
        k= (g-  image[i+1]) +KernelHalfWidth;
        kernel2=kerneltable[kernelbase2+k];

        g=pSequence[base+2];
        k= (g-  image[i+2]) +KernelHalfWidth;
        kernel3=kerneltable[kernelbase3+k];

        sum+=kernel1*kernel2*kernel3;
        j++;
      }

      p=sum / j;
      Pimage1[ig]=p;
    }
  }
  else // RGB color
  {
    unsigned int ig;
    int base;

    int kernelbase1;
    int kernelbase2;
    int kernelbase3;
    unsigned int kerneltablewidth=2*KernelHalfWidth+1;

    for (i=0,ig=0;i<imagesize;i+=3,ig++)
    {
      // used extimated kernel width to access the right kernel
      kernelbase1=SDbins[i]*kerneltablewidth;
      kernelbase2=SDbins[i+1]*kerneltablewidth;
      kernelbase3=SDbins[i+2]*kerneltablewidth;

      sum=0;
      j=0;
      while (j<SampleSize && sum < th)
      {
        base=j*imagesize+i;
        g=pSequence[base];
        k= (g-  image[i]) +KernelHalfWidth;
        kernel1=kerneltable[kernelbase1+k];

        g=pSequence[base+1];
        k= (g-  image[i+1]) +KernelHalfWidth;
        kernel2=kerneltable[kernelbase2+k];

        g=pSequence[base+2];
        k= (g-  image[i+2]) +KernelHalfWidth;
        kernel3=kerneltable[kernelbase3+k];

        sum+=kernel1*kernel2*kernel3;
        j++;
      }
      
      p=sum/j;
      Pimage1[ig]=p;
    }
  }

  DisplayPropabilityImageWithThresholding(Pimage1,FGImage,Threshold,rows,cols);
}

/*********************************************************************/

void NPBGSubtractor::NBBGSubtraction(unsigned char * Frame,
                                     unsigned char * FGImage,
                                     unsigned char * FilteredFGImage,
                                     unsigned char ** DisplayBuffers)
{
  if(UseColorRatiosFlag)
    BGR2SnGnRn(Frame,tempFrame,rows,cols);
  else
    memcpy(tempFrame,Frame,rows*cols*color_channels);

  NPBGSubtraction_Subset_Kernel(tempFrame,FGImage,FilteredFGImage);	
  /*NoiseFilter_o(FGImage,DisplayBuffers[3],rows,cols,4);
  BuildImageIndex(DisplayBuffers[3],imageindex,rows,cols);

  ExpandOperatorIndexed(DisplayBuffers[3],imageindex,DisplayBuffers[4],imageindex,rows,cols);
  ShrinkOperatorIndexed(DisplayBuffers[4],imageindex,FilteredFGImage,imageindex,rows,cols);

  memset(DisplayBuffers[3],0,rows*cols);*/
}

void NPBGSubtractor::Update(unsigned char * FGMask)
{
  if(UpdateBGFlag)
    SequenceBGUpdate_Pairs(tempFrame,FGMask);
}
