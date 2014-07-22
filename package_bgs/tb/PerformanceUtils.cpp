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
#include "PerformanceUtils.h"
#include <opencv2/legacy/compat.hpp>

PerformanceUtils::PerformanceUtils(void){}

PerformanceUtils::~PerformanceUtils(void){}

float PerformanceUtils::NrPixels(IplImage *image)
{
  return (float) (image->width * image->height);
}

float PerformanceUtils::NrAllDetectedPixNotNULL(IplImage *image, IplImage *ground_truth)
{
  //Nombre de tous les pixels non nuls dans Groundthruth et dans image
  float Union12 = 0.0;

  unsigned char *pixelGT = (unsigned char*) malloc(1*sizeof(unsigned char));
  unsigned char *pixelI = (unsigned char*) malloc(1*sizeof(unsigned char));

  PixelUtils p;

  for(int y = 0; y < image->height; y++) 
  {
    for(int x = 0; x < image->width; x++) 
    { 
      p.GetGrayPixel(ground_truth,x,y,pixelGT); 
      p.GetGrayPixel(image,x,y,pixelI);

      if((pixelGT[0] != 0) || (pixelI[0] != 0))	
        Union12++;
    }
  }

  free(pixelGT);
  free(pixelI);

  return Union12;	
}

float PerformanceUtils::NrTruePositives(IplImage *image, IplImage *ground_truth, bool debug)
{
  float nTP = 0.0;

  unsigned char *pixelGT = (unsigned char*) malloc(1*sizeof(unsigned char));
  unsigned char *pixelI = (unsigned char*) malloc(1*sizeof(unsigned char));

  IplImage *TPimage = 0;

  if(debug)
  {
    TPimage = cvCreateImage(cvSize(image->width,image->height),image->depth,image->nChannels);
    cvFillImage(TPimage,0.0);
  }

  PixelUtils p;

  for(int y = 0; y < image->height; y++) 
  {
    for(int x = 0; x < image->width; x++) 
    { 
      p.GetGrayPixel(ground_truth,x,y,pixelGT); 
      p.GetGrayPixel(image,x,y,pixelI);

      if((pixelGT[0] != 0) && (pixelI[0] != 0))	
      {
        if(debug)
          p.PutGrayPixel(TPimage,x,y,*pixelI);

        nTP++;
      }
    }
  }

  if(debug)
  {
    cvNamedWindow("TPImage", 0);
    cvShowImage("TPImage", TPimage);
    //std::cout << "True Positives: " << nTP << std::endl;
    //<< " press ENTER to continue" << std::endl;
    //cvWaitKey(0);
    cvReleaseImage(&TPimage);
  }

  free(pixelGT);
  free(pixelI);

  return nTP;	
}

float PerformanceUtils::NrTrueNegatives(IplImage* image, IplImage* ground_truth, bool debug)
{
  float nTN = 0.0;

  unsigned char *pixelGT = (unsigned char *)malloc(1*sizeof(unsigned char));
  unsigned char *pixelI = (unsigned char *)malloc(1*sizeof(unsigned char));

  IplImage *TNimage = 0;

  if(debug)
  {
    TNimage = cvCreateImage(cvSize(image->width,image->height),image->depth,image->nChannels);
    cvFillImage(TNimage, 0.0);
  }

  PixelUtils p;

  for(int y = 0; y < image->height; y++) 
  {
    for(int x = 0; x < image->width; x++) 
    {
      p.GetGrayPixel(ground_truth,x,y,pixelGT);                
      p.GetGrayPixel(image,x,y,pixelI);

      if((pixelGT[0] == 0) && (pixelI[0] == 0.0))
      {
        *pixelI = 255;

        if(debug)
          p.PutGrayPixel(TNimage,x,y,*pixelI);

        nTN++;
      }				
    }
  }

  if(debug)
  {
    cvNamedWindow("TNImage", 0);
    cvShowImage("TNImage", TNimage);
    //std::cout << "True Negatives: " << nTN << std::endl;
    //<< " press ENTER to continue" << std::endl;
    //cvWaitKey(0);
    cvReleaseImage(&TNimage);
  }

  free(pixelGT);
  free(pixelI);

  return nTN;
}

float PerformanceUtils::NrFalsePositives(IplImage *image, IplImage *ground_truth,bool debug)
{
  float nFP = 0.0;

  unsigned char *pixelGT = (unsigned char*) malloc(1*sizeof(unsigned char));
  unsigned char *pixelI = (unsigned char*) malloc(1*sizeof(unsigned char));

  IplImage *FPimage = 0;

  if(debug)
  {
    FPimage = cvCreateImage(cvSize(image->width,image->height),image->depth,image->nChannels);
    cvFillImage(FPimage, 0.0);
  }

  PixelUtils p;

  for(int y = 0; y < image->height; y++) 
  {
    for(int x = 0; x < image->width; x++) 
    {
      p.GetGrayPixel(ground_truth,x,y,pixelGT);                
      p.GetGrayPixel(image,x,y,pixelI);

      if((pixelGT[0] == 0) && (pixelI[0] != 0))
      {
        if(debug)
          p.PutGrayPixel(FPimage,x,y,*pixelI);

        nFP++;
      }
    }
  }

  if(debug)
  {
    cvNamedWindow("FPImage", 0);
    cvShowImage("FPImage", FPimage);
    //std::cout << "False Positives: " << nFP << std::endl;
    //<< " press ENTER to continue" << std::endl;
    //cvWaitKey(0);
    cvReleaseImage(&FPimage);
  }

  free(pixelGT);
  free(pixelI);

  return nFP;	
}

float PerformanceUtils::NrFalseNegatives(IplImage * image, IplImage *ground_truth, bool debug)
{
  float nFN = 0.0;

  unsigned char *pixelGT = (unsigned char*) malloc(1*sizeof(unsigned char));
  unsigned char *pixelI = (unsigned char*) malloc(1*sizeof(unsigned char));

  IplImage *FNimage = 0;

  if(debug)
  {
    FNimage = cvCreateImage(cvSize(image->width,image->height),image->depth,image->nChannels);
    cvFillImage(FNimage, 0.0);
  }

  PixelUtils p;

  for(int y = 0; y < image->height; y++) 
  {
    for(int x = 0; x < image->width; x++) 
    {
      p.GetGrayPixel(ground_truth,x,y,pixelGT);
      p.GetGrayPixel(image,x,y,pixelI);

      if((pixelGT[0] != 0) && (pixelI[0] == 0))
      {
        if(debug)
          p.PutGrayPixel(FNimage,x,y,*pixelGT);

        nFN++;
      }
    }
  }

  if(debug)
  {
    cvNamedWindow("FNImage", 0);
    cvShowImage("FNImage", FNimage);
    //std::cout << "False Negatives: " << nFN << std::endl;
    //<< " press ENTER to continue" << std::endl;
    //cvWaitKey(0);
    cvReleaseImage(&FNimage);
  }

  free(pixelGT);
  free(pixelI);

  return nFN;	
}

float PerformanceUtils::SimilarityMeasure(IplImage *image, IplImage *ground_truth, bool debug)
{
  cv::Mat img_input(image,true);
  cv::Mat img_ref(ground_truth,true);

  int rn = cv::countNonZero(img_ref);
  cv::Mat i;
  cv::Mat u;

  if(rn > 0)
  {
    i = img_input & img_ref;
    u = img_input | img_ref;
  }
  else
  {
    i = (~img_input) & (~img_ref);
    u = (~img_input) | (~img_ref);
  }

  int in = cv::countNonZero(i);
  int un = cv::countNonZero(u);
    
  double s = (((double)in) / ((double)un));
  
  if(debug)
  {
    cv::imshow("A^B", i);
    cv::imshow("AvB", u);

    //std::cout << "Similarity Measure: " << s  << std::endl;
    
    //<< " press ENTER to continue" << std::endl;
    //cv::waitKey(0);
  }

  return s;
}

void PerformanceUtils::ImageROC(IplImage *image, IplImage* ground_truth, bool saveResults, char* filename)
{
  unsigned char *pixelGT = (unsigned char*) malloc(1*sizeof(unsigned char));
  unsigned char *pixelI = (unsigned char*) malloc(1*sizeof(unsigned char));

  IplImage *ROCimage = cvCreateImage(cvSize(image->width,image->height),image->depth,image->nChannels);
  cvFillImage(ROCimage, 0.0);

  PixelUtils p;

  for(int y = 0; y < image->height; y++) 
  {
    for(int x = 0; x < image->width; x++) 
    {
      p.GetGrayPixel(ground_truth,x,y,pixelGT);                
      p.GetGrayPixel(image,x,y,pixelI);

      if((pixelGT[0] != 0) && (pixelI[0] != 0)) // TP
      {
        *pixelI = 30;
        p.PutGrayPixel(ROCimage,x,y,*pixelI);
      }

      if((pixelGT[0] == 0) && (pixelI[0] == 0.0)) // TN
      {
        *pixelI = 0;
        p.PutGrayPixel(ROCimage,x,y,*pixelI);
      }	

      if((pixelGT[0] == 0) && (pixelI[0] != 0)) // FP
      {
        *pixelI = 255;
        p.PutGrayPixel(ROCimage,x,y,*pixelI);
      }

      if((pixelGT[0] != 0) && (pixelI[0] == 0)) // FN
      {
        *pixelI = 100;
        p.PutGrayPixel(ROCimage,x,y,*pixelI);
      }
    }
  }

  cvNamedWindow("ROC image", 0);
  cvShowImage("ROC image", ROCimage);

  if(saveResults)
  {
    unsigned char *pixelOI = (unsigned char*) malloc(1*sizeof(unsigned char));
    unsigned char *pixelROC = (unsigned char*) malloc(1*sizeof(unsigned char));

    float** freq;
    float nTP = 0.0;
    float nTN = 0.0;
    float nFP = 0.0;
    float nFN = 0.0;

    freq = (float**) malloc(256*(sizeof(float*)));
    for(int i = 0; i < 256; i++)
      freq[i] = (float*) malloc(7 * (sizeof(float)));

    for(int i = 0; i < 256; i++)
      for(int j = 0; j < 6; j++) 
        freq[i][j] = 0.0;

    for(int y = 0; y < image->height; y++)
    {
      for(int x = 0; x < image->width; x++) 
      {
        for(int i = 0; i < 256; i++)
        {
          p.GetGrayPixel(image,x,y,pixelOI);                
          p.GetGrayPixel(ROCimage,x,y,pixelROC);

          if((pixelOI[0] == i) && (pixelROC[0] == 30.0)) // TP
          {
            nTP++;
            freq[i][0] = nTP;
            break;
          }

          if((pixelOI[0] == i) && (pixelROC[0] == 0.0)) // TN
          {
            nTN++;
            freq[i][1] = nTN;
            break;
          }

          if((pixelOI[0] == i) && (pixelROC[0] == 255.0)) // FP
          {
            nFP++;
            freq[i][2] = nFP;
            break;
          }

          if((pixelOI[0] == i) && (pixelROC[0] == 100)) // FN
          {
            nFN++;
            freq[i][3] = nFN;
            break;
          }
        }
      }
    }

    //freq[i][0] = TP
    //freq[i][1] = TN
    //freq[i][2] = FP
    //freq[i][3] = FN
    //freq[i][4] = FNR
    //freq[i][5] = FPR

    std::ofstream f(filename);

    if(!f.is_open())
      std::cout << "Failed to open file " << filename << " for writing!" << std::endl;
    else
    {
      f << "  I     TP     TN     FP     FN    FPR      FNR      DR   \n" << std::endl;
      
      for(int i = 0; i < 256; i++)
      {
        //printf("%4d - TP:%5.0f, TN:%5.0f, FP:%5.0f, FN:%5.0f,", i, freq[i][0], freq[i][1], freq[i][2], freq[i][3]);

        if((freq[i][3] + freq[i][0] != 0.0) && (freq[i][2] + freq[i][1] != 0.0))
        {
          freq[i][4] = freq[i][3] / (freq[i][3] + freq[i][0]);  // FNR = FN / (TP + FN);
          freq[i][5] = freq[i][2] / (freq[i][2] + freq[i][1]);	// FPR = FP / (FP + TN);
          freq[i][6] = freq[i][0] / (freq[i][0] + freq[i][3]);	// DR = TP / (TP+FN);

          //printf(" FPR:%1.5f, FNR:%1.5f, D:%1.5f\n", freq[i][5], freq[i][4], freq[i][6]);
          ////fprintf(f," %4d     %1.6f     %1.6f\n",i,freq[i][5],freq[i][4]);
          ////fprintf(f,"  %1.6f     %1.6f\n",freq[i][5],freq[i][4]);
          char line[255];
          sprintf(line,"%3d %6.0f %6.0f %6.0f %6.0f %1.6f %1.6f %1.6f\n", 
            i, freq[i][0], freq[i][1], freq[i][2], freq[i][3], freq[i][5], freq[i][4], freq[i][6]);
          f << line;
        }
        //else
          //printf("\n");
      }

      std::cout << "Results saved in " << filename << std::endl;
      f.close();
    }

    free(freq);
    free(pixelOI);
    free(pixelROC);
  }

  //std::cout << "press ENTER to continue" << std::endl;
  //cvWaitKey(0);
  cvReleaseImage(&ROCimage);

  free(pixelGT);
  free(pixelI);
}

void PerformanceUtils::PerformanceEvaluation(IplImage *image, IplImage *ground_truth, bool saveResults, char* filename, bool debug)
{
  float N = 0;
  N = NrPixels(image);

  float U = 0;
  U = NrAllDetectedPixNotNULL(image, ground_truth);
  
  float TP = 0;
  TP = NrTruePositives(image, ground_truth, debug);
  
  float TN = 0;
  TN = NrTrueNegatives(image, ground_truth, debug);
  
  float FP = 0;
  FP = NrFalsePositives(image, ground_truth, debug);
  
  float FN = 0;
  FN = NrFalseNegatives(image, ground_truth, debug);
  
  float DetectionRate = TP / (TP + FN);
  float Precision = TP / (TP + FP);
  float Fmeasure = (2 * DetectionRate * Precision) / (DetectionRate + Precision);

  float Accuracy = (TN + TP) / N;
  float FalseNegativeRate = FN / (TP + FN);
  
  float FalsePositiveRate = FP / (FP + TN);
  float TruePositiveRate = TP / (TP + FN);
  
  float SM = 0;
  SM = SimilarityMeasure(image, ground_truth, debug);

  std::stringstream sstm;
  sstm << "N = " << N << std::endl;
  sstm << "U = " << U << std::endl;
  sstm << "TP = " << TP << std::endl;
  sstm << "TN = " << TN << std::endl;
  sstm << "FP = " << FP << std::endl;
  sstm << "FN = " << FN << std::endl;
  sstm << "DetectionRate     = " << DetectionRate << std::endl;
  sstm << "Precision         = " << Precision << std::endl;
  sstm << "Fmeasure          = " << Fmeasure << std::endl;
  sstm << "Accuracy          = " << Accuracy << std::endl;
  sstm << "FalseNegativeRate = " << FalseNegativeRate << std::endl;
  sstm << "FalsePositiveRate = " << FalsePositiveRate << std::endl;
  sstm << "TruePositiveRate  = " << TruePositiveRate << std::endl;
  sstm << "SimilarityMeasure = " << SM << std::endl;

  std::string results = sstm.str();
  std::cout << results;

  if(saveResults)
  {
    std::ofstream f(filename);

    if(!f.is_open())
      std::cout << "Failed to open file " << filename << " for writing!" << std::endl;
    else
    {
      f << results;
      std::cout << "Results saved in " << filename << std::endl;
      f.close();
    }
  }
}
