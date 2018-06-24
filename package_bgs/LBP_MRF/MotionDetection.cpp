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
*  This file is part of the AiBO+ project
*
*  Copyright (C) 2005-2013 Csaba Kertész (csaba.kertesz@gmail.com)
*
*  AiBO+ is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  AiBO+ is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
*
*  Paper: Csaba, Kertész: Texture-Based Foreground Detection, International Journal of Signal Processing,
*  Image Processing and Pattern Recognition (IJSIP), Vol. 4, No. 4, 2011.
*/

#include "MotionDetection.hpp"

#include "graph.h"
using namespace ck;

#if defined(__MINGW32__) || defined(__MINGW64__)
#include <cvaux.h>
#else
#include <opencv/cvaux.h>
#endif

#include <opencv2/imgproc.hpp>

#include "MEHistogram.hpp"
#include "MEImage.hpp"

// Pyramid picture for the tracking
IplImage *HUOFPyramid;
// Pyramid picture for the tracking
IplImage *HUOFPrevPyramid;

// Struct for histogram update data of a pixel
struct MEPixelDataType
{
  float BackgroundRate;
  int LifeCycle;
  float *Weights;
  bool *BackgroundHistogram;
  float **Histograms;
  float *PreviousHistogram;
};

MotionDetection::MotionDetection(DetectorType mode) :
  MDMode(md_NotDefined), MDDataState(ps_Uninitialized), Frames(0), ReadyMask(false),
  HUColorSpace(MEImage::csc_RGBtoCIELuv), HULBPMode(MEImage::lbp_Special),
  HUHistogramsPerPixel(3), HUHistogramArea(5), HUHistogramBins(8),
  HUImageWidth(-1), HUImageHeight(-1), HULBPPixelData(NULL),
  HUPrThres(0.75), HUBackgrThres(0.95), HUHistLRate(0.01), HUWeightsLRate(0.01),
  HUSamplePixels(-1), HUDesiredSamplePixels(-1), HUMinCutWeight(8.0),
  HUOFDataState(ps_Uninitialized), HUOFPointsNumber(-1),
  HUOFCamMovementX(0), MaxTrackedPoints(0), HUOFFrames(-1),
  HUOFCamMovement(false)
{
  HUOFPyramid = NULL;
  HUOFPrevPyramid = NULL;
  HUOFPoints[0] = NULL;
  HUOFPoints[1] = NULL;
  SetMode(mode);
}


MotionDetection::~MotionDetection()
{
  if (MDMode != md_NotDefined)
  {
    ReleaseData();
  }
}


void MotionDetection::SetMode(DetectorType newmode)
{
  if (MDMode != md_NotDefined && MDMode != newmode)
  {
    ReleaseData();
    Frames = 0;
    HUOFFrames = -1;
    HUOFCamMovement = false;
    HUOFCamMovementX = 0;
    ReadyMask = false;
  }

  switch (newmode)
  {
  case md_LBPHistograms:
    MDMode = md_LBPHistograms;
    break;

  case md_DLBPHistograms:
    MDMode = md_DLBPHistograms;
    break;

  default:
    MDMode = md_LBPHistograms;
    break;
  }
}


float MotionDetection::GetParameter(ParametersType param) const
{
  float ret = 0.0;

  switch (param)
  {
  case mdp_HUProximityThreshold:
    ret = (float)HUPrThres;
    break;

  case mdp_HUBackgroundThreshold:
    ret = (float)HUBackgrThres;
    break;

  case mdp_HUHistogramLearningRate:
    ret = (float)HUHistLRate;
    break;

  case mdp_HUWeightsLearningRate:
    ret = (float)HUWeightsLRate;
    break;

  case mdp_HUMinCutWeight:
    ret = (float)HUMinCutWeight;
    break;

  case mdp_HUDesiredSamplePixels:
    ret = (float)HUDesiredSamplePixels;
    break;

  case mdp_HUHistogramsPerPixel:
    ret = (float)HUHistogramsPerPixel;
    break;

  case mdp_HUHistogramArea:
    ret = (float)HUHistogramArea;
    break;

  case mdp_HUHistogramBins:
    ret = (float)HUHistogramBins;
    break;

  case mdp_HUColorSpace:
    ret = (float)HUColorSpace;
    break;

  case mdp_HULBPMode:
    ret = (float)HULBPMode;
    break;

  default:
    break;
  }
  return ret;
}


void MotionDetection::SetParameter(ParametersType param, float value)
{
  switch (param)
  {
  case mdp_HUProximityThreshold:
    HUPrThres = (float)value;
    break;

  case mdp_HUBackgroundThreshold:
    HUBackgrThres = (float)value;
    break;

  case mdp_HUHistogramLearningRate:
    HUHistLRate = (float)value;
    break;

  case mdp_HUWeightsLearningRate:
    HUWeightsLRate = (float)value;
    break;

  case mdp_HUMinCutWeight:
    HUMinCutWeight = (float)value;
    break;

  case mdp_HUDesiredSamplePixels:
    HUDesiredSamplePixels = (int)value;
    break;

  case mdp_HUHistogramsPerPixel:
    HUHistogramsPerPixel = (MDDataState == ps_Uninitialized) ? (int)value : HUHistogramsPerPixel;
    break;

  case mdp_HUHistogramArea:
    HUHistogramArea = (MDDataState == ps_Uninitialized) ? (int)value : HUHistogramArea;
    break;

  case mdp_HUHistogramBins:
    HUHistogramBins = (MDDataState == ps_Uninitialized) ? (int)value : HUHistogramBins;
    break;

  case mdp_HUColorSpace:
    HUColorSpace = (MDDataState == ps_Uninitialized) ? (int)value : HUColorSpace;
    break;

  case mdp_HULBPMode:
    HULBPMode = (MDDataState == ps_Uninitialized) ? (int)value : HULBPMode;
    break;

  default:
    break;
  }
}


void MotionDetection::DetectMotions(MEImage& image)
{
  switch (MDMode)
  {
  case md_LBPHistograms:
  case md_DLBPHistograms:
    DetectMotionsHU(image);
    break;

  default:
    break;
  }
}


void MotionDetection::GetMotionsMask(MEImage& mask_image)
{
  if (ReadyMask)
  {
    mask_image = MaskImage;
  }

  switch (MDMode)
  {
  case md_LBPHistograms:
  case md_DLBPHistograms:
    GetMotionsMaskHU(MaskImage);
    break;

  default:
    break;
  }

  ReadyMask = true;
  mask_image = MaskImage;
}


void MotionDetection::CalculateResults(MEImage& referenceimage, int& tnegatives, int& tpositives,
  int& ttnegatives, int& ttpositives)
{
  if (MDDataState != ps_Successful)
  {
    printf("No data for calculation.\n");
    return;
  }

  if (referenceimage.GetLayers() != 1)
    referenceimage.ConvertToGrayscale(MEImage::g_OpenCV);

  referenceimage.Binarize(1);

  MEImage mask_image;

  GetMotionsMask(mask_image);

  if ((mask_image.GetWidth() != referenceimage.GetWidth()) ||
    (mask_image.GetHeight() != referenceimage.GetHeight()))
  {
    printf("Different resolutions of mask<->reference image.\n");
    return;
  }

  unsigned char* RefMaskImgData = referenceimage.GetImageData();
  unsigned char* MaskImgData = mask_image.GetImageData();
  int RowStart = 0;
  int RowWidth = referenceimage.GetRowWidth();

  int TrueNegatives = 0;
  int TruePositives = 0;
  int TotalTrueNegatives = 0;
  int TotalTruePositives = 0;

  int ImageFrame = 0;

  if (MDMode == md_LBPHistograms || md_DLBPHistograms)
  {
    ImageFrame = HUHistogramArea / 2;
  }

  for (int y = referenceimage.GetHeight() - ImageFrame - 1; y >= ImageFrame; --y)
  {
    for (int x = referenceimage.GetWidth() - ImageFrame - 1; x >= ImageFrame; --x)
    {
      TrueNegatives +=
        (RefMaskImgData[RowStart + x] == 0) &&
        (MaskImgData[RowStart + x] == 0);
      TotalTrueNegatives += (RefMaskImgData[RowStart + x] == 0);
      TruePositives +=
        (RefMaskImgData[RowStart + x] == 255) &&
        (MaskImgData[RowStart + x] == 255);
      TotalTruePositives += (RefMaskImgData[RowStart + x] == 255);
    }
    RowStart += RowWidth;
  }

  tnegatives = TrueNegatives;
  ttnegatives = TotalTrueNegatives;
  tpositives = TruePositives;
  ttpositives = TotalTruePositives;
}


void MotionDetection::ReleaseData()
{
  if (MDMode == md_LBPHistograms || MDMode == md_DLBPHistograms)
  {
    ReleaseHUData();
  }
}


void MotionDetection::InitHUData(int imagewidth, int imageheight)
{
  if ((HUImageWidth != imagewidth - HUHistogramArea + 1) ||
    (HUImageHeight != imageheight - HUHistogramArea + 1) ||
    (MDDataState == ps_Uninitialized))
  {
    if (MDDataState != ps_Uninitialized)
    {
      ReleaseHUData();
    }

    MDDataState = ps_Initialized;

    HUImageWidth = imagewidth - HUHistogramArea + 1;
    HUImageHeight = imageheight - HUHistogramArea + 1;

    HULBPPixelData = new MEPixelDataType**[HUImageWidth / 2];

    for (int i = 0; i < HUImageWidth / 2; ++i)
    {
      HULBPPixelData[i] = new MEPixelDataType*[HUImageHeight];
    }

    for (int i = 0; i < HUImageWidth / 2; ++i)
      for (int i1 = 0; i1 < HUImageHeight; ++i1)
      {
        HULBPPixelData[i][i1] = new MEPixelDataType;
        HULBPPixelData[i][i1]->Weights = new float[HUHistogramsPerPixel];
        HULBPPixelData[i][i1]->BackgroundHistogram = new bool[HUHistogramsPerPixel];
        HULBPPixelData[i][i1]->Histograms = new float*[HUHistogramsPerPixel];
        for (int i2 = 0; i2 < HUHistogramsPerPixel; ++i2)
          HULBPPixelData[i][i1]->Histograms[i2] = new float[HUHistogramBins];
        HULBPPixelData[i][i1]->PreviousHistogram = new float[HUHistogramBins];
      }

    // Allocate auxiliary variables
    HUMaskColumnAddDel = new int*[HUHistogramArea];
    for (int i = 0; i < HUHistogramArea; ++i)
      HUMaskColumnAddDel[i] = new int[2];

    HUMaskRowAddDel = new int*[HUHistogramArea];
    for (int i = 0; i < HUHistogramArea; ++i)
      HUMaskRowAddDel[i] = new int[2];

    // Generate sample mask
    SetSampleMaskHU(sm_Circle, HUDesiredSamplePixels);

    // Init HU optical flow data
    if (MDMode == md_DLBPHistograms)
      InitHUOFData(imagewidth, imageheight);

    ClearHUData();
  }
}


void MotionDetection::InitHUOFData(int imagewidth, int imageheight)
{
  if (HUOFDataState != ps_Uninitialized)
  {
    ReleaseHUOFData();
  }

  if (HUOFDataState == ps_Uninitialized)
  {
    HUOFPointsNumber = imagewidth*imageheight / 1000;
    HUOFPyramid = cvCreateImage(cvSize(imagewidth, imageheight), 8, 1);
    HUOFPrevPyramid = cvCreateImage(cvSize(imagewidth, imageheight), 8, 1);
    HUOFPoints[0] = (CvPoint2D32f*)cvAlloc(HUOFPointsNumber * sizeof(HUOFPoints[0][0]));
    HUOFPoints[1] = (CvPoint2D32f*)cvAlloc(HUOFPointsNumber * sizeof(HUOFPoints[1][0]));
  }
}


void MotionDetection::ReleaseHUData()
{
  if (MDDataState != ps_Uninitialized)
  {
    for (int i = 0; i < HUImageWidth / 2; i++)
      for (int i1 = 0; i1 < HUImageHeight; i1++)
      {
        delete[] HULBPPixelData[i][i1]->PreviousHistogram;
        for (int i2 = 0; i2 < HUHistogramsPerPixel; ++i2)
          delete[] HULBPPixelData[i][i1]->Histograms[i2];
        delete[] HULBPPixelData[i][i1]->Histograms;
        delete[] HULBPPixelData[i][i1]->BackgroundHistogram;
        delete[] HULBPPixelData[i][i1]->Weights;
        delete HULBPPixelData[i][i1];
      }

    for (int i = 0; i < HUImageWidth / 2; i++)
    {
      delete[] HULBPPixelData[i];
    }
    delete[] HULBPPixelData;

    if (MDMode == md_DLBPHistograms)
      ReleaseHUOFData();

    HUImageWidth = -1;
    HUImageHeight = -1;
    HULBPPixelData = NULL;
    MDDataState = ps_Uninitialized;

    // Release auxiliary variables
    for (int i = 0; i < HUHistogramArea; ++i)
      delete[] HUMaskColumnAddDel[i];
    delete[] HUMaskColumnAddDel;

    for (int i = 0; i < HUHistogramArea; ++i)
      delete[] HUMaskRowAddDel[i];
    delete[] HUMaskRowAddDel;

    HUMaskColumnAddDel = NULL;
    HUMaskRowAddDel = NULL;
  }
}


void MotionDetection::ReleaseHUOFData()
{
  if (MDDataState != ps_Uninitialized)
  {
    if (HUOFPyramid)
    {
      cvReleaseImage(&HUOFPyramid);
      HUOFPyramid = NULL;
    }
    if (HUOFPrevPyramid)
    {
      cvReleaseImage(&HUOFPrevPyramid);
      HUOFPrevPyramid = NULL;
    }
    if (HUOFPoints[0])
    {
      cvFree(&HUOFPoints[0]);
      HUOFPoints[0] = NULL;
    }
    if (HUOFPoints[1])
    {
      cvFree(&HUOFPoints[1]);
      HUOFPoints[1] = NULL;
    }
    HUOFDataState = ps_Uninitialized;
  }
}


void MotionDetection::ClearHUData()
{
  if (MDDataState != ps_Uninitialized)
  {
    for (int i = (HUImageWidth / 2) - 1; i >= 0; --i)
      for (int i1 = HUImageHeight - 1; i1 >= 0; --i1)
      {
        for (int i2 = HUHistogramsPerPixel - 1; i2 >= 0; --i2)
        {
          memset(HULBPPixelData[i][i1]->Histograms[i2], 0,
            HUHistogramBins * sizeof(float));
          HULBPPixelData[i][i1]->Weights[i2] = 1.0 / HUHistogramsPerPixel;
          HULBPPixelData[i][i1]->BackgroundHistogram[i2] = true;
        }
        HULBPPixelData[i][i1]->BackgroundRate = 1.0;
        HULBPPixelData[i][i1]->LifeCycle = 0;
      }
    MDDataState = ps_Initialized;
  }
}


void MotionDetection::DetectMotionsHU(MEImage& image)
{
  unsigned char *ImgData = NULL;
  MEImage newimage = image;
  float DiffAreas = 0;

  // Init the histogram update data structures if needs be
  if ((MDDataState == ps_Uninitialized) ||
    (HUImageWidth != newimage.GetWidth() - HUHistogramArea + 1) ||
    (HUImageHeight != newimage.GetHeight() - HUHistogramArea + 1))
  {
    InitHUData(newimage.GetWidth(), newimage.GetHeight());
  }

  if (newimage.GetLayers() == 1)
  {
    newimage.ConvertGrayscaleToRGB();
  }

  MEImage blueimage = newimage;
  blueimage.ColorSpace(MEImage::csc_RGBtoCIELuv);

  if (HUColorSpace != -1)
  {
    newimage.ColorSpace((MEImage::ColorSpaceConvertType)HUColorSpace);
  }

  if (Frames == 0)
  {
    MEImage BlueLayer;
    blueimage.GetLayer(BlueLayer, 1);
    BlueLayer.Resize(32, 24);
    PreviousBlueLayer = BlueLayer;
  }

  Frames++;

  // Detect the fast, big changes in the scene
  MEImage BlueLayer;
  blueimage.GetLayer(BlueLayer, 1);
  BlueLayer.Resize(32, 24);
  DiffAreas = BlueLayer.DifferenceAreas(PreviousBlueLayer, 12);

  if (DiffAreas > 80)
  {
    MDDataState = ps_Initialized;
    if (MDMode == md_DLBPHistograms)
      HUOFDataState = ps_Initialized;
    printf("Frame: %d - big changes in the scene (%f)", Frames, DiffAreas);
    Frames = 1;
    HUOFFrames = -1;
  }
  PreviousBlueLayer = BlueLayer;

  if (Frames == 1)
  {
    CurrentImage = image;
    PreviousImage = CurrentImage;
  }
  else
    if (Frames > 1)
    {
      PreviousImage = CurrentImage;
      CurrentImage = image;
      // Optical flow correction of the camera movements
      if (MDMode == md_DLBPHistograms)
      {
        OpticalFlowCorrection();
      }
    }

  newimage.ConvertToGrayscale(MEImage::g_OpenCV);

  if (HULBPMode != -1)
  {
    newimage.LBP((MEImage::LBPType)HULBPMode);
  }

  // Set some auxiliary variables
  ImgData = newimage.GetImageData();
  int DivisionOperator = (int)(log((double)256 / HUHistogramBins) / log((double) 2.)) + 1;

  // Downscale the image
  for (int i = newimage.GetRowWidth()*newimage.GetHeight() - 1; i >= 0; --i)
  {
    ImgData[i] >>= DivisionOperator;
  }

  UpdateModelHU(newimage, HULBPPixelData);

  // Change the state of the HU data structures
  if (MDDataState == ps_Initialized)
  {
    MDDataState = ps_Successful;
  }
  HUOFCamMovement = false;
  ReadyMask = false;
}


void MotionDetection::UpdateModelHU(MEImage& image, MEPixelDataType*** model)
{
  float *CurrentHistogram = new float[HUHistogramBins];
  float *CurrentHistogram2 = new float[HUHistogramBins];
  unsigned char *ImgData = image.GetImageData();
  int RowWidth = image.GetRowWidth();
  int RowStart = (HUImageHeight - 1)*RowWidth;

  memset(CurrentHistogram, 0, HUHistogramBins * sizeof(float));
  // Calculate the first histogram
  for (int y = HUHistogramArea - 1; y >= 0; --y)
  {
    for (int x = HUHistogramArea - 1; x >= 0; --x)
    {
      if ((HUMaskRowAddDel[y][1] > x) && (HUMaskRowAddDel[y][0] <= x) &&
        (HUMaskColumnAddDel[x][1] > y) && (HUMaskColumnAddDel[x][0] <= y))
      {
        CurrentHistogram[ImgData[RowStart + HUImageWidth - 1 + x]]++;
      }
    }
    RowStart += RowWidth;
  }

  // This cycle generates the last row of histograms
  for (int y = HUImageHeight - 1; y >= 0; --y)
  {
    if (HUImageHeight - 1 > y)
    {
      // Delete and add a pixel column from the histogram data
      for (int i = HUHistogramArea - 1; i >= 0; --i)
      {
        if (HUMaskColumnAddDel[i][0] != -1)
          CurrentHistogram[ImgData[RowWidth*(y + HUMaskColumnAddDel[i][0]) + HUImageWidth - 1 + i]]++;
        if (HUMaskColumnAddDel[i][1] != -1)
          CurrentHistogram[ImgData[RowWidth*(y + HUMaskColumnAddDel[i][1]) + HUImageWidth - 1 + i]]--;
      }
    }

    if (y % 2 == HUImageWidth % 2)
    {
      MEPixelDataType* PixelData = model[(HUImageWidth - 1) / 2][y];

      // Allocate and initialize the pixel data if needs be
      if (!PixelData)
      {
        // Memory allocation
        PixelData = new MEPixelDataType;
        PixelData->Weights = new float[HUHistogramsPerPixel];
        PixelData->BackgroundHistogram = new bool[HUHistogramsPerPixel];
        PixelData->Histograms = new float*[HUHistogramsPerPixel];
        for (int i2 = 0; i2 < HUHistogramsPerPixel; ++i2)
          PixelData->Histograms[i2] = new float[HUHistogramBins];
        PixelData->PreviousHistogram = new float[HUHistogramBins];

        for (int i = HUHistogramsPerPixel - 1; i >= 0; --i)
        {
          memcpy(PixelData->Histograms[i], CurrentHistogram, HUHistogramBins * sizeof(float));
          PixelData->Weights[i] = 1.0 / HUHistogramsPerPixel;
          PixelData->BackgroundHistogram[i] = true;
        }
        PixelData->BackgroundRate = 1.0;
        PixelData->LifeCycle = 0;
        memcpy(PixelData->PreviousHistogram, CurrentHistogram, HUHistogramBins * sizeof(float));

        model[(HUImageWidth - 1) / 2][y] = PixelData;
      }
      else {
        bool InitHistograms = (MDDataState == ps_Initialized);

        if (MDDataState != ps_Initialized && HUOFCamMovement)
        {
          // Histogram intersection between the previous and the current histogram
          float Difference = 0.0;
          for (int i1 = HUHistogramBins - 1; i1 >= 0; --i1)
          {
            Difference += (float)(CurrentHistogram[i1] < PixelData->PreviousHistogram[i1] ?
              CurrentHistogram[i1] : PixelData->PreviousHistogram[i1]);
          }
          Difference /= HUSamplePixels;

          if (Difference < HUBackgrThres)
            InitHistograms = true;
        }
        if (InitHistograms)
        {
          // Copy the histogram data to the HU data structures
          for (int i = HUHistogramsPerPixel - 1; i >= 0; --i)
          {
            memcpy(PixelData->Histograms[i], CurrentHistogram, HUHistogramBins * sizeof(float));
            PixelData->Weights[i] = 1.0 / HUHistogramsPerPixel;
            PixelData->BackgroundHistogram[i] = true;
          }
          memcpy(PixelData->PreviousHistogram, CurrentHistogram, HUHistogramBins * sizeof(float));
          PixelData->BackgroundRate = 1.0;
          PixelData->LifeCycle = 0;
        }
        else {
          // Update the HU data structures
          UpdateHUPixelData(PixelData, CurrentHistogram);

          if (MDMode == md_DLBPHistograms)
          {
            memcpy(PixelData->PreviousHistogram, CurrentHistogram, HUHistogramBins * sizeof(float));
          }
        }
      }
    }

    // Copy the histogram
    memcpy(CurrentHistogram2, CurrentHistogram, HUHistogramBins * sizeof(float));

    // This cycle generates a column of histograms
    for (int x = HUImageWidth - 2; x >= 0; --x)
    {
      RowStart = RowWidth*y;

      // Delete and add a pixel column from the histogram data
      for (int i = HUHistogramArea - 1; i >= 0; --i)
      {
        if (HUMaskRowAddDel[i][0] != -1)
          CurrentHistogram2[ImgData[RowStart + x + HUMaskRowAddDel[i][0]]]++;
        if (HUMaskRowAddDel[i][1] != -1)
          CurrentHistogram2[ImgData[RowStart + x + HUMaskRowAddDel[i][1]]]--;

        RowStart += RowWidth;
      }
      if (x % 2 == 0)
      {
        MEPixelDataType* PixelData = model[x / 2][y];

        // Allocate and initialize the pixel data if needs be
        if (!PixelData)
        {
          // Memory allocation
          PixelData = new MEPixelDataType;
          PixelData->Weights = new float[HUHistogramsPerPixel];
          PixelData->BackgroundHistogram = new bool[HUHistogramsPerPixel];
          PixelData->Histograms = new float*[HUHistogramsPerPixel];
          for (int i2 = 0; i2 < HUHistogramsPerPixel; ++i2)
            PixelData->Histograms[i2] = new float[HUHistogramBins];
          PixelData->PreviousHistogram = new float[HUHistogramBins];

          for (int i = HUHistogramsPerPixel - 1; i >= 0; --i)
          {
            memcpy(PixelData->Histograms[i], CurrentHistogram2, sizeof(CurrentHistogram2));
            PixelData->Weights[i] = 1.0 / HUHistogramsPerPixel;
            PixelData->BackgroundHistogram[i] = true;
          }
          PixelData->BackgroundRate = 1.0;
          PixelData->LifeCycle = 0;
          model[x / 2][y] = PixelData;
          memcpy(PixelData->PreviousHistogram, CurrentHistogram2, sizeof(CurrentHistogram2));
        }
        else {
          bool InitHistograms = (MDDataState == ps_Initialized);

          if (MDDataState != ps_Initialized && HUOFCamMovement)
          {
            // Histogram intersection between the previous and the current histogram
            float Difference = 0.0;
            for (int i1 = HUHistogramBins - 1; i1 >= 0; --i1)
            {
              Difference += (float)(CurrentHistogram2[i1] < PixelData->PreviousHistogram[i1] ?
                CurrentHistogram2[i1] : PixelData->PreviousHistogram[i1]);
            }
            Difference /= HUSamplePixels;

            if (Difference < HUBackgrThres)
              InitHistograms = true;
          }
          if (InitHistograms)
          {
            // Copy the histogram data to the HU data structures
            for (int i = HUHistogramsPerPixel - 1; i >= 0; --i)
            {
              memcpy(PixelData->Histograms[i], CurrentHistogram2, sizeof(CurrentHistogram2));
              PixelData->Weights[i] = 1.0 / HUHistogramsPerPixel;
              PixelData->BackgroundHistogram[i] = true;
            }
            memcpy(PixelData->PreviousHistogram, CurrentHistogram2, sizeof(CurrentHistogram2));
            PixelData->BackgroundRate = 1.0;
            PixelData->LifeCycle = 0;
          }
          else {
            // Update the HU data structures
            UpdateHUPixelData(PixelData, CurrentHistogram2);

            if (MDMode == md_DLBPHistograms)
            {
              memcpy(PixelData->PreviousHistogram, CurrentHistogram2, sizeof(CurrentHistogram2));
            }
          }
        }
      }

    }
  }
  delete[] CurrentHistogram;
  delete[] CurrentHistogram2;
}


void MotionDetection::UpdateHUPixelData(MEPixelDataType* PixelData, const float *histogram)
{
  int MaxIndex = 0;
  float MaxValue = -1;
  bool Replace = true;
  float *IntersectionResults = new float[HUHistogramsPerPixel];

  PixelData->LifeCycle++;
  PixelData->BackgroundRate = 0.0;

  // Compute intersection between the currect and older histograms
  for (int i = HUHistogramsPerPixel - 1; i >= 0; --i)
  {
    // Histogram intersection
    float Difference = 0.0;
    for (int i1 = HUHistogramBins - 1; i1 >= 0; --i1)
    {
      Difference += (float)histogram[i1] < PixelData->Histograms[i][i1] ?
        (float)histogram[i1] : PixelData->Histograms[i][i1];
    }

    IntersectionResults[i] = (float)Difference / (float)(HUSamplePixels);

    if (PixelData->BackgroundHistogram[i] &&
      IntersectionResults[i] > PixelData->BackgroundRate)
    {
      PixelData->BackgroundRate = IntersectionResults[i];
    }

    if (MaxValue < IntersectionResults[i])
    {
      MaxValue = IntersectionResults[i];
      MaxIndex = i;
    }

    Replace = Replace && (IntersectionResults[i] < HUPrThres);
  }

  // Replace the histogram with the lowest weight
  if (Replace)
  {
    // Find the histogram with minimal weight
    int MinIndex = 0;
    float MinValue = PixelData->Weights[0];
    for (int i1 = HUHistogramsPerPixel - 1; i1 > 0; --i1)
    {
      if (MinValue > PixelData->Weights[i1])
      {
        MinValue = PixelData->Weights[i1];
        MinIndex = i1;
      }
    }

    PixelData->Weights[MinIndex] = 0.01;
    for (int i1 = HUHistogramBins - 1; i1 >= 0; --i1)
      PixelData->Histograms[MinIndex][i1] = (float)histogram[i1];
    PixelData->BackgroundHistogram[MinIndex] = 0;

    // Normalize the weights
    float sum = 0;
    for (int i1 = HUHistogramsPerPixel - 1; i1 >= 0; --i1)
      sum += PixelData->Weights[i1];

    for (int i1 = HUHistogramsPerPixel - 1; i1 >= 0; --i1)
      PixelData->Weights[i1] = PixelData->Weights[i1] / sum;

    return;
  }

  float LearningRate = HUHistLRate;

  if (PixelData->LifeCycle < 100)
    LearningRate += (float)(100 - PixelData->LifeCycle) / 100;
  else
    if (MDMode == md_DLBPHistograms && HUOFFrames != -1 && HUOFFrames < 40)
      LearningRate += (HUOFFrames < 80 ? 0.05 : 0);

  // Match was found -> Update the histogram of the best match
  for (int i = HUHistogramBins - 1; i >= 0; --i)
  {
    PixelData->Histograms[MaxIndex][i] *= (1.0 - LearningRate);
    PixelData->Histograms[MaxIndex][i] += LearningRate*(float)histogram[i];
  }

  LearningRate = HUWeightsLRate;
  if (PixelData->LifeCycle < 100)
    LearningRate += (float)(100 - PixelData->LifeCycle) / 100;
  else
    if (MDMode == md_DLBPHistograms && HUOFFrames != -1 && HUOFFrames < 40)
      LearningRate += (HUOFFrames < 80 ? 0.05 : 0);

  // Update the weights of the histograms
  for (int i = HUHistogramsPerPixel - 1; i >= 0; --i)
  {
    PixelData->Weights[i] =
      (LearningRate*(i == MaxIndex) + (1.0 - LearningRate)*PixelData->Weights[i]);
  }

  // Order and select the background histograms
  float **Weights = new float*[HUHistogramsPerPixel];
  for (int i = 0; i < HUHistogramsPerPixel; ++i)
    Weights[i] = new float[2];

  for (int i = HUHistogramsPerPixel - 1; i >= 0; --i)
  {
    Weights[i][0] = (float)i;
    Weights[i][1] = PixelData->Weights[i];
  }

  for (int i1 = HUHistogramsPerPixel - 1; i1 >= 2; --i1)
    for (int i = i1; i >= 1; --i)
    {
      if (Weights[i][1] <= Weights[i - 1][1])
      {
        float tmp = Weights[i][0];
        float tmp2 = Weights[i][1];

        Weights[i][0] = Weights[i - 1][0];
        Weights[i][1] = Weights[i - 1][1];

        Weights[i - 1][0] = tmp;
        Weights[i - 1][1] = tmp2;
      }
    }

  float Sum = 0;
  int i = 0;

  for (i = HUHistogramsPerPixel - 1; i >= 0; --i)
  {
    Sum += Weights[i][1];
    PixelData->BackgroundHistogram[(int)Weights[i][0]] = true;

    if (Sum > HUBackgrThres)
      break;
  }
  for (int i1 = i - 1; i1 >= 0; --i1)
  {
    PixelData->BackgroundHistogram[(int)Weights[i1][0]] = false;
  }
  delete[] IntersectionResults;
  for (int i = 0; i < HUHistogramsPerPixel; ++i)
    delete[] Weights[i];
  delete[] Weights;
}


void MotionDetection::OpticalFlowCorrection()
{
  IplImage *PreviousGray = NULL, *CurrentGray = NULL;
  char* PointsStatus = (char*)cvAlloc(HUOFPointsNumber);
  int i = 0, i1 = 0;

  if (HUOFFrames != -1)
    HUOFFrames++;

  // Convert the images into grayscale
  if (CurrentImage.GetLayers() > 1)
  {
    CurrentGray = cvCreateImage(cvGetSize(CurrentImage.GetIplImage()), IPL_DEPTH_8U, 1);
    cvCvtColor(CurrentImage.GetIplImage(), CurrentGray, CV_BGR2GRAY);
  }
  else
    CurrentGray = (IplImage*)CurrentImage.GetIplImage();
  if (PreviousImage.GetLayers() > 1)
  {
    PreviousGray = cvCreateImage(cvGetSize(CurrentImage.GetIplImage()), IPL_DEPTH_8U, 1);
    cvCvtColor(PreviousImage.GetIplImage(), PreviousGray, CV_BGR2GRAY);
  }
  else
    PreviousGray = (IplImage*)PreviousImage.GetIplImage();

  if (HUOFDataState != ps_Successful)
  {
    printf("Search new corners\n");
    IplImage* TempEig = cvCreateImage(cvGetSize(CurrentGray), 32, 1);
    IplImage* Temp = cvCreateImage(cvGetSize(CurrentGray), 32, 1);
    double MinDistance = (CurrentImage.GetWidth() + CurrentImage.GetHeight()) / 20;
    HUOFPointsNumber = MaxTrackedPoints = CurrentImage.GetWidth()*CurrentImage.GetHeight() / 1000;

    // Search good trackable points
    cvGoodFeaturesToTrack(PreviousGray, TempEig, Temp,
      HUOFPoints[0], &HUOFPointsNumber,
      0.01, MinDistance, NULL, 3);
    MaxTrackedPoints = HUOFPointsNumber;
    // Release temporary images
    cvReleaseImage(&TempEig);
    cvReleaseImage(&Temp);
    // Realloc the point status array
    if (PointsStatus)
    {
      cvFree(&PointsStatus);
      PointsStatus = NULL;
    }

    if (MaxTrackedPoints < 2)
    {
      HUOFDataState = ps_Initialized;
      HUOFPointsNumber = CurrentImage.GetWidth()*CurrentImage.GetHeight() / 1000;
      return;
    }
    else
      HUOFDataState = ps_Successful;
    PointsStatus = (char*)cvAlloc(HUOFPointsNumber);
  }

  cvCalcOpticalFlowPyrLK(PreviousGray, CurrentGray, HUOFPrevPyramid, HUOFPyramid,
    HUOFPoints[0], HUOFPoints[1], HUOFPointsNumber,
    cvSize(10, 10), 3, PointsStatus, NULL,
    cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 5, 1), 0);

  // Count the distances of the tracked points
  int **Distances = new int*[HUOFPointsNumber];
  for (int i = 0; i < HUOFPointsNumber; ++i)
    Distances[i] = new int[3];

  int DistanceMax = 0;
  for (i = 0; i < HUOFPointsNumber; ++i)
  {
    int DiffX = (int)MERound(HUOFPoints[1][i].x - HUOFPoints[0][i].x);
    int DiffY = (int)MERound(HUOFPoints[1][i].y - HUOFPoints[0][i].y);
    if ((PointsStatus[i] == 1) && !((DiffX == 0) && (DiffY == 0)))
    {
      bool found = false;
      // Create a list from the differences to count them
      for (i1 = 0; i1 < DistanceMax; ++i1)
      {
        if ((Distances[i1][0] == DiffX) &&
          (Distances[i1][1] == DiffY))
        {
          Distances[i1][2]++;
          found = true;
          break;
        }
      }
      if ((!found) && !((DiffX == 0) && (DiffY == 0)))
      {
        Distances[DistanceMax][0] = (int)MERound(HUOFPoints[1][i].x - HUOFPoints[0][i].x);
        Distances[DistanceMax][1] = (int)MERound(HUOFPoints[1][i].y - HUOFPoints[0][i].y);
        Distances[DistanceMax][2] = 1;
        DistanceMax++;
      }
    }
  }

  // Sort the results
  for (int i1 = DistanceMax - 1; i1 >= 2; --i1)
  {
    for (int i = i1; i >= 1; --i)
    {
      if ((Distances[i][2] > Distances[i - 1][2]) ||
        ((Distances[i][2] == Distances[i - 1][2]) &&
        (abs(Distances[i][0]) + abs(Distances[i][1]) <
          abs(Distances[i - 1][0]) + abs(Distances[i - 1][1]))))
      {
        int tmp = Distances[i][0];
        int tmp2 = Distances[i][1];
        int tmp3 = Distances[i][2];

        Distances[i][0] = Distances[i - 1][0];
        Distances[i][1] = Distances[i - 1][1];
        Distances[i][2] = Distances[i - 1][2];

        Distances[i - 1][0] = tmp;
        Distances[i - 1][1] = tmp2;
        Distances[i - 1][2] = tmp3;
      }
    }
  }

  float MoveX = 0.0;
  float MoveY = 0.0;
  int SampleNums = 0;
  float DistanceMeasure = 0.0;

  // Calculate the final camera movement
  for (i = 0; i < DistanceMax; ++i)
  {
    if ((Distances[i][2] <= MaxTrackedPoints / 10))
      break;

    if (i > 0)
    {
      DistanceMeasure += (Distances[i][0] - Distances[i - 1][0])*(Distances[i][0] - Distances[i - 1][0]);
      DistanceMeasure += (Distances[i][1] - Distances[i - 1][1])*(Distances[i][1] - Distances[i - 1][1]);
    }

    MoveX += Distances[i][0] * Distances[i][2];
    MoveY += Distances[i][1] * Distances[i][2];
    SampleNums += Distances[i][2];
  }

  if (SampleNums > 0)
  {
    MoveX = MERound(MoveX / SampleNums);
    MoveY = MERound(MoveY / SampleNums);
  }

  if (!((MoveX == 0) && (MoveY == 0)) &&
    (SampleNums > MaxTrackedPoints / 2))
  {
    HUOFCamMovementX += (int)MoveX;
    int HUOFCamMovementY = (int)MoveY;
    int MaxX = (HUImageWidth / 2) - 1;
    int MaxY = HUImageHeight - 1;
    /*
    printf("-----------\n");

    for (i = 0; i < DistanceMax; ++i)
    printf("%d: %d,%d\n", Distances[i][2], Distances[i][0], Distances[i][1]);

    printf("FINAL: %d,%d,%1.2f\n", (int)MoveX, (int)MoveY, DistanceMeasure);
    printf("-----------\n");
    printf("Camera movement: %d,%d,%d (max: %d, current: %d)\n",
    SampleNums, HUOFCamMovementX, HUOFCamMovementY, MaxTrackedPoints, HUOFPointsNumber);
    */
    HUOFFrames = 0;
    HUOFCamMovement = true;

    if (!(HUOFCamMovementY == 0 && HUOFCamMovementX >= -1 && HUOFCamMovementX <= 1))
    {
      MEPixelDataType ***PreviousData = new MEPixelDataType**[MaxX + 1];

      for (int i = 0; i < MaxX + 1; ++i)
        PreviousData[i] = new MEPixelDataType*[MaxY + 1];

      // Camera movement being happened
      for (int y = MaxY; y >= 0; --y)
      {
        for (int x = MaxX; x >= 0; --x)
        {
          PreviousData[x][y] = NULL;
        }
      }

      // Move the LBP data to new locations
      for (int y = MaxY; y >= 0; --y)
      {
        for (int x = MaxX; x >= 0; --x)
        {
          int NewX = x + (HUOFCamMovementX / 2);
          int NewY = y + HUOFCamMovementY;

          if (NewX >= 0 && NewX <= MaxX &&
            NewY >= 0 && NewY <= MaxY)
          {
            if (HULBPPixelData[NewX][NewY])
            {
              PreviousData[NewX][NewY] = HULBPPixelData[NewX][NewY];
              HULBPPixelData[NewX][NewY] = NULL;
              if (PreviousData[x][y])
              {
                HULBPPixelData[NewX][NewY] = PreviousData[x][y];
                PreviousData[x][y] = NULL;
              }
              else
              {
                HULBPPixelData[NewX][NewY] = HULBPPixelData[x][y];
                HULBPPixelData[x][y] = NULL;
              }
            }
            else
            {
              if (PreviousData[x][y])
              {
                HULBPPixelData[NewX][NewY] = PreviousData[x][y];
                PreviousData[x][y] = NULL;
              }
              else
              {
                HULBPPixelData[NewX][NewY] = HULBPPixelData[x][y];
                HULBPPixelData[x][y] = NULL;
              }
            }
          }
          else
          {
            if (HULBPPixelData[x][y])
            {
              delete[] HULBPPixelData[x][y]->PreviousHistogram;
              for (int i2 = 0; i2 < HUHistogramsPerPixel; ++i2)
                delete[] HULBPPixelData[x][y]->Histograms[i2];
              delete[] HULBPPixelData[x][y]->Histograms;
              delete[] HULBPPixelData[x][y]->BackgroundHistogram;
              delete[] HULBPPixelData[x][y]->Weights;
              delete HULBPPixelData[x][y];
              HULBPPixelData[x][y] = NULL;
            }
          }
        }
      }

      // Release unused data
      for (int y = MaxY; y >= 0; --y)
      {
        for (int x = MaxX; x >= 0; --x)
        {
          if (PreviousData[x][y])
          {
            delete[] PreviousData[x][y]->PreviousHistogram;
            for (int i2 = 0; i2 < HUHistogramsPerPixel; ++i2)
              delete[] PreviousData[x][y]->Histograms[i2];
            delete[] PreviousData[x][y]->Histograms;
            delete[] PreviousData[x][y]->BackgroundHistogram;
            delete[] PreviousData[x][y]->Weights;
            delete PreviousData[x][y];
            PreviousData[x][y] = NULL;
          }
        }
      }

      HUOFCamMovementX = HUOFCamMovementX % 1;

      for (int i = 0; i < MaxX + 1; ++i)
        delete[] PreviousData[i];
      delete[] PreviousData;
    }
  }

  i1 = 0;
  // Throw the missed points away
  for (i = 0; i < HUOFPointsNumber; ++i)
  {
    if (PointsStatus[i] == 1)
    {
      HUOFPoints[0][i1] = HUOFPoints[1][i];
      i1++;
    }
  }
  HUOFPointsNumber -= i + 1 - i1;

  if (HUOFPointsNumber < MaxTrackedPoints / 2)
  {
    printf("Re-init the optical flow\n");
    HUOFDataState = ps_Initialized;
    HUOFPointsNumber = CurrentImage.GetWidth()*CurrentImage.GetHeight() / 1000;
  }
  // Free memory
  if (PreviousGray != PreviousImage.GetIplImage())
    cvReleaseImage(&PreviousGray);
  if (CurrentGray != CurrentImage.GetIplImage())
    cvReleaseImage(&CurrentGray);
  cvFree(&PointsStatus);

  for (int i = 0; i < HUOFPointsNumber; ++i)
    delete[] Distances[i];
  delete[] Distances;
}


void MotionDetection::GetMotionsMaskHU(MEImage& mask_image)
{
  if (MDDataState != ps_Successful)
  {
    mask_image.Clear();
    return;
  }

  // Reallocate the mask image if needs be
  if ((HUImageWidth + HUHistogramArea - 1 != mask_image.GetWidth()) ||
    (HUImageHeight + HUHistogramArea - 1 != mask_image.GetHeight()) ||
    (mask_image.GetLayers() != 1))
  {
    mask_image.Realloc(HUImageWidth + HUHistogramArea - 1,
      HUImageHeight + HUHistogramArea - 1, 1);
  }
  mask_image.Clear();
  // Generate the mask image
  unsigned char* MaskImgData = mask_image.GetImageData();
  int RowStart = (mask_image.GetHeight() - HUHistogramArea / 2)*mask_image.GetRowWidth();
  int RowWidth = mask_image.GetRowWidth();

  // Generate a graph about the histogram data
  Graph::node_id **Nodes = new Graph::node_id*[HUImageWidth / 2];
  for (int i = 0; i < HUImageWidth / 2; ++i)
    Nodes[i] = new Graph::node_id[HUImageHeight];
  Graph *LBPGraph = new Graph();

  for (int x = (HUImageWidth / 2) - 1; x >= 0; --x)
  {
    for (int y = HUImageHeight - 1; y >= 0; --y)
    {
      Nodes[x][y] = LBPGraph->add_node();
    }
  }

  for (int x = (HUImageWidth / 2) - 1; x >= 0; --x)
  {
    for (int y = HUImageHeight - 1; y >= 0; --y)
    {
      LBPGraph->set_tweights(Nodes[x][y], 1,
        (short int)(HUMinCutWeight*(1 - HULBPPixelData[x][y]->BackgroundRate)));

      if (x > 0 && y > 0)
      {
        LBPGraph->add_edge(Nodes[x][y], Nodes[x - 1][y], 1, 1);
        LBPGraph->add_edge(Nodes[x][y], Nodes[x][y - 1], 1, 1);
      }
    }
  }

  LBPGraph->maxflow();

  for (int x = (HUImageWidth / 2) - 1; x >= 0; --x)
  {
    for (int y = HUImageHeight - 1; y >= 0; --y)
    {
      if (LBPGraph->what_segment(Nodes[x][y]) == Graph::SINK)
        HULBPPixelData[x][y]->BackgroundRate = 0.0;
      else
        HULBPPixelData[x][y]->BackgroundRate = 1.0;
    }
  }

  delete LBPGraph;
  LBPGraph = NULL;
  for (int y = HUImageHeight - 1; y >= 0; --y)
  {
    for (int x = HUImageWidth - 1; x >= 0; --x)
    {
      if (y % 2 == (x + 1) % 2)
        MaskImgData[RowStart + x + (HUHistogramArea / 2)] =
        (HULBPPixelData[x / 2][y]->BackgroundRate == 0.0) ? 255 : 0;
      else
      {
        MaskImgData[RowStart + x + (HUHistogramArea / 2)] =
          ((int)(x > 1 && HULBPPixelData[(x / 2) - 1][y]->BackgroundRate == 0.0) +
          (int)(x < mask_image.GetWidth() - HUHistogramArea - 1 &&
            HULBPPixelData[(x / 2) + 1][y]->BackgroundRate == 0.0) +
            (int)(y > 0 && HULBPPixelData[x / 2][y - 1]->BackgroundRate == 0.0) +
            (int)(y < mask_image.GetHeight() - HUHistogramArea &&
              HULBPPixelData[x / 2][y + 1]->BackgroundRate == 0.0) > 1)
          ? 255 : 0;
      }
    }
    RowStart -= RowWidth;
  }

  cvFloodFill(mask_image.GetIplImage(), cvPoint(0, 0), cvScalar(128, 128, 128, 128),
    cvScalar(0, 0, 0, 0), cvScalar(0, 0, 0, 0));
  for (int i = ((IplImage*)mask_image.GetIplImage())->widthStep*((IplImage*)mask_image.GetIplImage())->height - 1; i >= 0; --i)
  {
    if (MaskImgData[i] == 128)
    {
      MaskImgData[i] = 0;
    }
    else
    {
      if (MaskImgData[i] == 0)
      {
        MaskImgData[i] = 255;
      }
    }
  }
  // Apply an erode operator
  mask_image.Erode(1);

  for (int i = 0; i < HUImageWidth / 2; ++i)
    delete[] Nodes[i];
  delete[] Nodes;
}


void MotionDetection::SetSampleMaskHU(SampleMaskType mask_type, int desiredarea)
{
  if (HUMaskColumnAddDel == NULL || HUMaskRowAddDel == NULL)
  {
    printf("Auxiliary variables are NULL\n");
    return;
  }

  // Generate a mask for computing the histograms
  IplImage *MaskImage = cvCreateImage(cvSize(HUHistogramArea, HUHistogramArea), 8, 1);
  int DesiredArea = desiredarea <= 0 ? HUHistogramBins * 2 : desiredarea;

  int **CalculationMask = new int*[HUHistogramArea];
  for (int i = 0; i < HUHistogramArea; ++i)
    CalculationMask[i] = new int[HUHistogramArea];

  int SquareSide = (int)MERound(sqrt((float)DesiredArea));
  int CircleRadius = (int)MERound(sqrt((float)DesiredArea / ME_PI_VALUE));
  int EllipseA = (int)MERound(HUHistogramArea / 2 + 1);
  int EllipseB = (int)MERound(DesiredArea / (EllipseA*1.2*ME_PI_VALUE));

  cvSetZero(MaskImage);

  switch (mask_type)
  {
  case sm_Circle:
    cvCircle(MaskImage, cvPoint(HUHistogramArea / 2, HUHistogramArea / 2),
      CircleRadius, CV_RGB(1, 1, 1), -1);
    break;

  case sm_Square:
    cvRectangle(MaskImage,
      cvPoint(HUHistogramArea / 2 - SquareSide / 2, HUHistogramArea / 2 - SquareSide / 2),
      cvPoint(HUHistogramArea / 2 + SquareSide / 2, HUHistogramArea / 2 + SquareSide / 2),
      CV_RGB(1, 1, 1), -1);
    break;

  case sm_Ellipse:
    cvEllipse(MaskImage, cvPoint(HUHistogramArea / 2, HUHistogramArea / 2),
      cvSize(EllipseA, EllipseB), 45, 0, 360,
      CV_RGB(1, 1, 1), -1);
    break;

  case sm_RandomPixels:
    HUSamplePixels = 0;
    while (HUSamplePixels != DesiredArea)
    {
      int i = rand() % HUHistogramArea;
      int j = rand() % HUHistogramArea;

      if (MaskImage->imageData[i*MaskImage->widthStep + j] == 0)
      {
        MaskImage->imageData[i*MaskImage->widthStep + j] = 1;
        HUSamplePixels++;
      }
    }
    break;

  default:
    cvCircle(MaskImage, cvPoint(HUHistogramArea / 2, HUHistogramArea / 2),
      (int)MERound(sqrt((float)DesiredArea / ME_PI_VALUE)), CV_RGB(1, 1, 1), -1);
    break;
  }

  HUSamplePixels = 0;
  //memset(CalculationMask, 0, sizeof(CalculationMask));

  for (int i = 0; i < HUHistogramArea; ++i)
  {
    for (int i1 = 0; i1 < HUHistogramArea; ++i1)
    {
      if (MaskImage->imageData[i*MaskImage->widthStep + i1] != 0)
      {
        HUSamplePixels++;
        CalculationMask[i][i1] = 1;
      }
      else {
        CalculationMask[i][i1] = 0;
      }
    }
  }

  // Fill an auxiliary variable for fast computing with data
  for (int i = 0; i < HUHistogramArea; ++i)
  {
    HUMaskColumnAddDel[i][0] = -1;
    for (int i1 = 0; i1 < HUHistogramArea; ++i1)
    {
      if (CalculationMask[i][i1] != 0)
      {
        HUMaskColumnAddDel[i][0] = i1;
        break;
      }
    }
    HUMaskColumnAddDel[i][1] = -1;
    for (int i1 = HUHistogramArea - 1; i1 >= 0; --i1)
    {
      if (CalculationMask[i][i1] != 0)
      {
        HUMaskColumnAddDel[i][1] = i1 + 1;
        break;
      }
    }
  }

  // Fill an auxiliary variable for fast computing with data
  for (int i = 0; i < HUHistogramArea; ++i)
  {
    HUMaskRowAddDel[i][0] = -1;
    for (int i1 = 0; i1 < HUHistogramArea; ++i1)
    {
      if (CalculationMask[i1][i] != 0)
      {
        HUMaskRowAddDel[i][0] = i1;
        break;
      }
    }
    HUMaskRowAddDel[i][1] = -1;
    for (int i1 = HUHistogramArea - 1; i1 >= 0; --i1)
    {
      if (CalculationMask[i1][i] != 0)
      {
        HUMaskRowAddDel[i][1] = i1 + 1;
        break;
      }
    }
  }

  // Freeing memory
  cvReleaseImage(&MaskImage);

  for (int i = 0; i < HUHistogramArea; ++i)
    delete[] CalculationMask[i];
  delete[] CalculationMask;
}
