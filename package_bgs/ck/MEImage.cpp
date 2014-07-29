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
 */

#include "MEImage.hpp"

#include <opencv2/opencv.hpp>

#include "MEDefs.hpp"

#define ME_CAST_TO_IPLIMAGE(image_ptr) ((IplImage*)image_ptr)
#define ME_RELEASE_IPLIMAGE(image_ptr) \
  cvReleaseImage((IplImage**)&image_ptr); \
  image_ptr = NULL;

// RGB to YUV transform
const float RGBtoYUVMatrix[3][3] =
  {{ 0.299, 0.587, 0.114 },
  { -0.147, -0.289, 0.436 },
  { 0.615, -0.515, -0.100 }};

// RGB to YIQ transform
const float RGBtoYIQMatrix[3][3] =
  {{ 0.299, 0.587, 0.114 },
  { 0.596, -0.274, -0.322 },
  { 0.212, -0.523, 0.311 }};

MEImage::MEImage(int width, int height, int layers) : cvImg(NULL)
{
  _Init(width, height, layers);
}


MEImage::MEImage(const MEImage& other) : cvImg(NULL)
{
  _Copy(other);
}


MEImage::~MEImage()
{
  if (ME_CAST_TO_IPLIMAGE(cvImg))
  {
    ME_RELEASE_IPLIMAGE(cvImg);
  }
}


void MEImage::Clear()
{
  cvSetZero(ME_CAST_TO_IPLIMAGE(cvImg));
}


void MEImage::GetLayer(MEImage& new_layer, int layer_number) const
{
  int LayerNumber = layer_number;

  if ((new_layer.GetWidth() != ME_CAST_TO_IPLIMAGE(cvImg)->width) ||
      (new_layer.GetHeight() != ME_CAST_TO_IPLIMAGE(cvImg)->height) ||
      (new_layer.GetLayers() != 1))
  {
    new_layer.Realloc(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height, 1);
  }
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels < LayerNumber)
  {
    printf("The given layer number is too large (%d > %d)\n",
               LayerNumber, ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);

    LayerNumber = ME_CAST_TO_IPLIMAGE(cvImg)->nChannels;
  }
  if (LayerNumber <= 0)
  {
    printf("The given layer number is too small (%d <= 0)\n", LayerNumber);
    LayerNumber = 1;
  }

  cvSetImageCOI(ME_CAST_TO_IPLIMAGE(cvImg), LayerNumber);
  cvCopy(ME_CAST_TO_IPLIMAGE(cvImg), (IplImage*)new_layer.GetIplImage(), NULL);
  cvSetImageCOI(ME_CAST_TO_IPLIMAGE(cvImg), 0);
}


void MEImage::SetLayer(MEImage& layer, int layer_number)
{
  int LayerNumber = layer_number;

  if (layer.GetWidth() != ME_CAST_TO_IPLIMAGE(cvImg)->width ||
      layer.GetHeight() != ME_CAST_TO_IPLIMAGE(cvImg)->height)
  {
    printf("The dimensions of the layer and "
               "destination image is different (%dx%d <> %dx%d)\n",
               layer.GetWidth(), layer.GetHeight(), ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height);
    return;
  }
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels < LayerNumber)
  {
    printf("The given layer number is too large (%d > %d)\n",
               LayerNumber, ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
    LayerNumber = ME_CAST_TO_IPLIMAGE(cvImg)->nChannels;
  }
  if (LayerNumber <= 0)
  {
    printf("The given layer number is too small (%d <= 0)\n", LayerNumber);
    LayerNumber = 1;
  }
  if (layer.GetLayers() != 1)
  {
    printf("The layer image has not one color channel (1 != %d)\n",
               layer.GetLayers());
    return;
  }
  cvSetImageCOI(ME_CAST_TO_IPLIMAGE(cvImg), LayerNumber);
  cvCopy((IplImage*)layer.GetIplImage(), ME_CAST_TO_IPLIMAGE(cvImg), NULL);
  cvSetImageCOI(ME_CAST_TO_IPLIMAGE(cvImg), 0);
}


void MEImage::CopyImageData(unsigned char* data)
{
  memcpy(ME_CAST_TO_IPLIMAGE(cvImg)->imageData, data, ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->height*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
}


void* MEImage::GetIplImage() const
{
  return (void*)ME_CAST_TO_IPLIMAGE(cvImg);
}


void MEImage::SetIplImage(void* image)
{
  if (ME_CAST_TO_IPLIMAGE(cvImg))
  {
    ME_RELEASE_IPLIMAGE(cvImg);
  }
  cvImg = cvCloneImage((IplImage*)image);
  // Correct the origin of the image
  if (ME_CAST_TO_IPLIMAGE(cvImg)->origin == 1)
  {
    MirrorVertical();
    ME_CAST_TO_IPLIMAGE(cvImg)->origin = 0;
  }
}


bool MEImage::operator==(const MEImage& image)
{
  return Equal(image);
}


bool MEImage::operator!=(const MEImage& image)
{
  return !operator==(image);
}


MEImage& MEImage::operator=(const MEImage& other_image)
{
  if (&other_image == this)
    return *this;

  _Copy(other_image);
  return *this;
}


int MEImage::GetWidth() const
{
  return ME_CAST_TO_IPLIMAGE(cvImg) ? ME_CAST_TO_IPLIMAGE(cvImg)->width : 0;
}


int MEImage::GetRowWidth() const
{
  return ME_CAST_TO_IPLIMAGE(cvImg) ? ME_CAST_TO_IPLIMAGE(cvImg)->widthStep : 0;
}


int MEImage::GetHeight() const
{
  return ME_CAST_TO_IPLIMAGE(cvImg) ? ME_CAST_TO_IPLIMAGE(cvImg)->height : 0;
}


int MEImage::GetLayers() const
{
  return ME_CAST_TO_IPLIMAGE(cvImg) ? ME_CAST_TO_IPLIMAGE(cvImg)->nChannels : 0;
}


int MEImage::GetPixelDataNumber() const
{
  return ME_CAST_TO_IPLIMAGE(cvImg) ? GetWidth()*GetHeight()*GetLayers() : 0;
}


unsigned char* MEImage::GetImageData() const
{
  return ME_CAST_TO_IPLIMAGE(cvImg) ? (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData : NULL;
}


void MEImage::SetData(unsigned char* image_data, int width, int height, int channels)
{
  _Init(width, height, channels);

  for (int y = height-1; y >= 0; --y)
  {
    int Start = GetRowWidth()*y;
    int Start2 = width*channels*y;

    memcpy(&ME_CAST_TO_IPLIMAGE(cvImg)->imageData[Start], &image_data[Start2], width*channels);
  }
}


float MEImage::GetRatio() const
{
  return ME_CAST_TO_IPLIMAGE(cvImg) ? (float)ME_CAST_TO_IPLIMAGE(cvImg)->height/(float)ME_CAST_TO_IPLIMAGE(cvImg)->width : 0.0;
}


void MEImage::Realloc(int width, int height)
{
  Realloc(width, height, ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
}


void MEImage::Realloc(int width, int height, int layers)
{
  _Init(width, height, layers);
}


void MEImage::Resize(int new_width, int new_height)
{
  if (new_height < 1)
  {
    printf("Invalid new height: %d < 1\n", new_height);
    return;
  }
  if (new_width < 1)
  {
    printf("Invalid new width: %d < 1\n", new_width);
    return;
  }
  IplImage* TempImg = cvCreateImage(cvSize(new_width, new_height), 8, ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);

  cvResize(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_INTER_NN);
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::ResizeScaleX(int new_width)
{
  if (new_width < 1)
  {
    printf("Invalid new width: %d < 1\n", new_width);
    return;
  }
  Resize(new_width, (int)((float)new_width*GetRatio()));
}


void MEImage::ResizeScaleY(int new_height)
{
  if (new_height < 1)
  {
    printf("Invalid new height: %d < 1\n", new_height);
    return;
  }
  Resize((int)((float)new_height*1/GetRatio()), new_height);
}


void MEImage::MirrorHorizontal()
{
  cvFlip(ME_CAST_TO_IPLIMAGE(cvImg), NULL, 1);
}


void MEImage::MirrorVertical()
{
  cvFlip(ME_CAST_TO_IPLIMAGE(cvImg), NULL, 0);
}


void MEImage::Crop(int x1, int y1, int x2, int y2)
{
  int NewX1 = x1;
  int NewY1 = y1;
  int NewX2 = x2;
  int NewY2 = y2;

  NewX1 = (NewX1 < 0) ? 0 : NewX1;
  NewX1 = (NewX1 > ME_CAST_TO_IPLIMAGE(cvImg)->width) ? ME_CAST_TO_IPLIMAGE(cvImg)->width : NewX1;
  NewY1 = (NewY1 < 0) ? 0 : NewY1;
  NewY1 = (NewY1 > ME_CAST_TO_IPLIMAGE(cvImg)->height) ? ME_CAST_TO_IPLIMAGE(cvImg)->height : NewY1;

  NewX2 = (NewX2 < 0) ? 0 : NewX2;
  NewX2 = (NewX2 > ME_CAST_TO_IPLIMAGE(cvImg)->width) ? ME_CAST_TO_IPLIMAGE(cvImg)->width : NewX2;
  NewY2 = (NewY2 < 0) ? 0 : NewY2;
  NewY2 = (NewY2 > ME_CAST_TO_IPLIMAGE(cvImg)->height) ? ME_CAST_TO_IPLIMAGE(cvImg)->height : NewY2;

  if ((NewX2-NewX1) <= 0)
  {
    printf("Invalid new width: %d <= 0\n", NewX2-NewX1);
    return;
  }
  if ((NewY2-NewY1) <= 0)
  {
    printf("Invalid new height: %d <= 0\n", NewY2-NewY1);
    return;
  }
  IplImage* TempImg = cvCreateImage(cvSize(NewX2-NewX1, NewY2-NewY1), 8, ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);

  cvSetImageROI(ME_CAST_TO_IPLIMAGE(cvImg), cvRect(NewX1, NewY1, NewX2-NewX1, NewY2-NewY1));
  cvCopy(ME_CAST_TO_IPLIMAGE(cvImg), TempImg);
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::CopyImageInside(int x, int y, MEImage& source_image)
{
  int NewX = x;
  int NewY = y;
  int PasteLengthX = source_image.GetWidth();
  int PasteLengthY = source_image.GetHeight();

  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels != source_image.GetLayers())
  {
    if (source_image.GetLayers() == 1 && ME_CAST_TO_IPLIMAGE(cvImg)->nChannels == 3)
    {
      source_image.ConvertGrayscaleToRGB();
    }
    if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels == 1 && source_image.GetLayers() == 3)
    {
      source_image.ConvertToGrayscale(g_OpenCV);
    }
  }
  if (NewX < 0)
    NewX = 0;
  if (NewX > ME_CAST_TO_IPLIMAGE(cvImg)->width)
    NewX = ME_CAST_TO_IPLIMAGE(cvImg)->width;
  if (NewY < 0)
    NewY = 0;
  if (NewY > ME_CAST_TO_IPLIMAGE(cvImg)->height)
    NewY = ME_CAST_TO_IPLIMAGE(cvImg)->height;
  if (NewX+PasteLengthX > ME_CAST_TO_IPLIMAGE(cvImg)->width)
    PasteLengthX = ME_CAST_TO_IPLIMAGE(cvImg)->width-NewX;
  if (NewY+PasteLengthY > ME_CAST_TO_IPLIMAGE(cvImg)->height)
    PasteLengthY = ME_CAST_TO_IPLIMAGE(cvImg)->height-NewY;

  if (PasteLengthX != source_image.GetWidth() ||
      PasteLengthY != source_image.GetHeight())
  {
    source_image.Resize(PasteLengthX, PasteLengthY);
  }
  cvSetImageROI(ME_CAST_TO_IPLIMAGE(cvImg), cvRect(NewX, NewY, PasteLengthX, PasteLengthY));
  cvCopy((IplImage*)source_image.GetIplImage(), ME_CAST_TO_IPLIMAGE(cvImg));
  cvResetImageROI(ME_CAST_TO_IPLIMAGE(cvImg));
}


void MEImage::Erode(int iterations)
{
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width,
                                           ME_CAST_TO_IPLIMAGE(cvImg)->height),
                                    8, ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);

  cvErode(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, NULL, iterations);
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::Dilate(int iterations)
{
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width,
                                           ME_CAST_TO_IPLIMAGE(cvImg)->height),
                                    8, ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);

  cvDilate(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, NULL, iterations);
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::Smooth()
{
  SmoothAdvanced(s_Median, 3);
}


void MEImage::SmoothAdvanced(SmoothType filtermode, int filtersize)
{
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                                    ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);

  switch (filtermode)
  {
    case s_Blur:
      cvSmooth(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_BLUR, filtersize, filtersize, 0);
      break;
    case s_Median:
      cvSmooth(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_MEDIAN, filtersize, 0, 0);
      break;
    case s_Gaussian:
      cvSmooth(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_GAUSSIAN, filtersize, filtersize, 0);
      break;
    default:
      cvSmooth(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_MEDIAN, filtersize, 0, 0);
      break;
  }
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::Canny()
{
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels > 1)
  {
    ConvertToGrayscale(g_OpenCV);
  }

  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                                    ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
  cvCanny(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, 800, 1100, 5);
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::Laplace()
{
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels != 1)
  {
    ConvertToGrayscale(g_OpenCV);
  }
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width,
                                           ME_CAST_TO_IPLIMAGE(cvImg)->height),
                                    IPL_DEPTH_16S, 1);
  cvLaplace(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, 3);
  cvConvertScale(TempImg, ME_CAST_TO_IPLIMAGE(cvImg), 1, 0);
  ME_RELEASE_IPLIMAGE(cvImg);
}


void MEImage::Quantize(int levels)
{
  if (levels <= 0)
  {
    printf("Level number is too small (%d <= 0)\n", levels);
    return;
  }
  if (levels > 256)
  {
    printf("Level number is too large (%d > 256)\n", levels);
    return;
  }
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;

  for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*ME_CAST_TO_IPLIMAGE(cvImg)->height-1; i >= 0; --i)
  {
    ImageData[i] = ImageData[i] / (256 / levels)*(256 / levels);
  }
}


void MEImage::Threshold(int threshold_limit)
{
  if (threshold_limit < 0)
  {
    printf("Threshold number is too small (%d <= 0)\n", threshold_limit);
    return;
  }
  if (threshold_limit > 255)
  {
    printf("Threshold number is too large (%d > 255)\n", threshold_limit);
    return;
  }
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;

  for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*ME_CAST_TO_IPLIMAGE(cvImg)->height-1; i >= 0; --i)
  {
    if (ImageData[i] < threshold_limit)
    {
      ImageData[i] = 0;
    }
  }
}


void MEImage::AdaptiveThreshold()
{
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels != 1)
  {
    ConvertToGrayscale(g_OpenCV);
  }
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                                    ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
  cvAdaptiveThreshold(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, 25,
                      CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 7, -7);
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::ThresholdByMask(MEImage& mask_image)
{
  if (mask_image.GetWidth() != ME_CAST_TO_IPLIMAGE(cvImg)->width ||
      mask_image.GetHeight() != ME_CAST_TO_IPLIMAGE(cvImg)->height)
  {
    printf("Image properties are different\n");
    return;
  }
  if (mask_image.GetLayers() != 3 && ME_CAST_TO_IPLIMAGE(cvImg)->nChannels == 3)
  {
    mask_image.ConvertGrayscaleToRGB();
  }
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* MaskImageData = mask_image.GetImageData();

  for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*ME_CAST_TO_IPLIMAGE(cvImg)->height-1; i >= 0; --i)
  {
    if (MaskImageData[i] == 0)
    {
      ImageData[i] = 0;
    }
  }
}


void MEImage::ColorSpace(ColorSpaceConvertType mode)
{
  IplImage* TempImg = NULL;
  unsigned char* ImageData = NULL;
  int WidthStep = 0;
  int RowStart = 0;

  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels == 1)
  {
    printf("No sense to convert: source image is greyscale\n");
    ConvertGrayscaleToRGB();
  }
  switch (mode)
  {
    case csc_RGBtoXYZCIED65:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width,
                                     ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_RGB2XYZ);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_XYZCIED65toRGB:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width,
                                     ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_XYZ2RGB);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_RGBtoHSV:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width,
                                     ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_RGB2HSV);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_HSVtoRGB:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width,
                                     ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_HSV2RGB);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_RGBtoHLS:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_RGB2HLS);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_HLStoRGB:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_HLS2RGB);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_RGBtoCIELab:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_RGB2Lab);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_CIELabtoRGB:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_Lab2RGB);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_RGBtoCIELuv:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_RGB2Luv);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_CIELuvtoRGB:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                              ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_Luv2RGB);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case csc_RGBtoYUV:
      ComputeColorSpace(csc_RGBtoYUV);
      break;

    case csc_RGBtoYIQ:
      ComputeColorSpace(csc_RGBtoYIQ);
      break;

    case csc_RGBtorgI:
      ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
      WidthStep = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;
      RowStart = 0;
      for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
      {
        for (int x = (ME_CAST_TO_IPLIMAGE(cvImg)->width-1)*3; x >= 0; x -= 3)
        {
          int r = 0;
          int g = 0;
          int I = 0;

          I = (int)ImageData[RowStart+x]+(int)ImageData[RowStart+x+1]+(int)ImageData[RowStart+x+2];
          r = (int)((float)ImageData[RowStart+x] / I*255);
          g = (int)((float)ImageData[RowStart+x+1] / I*255);
          ImageData[RowStart+x] = (unsigned char)r;
          ImageData[RowStart+x+1] = (unsigned char)g;
          ImageData[RowStart+x+2] = (unsigned char)(I / 3);
        }
        RowStart += WidthStep;
      }
      break;

    default:
      break;
  }
}


void MEImage::ConvertToGrayscale(GrayscaleType grayscale_mode)
{
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels == 1)
  {
    printf("Image is already grayscale\n");
    return;
  }
  IplImage* TempImg = NULL;
  unsigned char* ImgData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* ImageData = NULL;

  switch (grayscale_mode)
  {
    case g_Average:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8, 1);
      ImageData = (unsigned char*)TempImg->imageData;

      for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*ME_CAST_TO_IPLIMAGE(cvImg)->height-3; i >= 0; i -= 3)
      {
        ImageData[i / 3] = (ImgData[i]+ImgData[i+1]+ImgData[i+2]) / 3;
      }
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    case g_OpenCV:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8, 1);
      cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_RGB2GRAY);
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    default:
      break;
  }
}


void MEImage::ConvertGrayscaleToRGB()
{
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels != 1)
  {
    return;
  }
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8, 3);

  cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), TempImg, CV_GRAY2RGB);
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::ConvertBGRToRGB()
{
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels != 3)
  {
    return;
  }
  cvCvtColor(ME_CAST_TO_IPLIMAGE(cvImg), ME_CAST_TO_IPLIMAGE(cvImg), CV_RGB2BGR);
}


void MEImage::LBP(LBPType mode)
{
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels > 1)
  {
    ConvertToGrayscale(g_OpenCV);
  }
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8, 1);
  unsigned char* TempImgData = (unsigned char*)TempImg->imageData;
  int WidthStep = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;
  int WidthStep_2 = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*2;

  cvSetZero(TempImg);
  switch (mode)
  {
    case lbp_Normal:
      for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*(ME_CAST_TO_IPLIMAGE(cvImg)->height-2)-1; i >= ME_CAST_TO_IPLIMAGE(cvImg)->widthStep+1; --i)
      {
        TempImgData[i] =
            (ImageData[i] <= ImageData[i-ME_CAST_TO_IPLIMAGE(cvImg)->widthStep-1])+
            ((ImageData[i] <= ImageData[i-ME_CAST_TO_IPLIMAGE(cvImg)->widthStep])*2)+
            ((ImageData[i] <= ImageData[i-ME_CAST_TO_IPLIMAGE(cvImg)->widthStep+1])*4)+
            ((ImageData[i] <= ImageData[i-1])*8)+
            ((ImageData[i] <= ImageData[i+1])*16)+
            ((ImageData[i] <= ImageData[i+ME_CAST_TO_IPLIMAGE(cvImg)->widthStep-1])*32)+
            ((ImageData[i] <= ImageData[i+ME_CAST_TO_IPLIMAGE(cvImg)->widthStep])*64)+
            ((ImageData[i] <= ImageData[i+ME_CAST_TO_IPLIMAGE(cvImg)->widthStep+1])*128);
      }
      break;

    case lbp_Special:
      for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*(ME_CAST_TO_IPLIMAGE(cvImg)->height-3)-2; i >= ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*2+2; --i)
      {
        int CenterPixel = (ImageData[i+1]+ImageData[i-1]+
                           ImageData[i-WidthStep]+ImageData[i+WidthStep]) / 4;
        TempImgData[i] = ((CenterPixel <= (ImageData[i-(WidthStep_2)-2]+
                           ImageData[i-(WidthStep_2)-1]+
                           ImageData[i-WidthStep-2]+
                           ImageData[i-WidthStep-1]) / 4))+
                         ((CenterPixel <= (ImageData[i-WidthStep]+
                                            ImageData[i-(WidthStep_2)]) / 2)*2)+
                         ((CenterPixel <= ((ImageData[i-(WidthStep_2)+2]+
                           ImageData[i-(WidthStep_2)+1]+
                           ImageData[i-WidthStep+2]+
                           ImageData[i-WidthStep+1]) / 4))*4)+
                         ((CenterPixel <= (ImageData[i-1]+
                                            ImageData[i-2]) / 2)*8)+
                         ((CenterPixel <= (ImageData[i+1]+
                                            ImageData[i+2]) / 2)*16)+
                         ((CenterPixel <= ((ImageData[i+(WidthStep_2)-2]+
                           ImageData[i+(WidthStep_2)-1]+
                           ImageData[i+WidthStep-2]+
                           ImageData[i+WidthStep-1]) / 4))*32)+
                         ((CenterPixel <= (ImageData[i+WidthStep]+
                                            ImageData[i-WidthStep_2]) / 2)*64)+
                         ((CenterPixel <= ((ImageData[i+(WidthStep_2)+2]+
                           ImageData[i+(WidthStep_2)+1]+
                           ImageData[i+WidthStep+2]+
                           ImageData[i+WidthStep+1]) / 4))*128);
      }
      break;

    default:
      break;
  }
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


void MEImage::Binarize(int threshold)
{
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;

  for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->height*ME_CAST_TO_IPLIMAGE(cvImg)->widthStep-1; i >= 0; --i)
  {
    if (ImageData[i] >= threshold)
    {
      ImageData[i] = 255;
    } else {
      ImageData[i] = 0;
    }
  }
}


void MEImage::Subtract(MEImage& source, SubtractModeType mode)
{
  if (source.GetWidth() != ME_CAST_TO_IPLIMAGE(cvImg)->width ||
      source.GetHeight() != ME_CAST_TO_IPLIMAGE(cvImg)->height ||
      source.GetLayers() != ME_CAST_TO_IPLIMAGE(cvImg)->nChannels)
  {
    printf("Image properties are different.\n");
    return;
  }
  unsigned char* ImageData = NULL;
  unsigned char* DstData = NULL;
  int WidthStep = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;
  int RowStart = 0;

  switch (mode)
  {
    case sub_Normal:
      ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
      DstData = source.GetImageData();
      RowStart = 0;

      for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
      {
        for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; x >= 0; --x)
        {
          ImageData[RowStart+x] =
            ImageData[RowStart+x]-DstData[RowStart+x] < 0 ? 0 :
            ImageData[RowStart+x]-DstData[RowStart+x];
        }
        RowStart += WidthStep;
      }
      break;

    case sub_Absolut:
      ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
      DstData = source.GetImageData();
      RowStart = 0;

      for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
      {
        for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; x >= 0; --x)
        {
          ImageData[RowStart+x] = ImageData[RowStart+x]-
              DstData[RowStart+x] < 0 ? -ImageData[RowStart+x]+
              DstData[RowStart+x] : ImageData[RowStart+x]-DstData[RowStart+x];
        }
        RowStart += WidthStep;
      }
      break;

    default:
      break;
  }
}


void MEImage::Multiple(MEImage& source, MultiplicationType mode)
{
  if (source.GetWidth() != ME_CAST_TO_IPLIMAGE(cvImg)->width ||
      source.GetHeight() != ME_CAST_TO_IPLIMAGE(cvImg)->height ||
      source.GetLayers() != ME_CAST_TO_IPLIMAGE(cvImg)->nChannels)
  {
    printf("Image properties are different.\n");
    return;
  }
  float Result = 0.0;
  IplImage* TempImg = NULL;
  unsigned char* ImageData = NULL;
  unsigned char* ImageData2 = NULL;
  unsigned char* ImageData3 = NULL;
  unsigned char* DstData = NULL;

  switch (mode)
  {
    case m_Normal:
      Result = 0;
      ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
      DstData = source.GetImageData();

      for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->height*ME_CAST_TO_IPLIMAGE(cvImg)->widthStep-1; i >= 0; --i)
      {
        if ((ImageData[i] >= 128) && (DstData[i] >= 128))
        {
          Result = (float)ImageData[i]/128*(float)DstData[i]/128;

          if (Result >= 1)
          {
            ImageData[i] = 255;
          } else {
            ImageData[i] = 0;
          }
        } else {
          ImageData[i] = 0;
        }
      }
      break;

    case m_Neighbourhood:
      TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                                     ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
      ImageData2 = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
      DstData = source.GetImageData();
      ImageData3 = (unsigned char*)TempImg->imageData;

      for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
        for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width-1; x >= 0; --x)
          for (int l = ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; l >= 0; --l)
      {
        if (((DstData[y*ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+
                      x*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] == 255) ||
            (ImageData2[y*ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+
                        x*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] == 255)) &&
            (NeighbourhoodCounter(x-2, y-2, n_5x5) > 3) &&
            (source.NeighbourhoodCounter(x-2, y-2, n_5x5) > 3))
        {
          ImageData3[y*ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+
                     x*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] = 255;
        }
      }
      ME_RELEASE_IPLIMAGE(cvImg);
      cvImg = TempImg;
      break;

    default:
      break;
  }
}


void MEImage::Addition(MEImage& source, AdditionType mode)
{
  if (source.GetWidth() != ME_CAST_TO_IPLIMAGE(cvImg)->width ||
      source.GetHeight() != ME_CAST_TO_IPLIMAGE(cvImg)->height ||
      source.GetLayers() != ME_CAST_TO_IPLIMAGE(cvImg)->nChannels)
  {
    printf("Image properties are different.\n");
    return;
  }
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* DstData = source.GetImageData();

  switch (mode)
  {
    case a_Average:
      for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->height*ME_CAST_TO_IPLIMAGE(cvImg)->widthStep-1; i >= 0; --i)
      {
        ImageData[i] = (ImageData[i]+DstData[i]) / 2;
      }
      break;

    case a_Union:
      for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->height*ME_CAST_TO_IPLIMAGE(cvImg)->widthStep-1; i >= 0; --i)
      {
        if (DstData[i] > ImageData[i])
        {
          ImageData[i] = DstData[i];
        }
      }
      break;

    default:
      break;
  }
}


void MEImage::EliminateSinglePixels()
{
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                                    ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* DstData = (unsigned char*)TempImg->imageData;
  int sum = 0;
  int xy = 0;
  int ywidth = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;

  for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
    for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width-1; x >= 0; --x)
  {
    xy = y*ywidth+x*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels;

    for (int l = ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; l >= 0; --l)
    {
      if ((ImageData[xy+l] > 0) && (x > 0) && (y > 0) && (x < ME_CAST_TO_IPLIMAGE(cvImg)->width-1) && (y < ME_CAST_TO_IPLIMAGE(cvImg)->height-1))
      {
        sum = (ImageData[xy-ywidth-ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] > 0)+
              (ImageData[xy-ywidth+l] > 0)+
              (ImageData[xy-ywidth+ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] > 0)+
              (ImageData[xy-ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] > 0)+
              (ImageData[xy+ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] > 0)+
              (ImageData[xy+ywidth-ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] > 0)+
              (ImageData[xy+ywidth+l] > 0)+
              (ImageData[xy+ywidth+ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l] > 0);

        if (sum > 3)
        {
          DstData[xy+l] = 255;
        } else {
          DstData[xy+l] = 0;
        }
      } else {
        DstData[xy+l] = 0;
      }
    }
  }
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}


float MEImage::DifferenceAreas(MEImage& reference, int difference) const
{
  if (reference.GetWidth() != GetWidth() ||
      reference.GetHeight() != GetHeight() ||
      reference.GetLayers() != GetLayers())
  {
    printf("Image dimensions or channels are different\n");
    return -1.0;
  }
  float PixelDiff = 0.0;
  int Pixels = 0;
  unsigned char* OrigImgData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* RefImgData = reference.GetImageData();
  int WidthStep = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;
  int RowStart = 0;

  for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
  {
    for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; x >= 0; --x)
    {
      if (abs(OrigImgData[RowStart+x]-RefImgData[RowStart+x]) > difference)
        Pixels++;
    }
    RowStart += WidthStep;
  }
  PixelDiff = (float)Pixels / (ME_CAST_TO_IPLIMAGE(cvImg)->height*ME_CAST_TO_IPLIMAGE(cvImg)->widthStep)*100;
  return PixelDiff;
}


int MEImage::AverageDifference(MEImage& reference) const
{
  if (reference.GetWidth() != GetWidth() ||
      reference.GetHeight() != GetHeight() ||
      reference.GetLayers() != GetLayers())
  {
    printf("Image dimensions or channels are different\n");
    return -1;
  }
  int Difference = 0;
  unsigned char* OrigImgData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* RefImgData = reference.GetImageData();
  int WidthStep = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;
  int RowStart = 0;

  for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
  {
    for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; x >= 0; --x)
    {
      Difference += abs(OrigImgData[RowStart+x]-RefImgData[RowStart+x]);
    }
    RowStart += WidthStep;
  }
  Difference = Difference / (ME_CAST_TO_IPLIMAGE(cvImg)->height*ME_CAST_TO_IPLIMAGE(cvImg)->widthStep);
  return Difference;
}


void MEImage::Minimum(MEImage& image)
{
  if (image.GetWidth() != ME_CAST_TO_IPLIMAGE(cvImg)->width ||
      image.GetHeight() != ME_CAST_TO_IPLIMAGE(cvImg)->height ||
      image.GetLayers() != ME_CAST_TO_IPLIMAGE(cvImg)->nChannels)
  {
    printf("Image properties are different\n");
    return;
  }
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* SecData = image.GetImageData();
  int WidthStep = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;
  int RowStart = 0;

  for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
  {
    for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; x >= 0; --x)
    {
      ImageData[RowStart+x] = ImageData[RowStart+x] > SecData[RowStart+x] ?
                              SecData[RowStart+x] : ImageData[RowStart+x];
    }
    RowStart += WidthStep;
  }
}


float MEImage::AverageBrightnessLevel() const
{
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  int WidthStep = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;
  int RowStart = 0;
  int BrightnessLevel = 0;

  for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
  {
    for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; x >= 0; --x)
    {
      BrightnessLevel += (int)ImageData[RowStart+x];
    }
    RowStart += WidthStep;
  }
  return BrightnessLevel / (GetWidth()*GetHeight()*GetLayers());
}


bool MEImage::Equal(const MEImage& reference) const
{
  return Equal(reference, 1);
}


bool MEImage::Equal(const MEImage& reference, int maxabsdiff) const
{
  bool Ret = true;

  if (reference.GetWidth() != ME_CAST_TO_IPLIMAGE(cvImg)->width ||
      reference.GetHeight() != ME_CAST_TO_IPLIMAGE(cvImg)->height ||
      reference.GetLayers() != ME_CAST_TO_IPLIMAGE(cvImg)->nChannels)
  {
    printf("Image properties are different\n");
    return false;
  }
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* RefData = reference.GetImageData();
  int WidthStep = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep;
  int RowStart = 0;

  for (int y = ME_CAST_TO_IPLIMAGE(cvImg)->height-1; y >= 0; --y)
  {
    for (int x = ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels-1; x >= 0; --x)
    {
      if (abs(ImageData[RowStart+x]-RefData[RowStart+x]) >= maxabsdiff)
      {
        Ret = false;
        return Ret;
      }
    }
    RowStart += WidthStep;
  }
  return Ret;
}


unsigned char MEImage::GrayscalePixel(int x, int y) const
{
  int NewX = x;
  int NewY = y;

  NewX = NewX < 0 ? 0 : NewX;
  NewX = NewX > ME_CAST_TO_IPLIMAGE(cvImg)->width-1 ? ME_CAST_TO_IPLIMAGE(cvImg)->width-1 : NewX;
  NewY = NewY < 0 ? 0 : NewY;
  NewY = NewY > ME_CAST_TO_IPLIMAGE(cvImg)->height-1 ? ME_CAST_TO_IPLIMAGE(cvImg)->height-1 : NewY;

  float Sum = 0;
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;

  for (int l = 0; l < ME_CAST_TO_IPLIMAGE(cvImg)->nChannels; l++)
  {
    Sum = Sum + (int)ImageData[NewY*ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+NewX*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+l];
  }
  Sum = Sum / ME_CAST_TO_IPLIMAGE(cvImg)->nChannels;
  return (unsigned char)(Sum);
}


int MEImage::NeighbourhoodCounter(int startx, int starty,
                                  NeighbourhoodType neighbourhood) const
{
  int IterX = 0;
  int IterY = 0;
  int Counter = 0;

  // Determine the iteration numbers
  switch (neighbourhood)
  {
    case n_2x2:
      IterX = 2;
      IterY = 2;
      break;

    case n_3x3:
      IterX = 3;
      IterY = 3;
      break;

    case n_3x2:
      IterX = 2;
      IterY = 3;
      break;

    case n_5x5:
      IterX = 5;
      IterY = 5;
      break;

    case n_7x7:
      IterX = 7;
      IterY = 7;
      break;

    default:
      IterX = 3;
      IterY = 3;
      break;
  }

  int NewStartX = startx ;
  int NewStartY = starty;

  NewStartX = startx < 0 ? 0 : startx;
  NewStartX = startx >= ME_CAST_TO_IPLIMAGE(cvImg)->width-IterX ? ME_CAST_TO_IPLIMAGE(cvImg)->width-IterX-1 : startx;
  NewStartY = starty < 0 ? 0 : starty;
  NewStartY = starty >= ME_CAST_TO_IPLIMAGE(cvImg)->height-IterY ? ME_CAST_TO_IPLIMAGE(cvImg)->height-IterY-1 : starty;

  int Value = 0;
  unsigned char* ImageData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;

  for (int x = NewStartX; x < NewStartX+IterX; x++)
    for (int y = NewStartY; y < NewStartY+IterY; y++)
  {
    Value = ((int)ImageData[y*ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+x*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels]+
             (int)ImageData[y*ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+x*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+1]+
             (int)ImageData[y*ME_CAST_TO_IPLIMAGE(cvImg)->width*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+x*ME_CAST_TO_IPLIMAGE(cvImg)->nChannels+2]) / 3;

    if (Value == 255)
    {
      Counter++;
    }
  }
  return Counter;
}


void MEImage::GradientVector(bool smooth, int x, int y, int mask_size, int& result_x, int& result_y)
{
  int Results[8];
  int DiagonalMaskSize = (int)((float)mask_size / sqrtf(2));

  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels > 1)
  {
    ConvertToGrayscale(g_OpenCV);
  }
  if (smooth)
  {
    SmoothAdvanced(s_Gaussian, mask_size*3-(mask_size*3-1) % 2);
  }

  Results[0] = (int)GrayscalePixel(x,y)-(int)GrayscalePixel(x,y-mask_size);
  Results[1] = (int)GrayscalePixel(x,y)-(int)GrayscalePixel(x+DiagonalMaskSize,y-DiagonalMaskSize);
  Results[2] = (int)GrayscalePixel(x,y)-(int)GrayscalePixel(x+mask_size,y);
  Results[3] = (int)GrayscalePixel(x,y)-(int)GrayscalePixel(x+DiagonalMaskSize,y+DiagonalMaskSize);
  Results[4] = (int)GrayscalePixel(x,y)-(int)GrayscalePixel(x,y+mask_size);
  Results[5] = (int)GrayscalePixel(x,y)-(int)GrayscalePixel(x-DiagonalMaskSize,y+DiagonalMaskSize);
  Results[6] = (int)GrayscalePixel(x,y)-(int)GrayscalePixel(x-mask_size,y);
  Results[7] = (int)GrayscalePixel(x,y)-(int)GrayscalePixel(x+DiagonalMaskSize,y-DiagonalMaskSize);

  result_x = (DiagonalMaskSize*Results[1]+mask_size*Results[2]+
             DiagonalMaskSize*Results[3]-DiagonalMaskSize*Results[5]-
             mask_size*Results[6]+DiagonalMaskSize*Results[7]) / 256;
  result_y = (-mask_size*Results[0]-DiagonalMaskSize*Results[1]+
             DiagonalMaskSize*Results[3]+mask_size*Results[4]+
             DiagonalMaskSize*Results[5]-DiagonalMaskSize*Results[7]) / 256;
}


void MEImage::GradientVisualize(int vector_x, int vector_y)
{
  if (vector_x <= 0)
  {
    printf("vectorx: wrong parameter (%d <= 0)\n", vector_x);
    return;
  }
  if (vector_y <= 0)
  {
    printf("vectory: wrong parameter (%d <= 0)\n", vector_y);
    return;
  }
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels > 1)
  {
    ConvertToGrayscale(g_OpenCV);
  }

  int masksize = (ME_CAST_TO_IPLIMAGE(cvImg)->width < ME_CAST_TO_IPLIMAGE(cvImg)->height) ?
                 ME_CAST_TO_IPLIMAGE(cvImg)->width / (vector_x+1) :
                 ME_CAST_TO_IPLIMAGE(cvImg)->height / (vector_y+1);

  SmoothAdvanced(s_Gaussian, masksize*2-1);
  for (int i = 1; i < vector_x; i++)
    for (int i1 = 1; i1 < vector_y; i1++)
  {
    int Resultx = 0, Resulty = 0;
    int x = (int)(((float)ME_CAST_TO_IPLIMAGE(cvImg)->width*i / (vector_x)));
    int y = (int)(((float)ME_CAST_TO_IPLIMAGE(cvImg)->height*i1 / (vector_y)));

    GradientVector(false, x, y, (int)(0.707*masksize), Resultx, Resulty);

    CvPoint Point1;
    CvPoint Point2;

    Point1.x = x-Resultx / 2;
    Point1.y = y-Resulty / 2;
    Point2.x = x+Resultx / 2;
    Point2.y = y+Resulty / 2;
    cvLine(ME_CAST_TO_IPLIMAGE(cvImg), Point1, Point2, CV_RGB(255, 255, 255), 1, 8);
  }
}


bool MEImage::_Copy(const MEImage& other_image)
{
  if (&other_image == this)
    return true;

  if (ME_CAST_TO_IPLIMAGE(cvImg))
  {
    ME_RELEASE_IPLIMAGE(cvImg);
  }
  cvImg = cvCloneImage((IplImage*)other_image.GetIplImage());
  return true;
}


void MEImage::_Init(int width, int height, int layers)
{
  if (width < 1)
  {
    printf("Given width for the new image is too small (%d <= 0)\n", width);
    return;
  }
  if (height < 1)
  {
    printf("Given height for the new image is (%d <= 0)\n", height);
    return;
  }
  if ((layers != 1) && (layers != 3))
  {
    printf("Only one or three (%d != 1 or 3) layer allowed\n", layers);
    return;
  }

  if (ME_CAST_TO_IPLIMAGE(cvImg))
  {
    ME_RELEASE_IPLIMAGE(cvImg);
  }
  cvImg = cvCreateImage(cvSize(width, height), 8, layers);
}


void MEImage::ComputeColorSpace(ColorSpaceConvertType mode)
{
  if (ME_CAST_TO_IPLIMAGE(cvImg)->nChannels != 3)
  {
    printf("Image has to have three color channels (%d != 3)\n", ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);
    return;
  }
  IplImage* TempImg = cvCreateImage(cvSize(ME_CAST_TO_IPLIMAGE(cvImg)->width, ME_CAST_TO_IPLIMAGE(cvImg)->height), 8,
                                    ME_CAST_TO_IPLIMAGE(cvImg)->nChannels);

  for (int i = 0; i < 3; i++)
    for (int i1 = 0; i1 < 3; i1++)
  {
    if (mode == csc_RGBtoYUV)
      TransformMatrix[i][i1] = RGBtoYUVMatrix[i][i1];
    if (mode == csc_RGBtoYIQ)
      TransformMatrix[i][i1] = RGBtoYIQMatrix[i][i1];
  }
  float x = 0.0;
  float y = 0.0;
  float z = 0.0;
  float xmin = 0.0;
  float xmax = 0.0;
  float ymin = 0.0;
  float ymax = 0.0;
  float zmin = 0.0;
  float zmax = 0.0;

  if (mode == csc_RGBtoYUV)
  {
    xmin = 0.0;
    xmax = 255.0;
    ymin = -111.18;
    ymax = 111.18;
    zmin = -156.825;
    zmax = 156.825;
  }
  if (mode == csc_RGBtoYIQ)
  {
    xmin = 0.0;
    xmax = 255.0;
    ymin = -151.98;
    ymax = 151.98;
    zmin = -133.365;
    zmax = 133.365;
  }
  unsigned char* SrcData = (unsigned char*)ME_CAST_TO_IPLIMAGE(cvImg)->imageData;
  unsigned char* DstData = (unsigned char*)TempImg->imageData;

  for (int i = ME_CAST_TO_IPLIMAGE(cvImg)->widthStep*ME_CAST_TO_IPLIMAGE(cvImg)->height-1; i >= 0; i-=3)
  {
    x = (float)SrcData[i]*TransformMatrix[0][0]+
        (float)SrcData[i+1]*TransformMatrix[0][1]+
        (float)SrcData[i+2]*TransformMatrix[0][2];
    y = (float)SrcData[i]*TransformMatrix[1][0]+
        (float)SrcData[i+1]*TransformMatrix[1][1]+
        (float)SrcData[i+2]*TransformMatrix[1][2];
    z = (float)SrcData[i]*TransformMatrix[2][0]+
        (float)SrcData[i+1]*TransformMatrix[2][1]+
        (float)SrcData[i+2]*TransformMatrix[2][2];

    x = xmax-xmin != 0.0 ? 255.0 : (x-xmin) / (xmax-xmin)*255.0;
    y = ymax-ymin != 0.0 ? 255.0 : (y-xmin) / (ymax-ymin)*255.0;
    z = zmax-zmin != 0.0 ? 255.0 : (z-xmin) / (zmax-zmin)*255.0;

    DstData[i] = (unsigned char)MEBound(0, (int)x, 255);
    DstData[i+1] = (unsigned char)MEBound(0, (int)y, 255);
    DstData[i+2] = (unsigned char)MEBound(0, (int)z, 255);
  }
  ME_RELEASE_IPLIMAGE(cvImg);
  cvImg = TempImg;
}
