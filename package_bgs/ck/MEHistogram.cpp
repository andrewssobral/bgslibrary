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
 *  Some histogram stretch codes are based on how Gimp does it, the same
 *  GPL 2 license applies for the following authors:
 *
 *  The GIMP -- an image manipulation program
 *  Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 */

#include "MEHistogram.hpp"

#include <opencv2/opencv.hpp>

#include "MEDefs.hpp"
#include "MEImage.hpp"

MEHistogram::MEHistogram()
{
  Clear();
}


MEHistogram::~MEHistogram()
{
}


void MEHistogram::Clear()
{
  memset(&HistogramData, 0, 256*sizeof(int));
}


bool MEHistogram::operator==(MEHistogram& histogram) const
{
  bool ret = true;

  for (int i = 255; i >= 0; --i)
  {
    if (HistogramData[i] != histogram.HistogramData[i])
    {
      ret = false;
      break;
    }
  }
  return ret;
}


void MEHistogram::Calculate(MEImage& image, int channel, HistogramType mode)
{
  int Channel = (channel < 1) ? 1 : ((channel > image.GetLayers()) ? image.GetLayers() : channel);

  if (mode == h_Overwrite)
  {
    Clear();
  }

  unsigned char *ImageData = image.GetImageData();
  int rowStart = 0;

  for (int i = image.GetHeight()-1; i >= 0; i--)
  {
    for (int i1 = (image.GetWidth()-1)*image.GetLayers()+Channel-1; i1 >= Channel-1; i1 -= image.GetLayers())
    {
      HistogramData[ImageData[rowStart+i1]]++;
    }
    rowStart += image.GetRowWidth();
  }
}


void MEHistogram::Calculate(MEImage& image, HistogramType mode)
{
  if (mode == h_Overwrite)
  {
    Clear();
  }

  unsigned char *ImageData = image.GetImageData();
  int RowStart = 0;
  int RowWidth = image.GetRowWidth();

  for (int i = image.GetHeight()-1; i >= 0; i--)
  {
    for (int i1 = image.GetWidth()*image.GetLayers()-1; i1 >= 0; i1--)
    {
      HistogramData[ImageData[RowStart+i1]]++;
    }
    RowStart += RowWidth;
  }
}


void MEHistogram::Calculate(MEImage& image, int channel, int x0, int y0, int x1, int y1)
{
  int Channel = (channel < 1) ? 1 : ((channel > image.GetLayers()) ? image.GetLayers() : channel);
  unsigned char *ImageData = image.GetImageData();
  int RowStart = 0;
  int RowWidth = image.GetRowWidth();
  int X0 = x0 > x1 ? x1 : x0;
  int Y0 = y0 > y1 ? y1 : y0;
  int X1 = x0 < x1 ? x1 : x0;
  int Y1 = y0 < y1 ? y1 : y0;

  Clear();

  // Compute the correct region coordinates and check them
  X0 = X0 < 0 ? 0 : X0;
  Y0 = Y0 < 0 ? 0 : Y0;
  X1 = X1 > image.GetWidth()-1 ? image.GetWidth()-1 : X1;
  Y1 = Y1 > image.GetHeight()-1 ? image.GetHeight()-1 : Y1;
  RowStart = Y0*image.GetRowWidth();

  for (int i = Y1; i >= Y0; --i)
  {
    for (int i1 = X1*image.GetLayers()+Channel-1; i1 >= X0*image.GetLayers()+Channel-1;
         i1 -= image.GetLayers())
    {
      HistogramData[ImageData[RowStart+i1]]++;
    }
    RowStart += RowWidth;
  }
}


int MEHistogram::GetPeakIndex() const
{
  int PeakIndex = 0;
  int PeakValue = 0;

  for (int i = 0; i < 256; i++)
  {
    if (PeakValue < HistogramData[i])
    {
      PeakValue = HistogramData[i];
      PeakIndex = i;
    }
  }
  return PeakIndex;
}


int MEHistogram::GetLowestLimitIndex(int threshold) const
{
  int MinIndex = 0;

  for (int i = 0; i < 256; i++)
  {
    if (threshold <= HistogramData[i])
    {
      MinIndex = i;
      break;
    }
  }
  return MinIndex;
}


int MEHistogram::GetHighestLimitIndex(int threshold) const
{
  int MaxIndex = 255;

  for (int i = 255; i >= 0; i--)
  {
    if (threshold <= HistogramData[i])
    {
      MaxIndex = i;
      break;
    }
  }
  return MaxIndex;
}


int MEHistogram::GetPowerAmount(int minindex, int maxindex) const
{
  int ValueAmount = 0;
  int MinIndex = (minindex > 255) ? 255 : ((minindex < 0) ? 0 : minindex);
  int MaxIndex = (maxindex > 255) ? 255 : ((maxindex < 0) ? 0 : maxindex);

  if (MinIndex > MaxIndex)
  {
    int TempInt = MinIndex;
    MinIndex = MaxIndex;
    MaxIndex = TempInt;
  }

  for (int i = MinIndex; i <= MaxIndex; i++)
  {
    ValueAmount += HistogramData[i];
  }
  return ValueAmount;
}


int MEHistogram::GetCentroidIndex() const
{
  int ValueAmount = GetPowerAmount(0, 255);
  int WeightedValueAmount = 1;
  int CentroidIndex = 0;

  // Calculate the normal and weighted amount of histogram values
  for (int i = 0; i < 256; i++)
  {
    WeightedValueAmount += i*HistogramData[i];
  }
  // Calculate the centroid point of the histogram
  CentroidIndex = WeightedValueAmount / ValueAmount;
  return CentroidIndex;
}


bool MEHistogram::Stretch(StretchType mode)
{
  int MinIndex = -1;
  int MaxIndex = -1;
  int Percent = -1;
  double Percentage = 0.0;
  double NextPercentage = 0.0;
  double Count = 0.0;
  double NewCount = 0.0;
  bool Ret = true;

  switch (mode)
  {
    case s_OwnMode:
      Percent = 20;
      MinIndex = GetLowestLimitIndex(Percent);
      MaxIndex = GetHighestLimitIndex(Percent);

      while ((abs(MaxIndex-MinIndex) < 52) && (Percent > 1))
      {
        Percent = Percent / 2;
        MinIndex = GetLowestLimitIndex(Percent);
        MaxIndex = GetHighestLimitIndex(Percent);

        // The calculation gives wrong answer back
        if (MinIndex == 0 && MaxIndex == 255)
        {
          MinIndex = 128;
          MaxIndex = 128;
          Ret = false;
        }
      }
      break;

    case s_GimpMode:
      Count = GetPowerAmount(0, 255);
      NewCount = 0;

      for (int i = 0; i < 255; i++)
      {
        double Value = 0.0;
        double NextValue = 0.0;

        Value = HistogramData[i];
        NextValue = HistogramData[i+1];
        NewCount += Value;
        Percentage = NewCount / Count;
        NextPercentage = (NewCount+NextValue) / Count;

        if (fabs(Percentage-0.006) < fabs(NextPercentage-0.006))
        {
          MinIndex = i+1;
          break;
        }
      }
      NewCount = 0.0;
      for (int i = 255; i > 0; i--)
      {
        double Value = 0.0;
        double NextValue = 0.0;

        Value = HistogramData[i];
        NextValue = HistogramData[i-1];
        NewCount += Value;
        Percentage = NewCount / Count;
        NextPercentage = (NewCount+NextValue) / Count;

        if (fabs(Percentage-0.006) < fabs(NextPercentage-0.006))
        {
          MaxIndex = i-1;
          break;
        }
      }
      break;

    default:
      break;
  }

  if (MaxIndex <= MinIndex)
  {
    MinIndex = 0;
    MaxIndex = 255;
    Ret = false;
  }
  if (MaxIndex-MinIndex <= 10 ||
      (MaxIndex-MinIndex <= 20 && (float)GetPowerAmount(MinIndex, MaxIndex) / GetPowerAmount(0, 255) < 0.20))
  {
    MinIndex = 0;
    MaxIndex = 255;
    Ret = false;
  }
  if (Ret)
  {
    unsigned char TransformedHistogram[256];

    for (int i = 0; i < 256; ++i)
    {
      TransformedHistogram[i] = (unsigned char)MEBound(0, 255*(i-MinIndex) / (MaxIndex-MinIndex), 255);
    }
    for (int i = 0; i < 256; ++i)
    {
      HistogramData[i] = TransformedHistogram[i];
    }
  }
  return Ret;
}


MEHistogramTransform::MEHistogramTransform() : ChannelMode(p_SeparateChannels),
StretchMode(MEHistogram::s_GimpMode), DiscreteStretchingDone(false)
{
}


MEHistogramTransform::~MEHistogramTransform()
{
}


void MEHistogramTransform::HistogramStretch(MEImage& image)
{
  SetStretchProcessingMode(p_SeparateChannels, MEHistogram::s_GimpMode);
  HistogramStretch(image, t_Continuous);
}


void MEHistogramTransform::HistogramStretch(MEImage& image, TransformType time_mode)
{
  if (time_mode == t_Continuous)
  {
    DiscreteStretchingDone = false;
  }

  if (ChannelMode == p_Average || image.GetLayers() == 1)
  {
    if (time_mode == t_Continuous || (time_mode == t_Discrete && !DiscreteStretchingDone))
    {
      RedChannel.Calculate(image, 1, MEHistogram::h_Overwrite);

      for (int l = 1; l < image.GetLayers(); l++)
      {
        RedChannel.Calculate(image, l+1, MEHistogram::h_Add);
      }
      RedChannel.Stretch(StretchMode);
      if (time_mode == t_Discrete && !DiscreteStretchingDone)
      {
        DiscreteStretchingDone = true;
      }
    }
    unsigned char *ImageData = image.GetImageData();
    int RowStart = 0;
    int RowWidth = image.GetRowWidth();

    for (int i = image.GetHeight()-1; i >= 0; i--)
    {
      for (int i1 = image.GetWidth()*image.GetLayers()-1; i1 >= 0; i1--)
      {
        ImageData[RowStart+i1] = RedChannel.HistogramData[ImageData[RowStart+i1]];
      }
      RowStart += RowWidth;
    }
  } else
  if (ChannelMode == p_SeparateChannels)
  {
    if (time_mode == t_Continuous || (time_mode == t_Discrete && !DiscreteStretchingDone))
    {
      RedChannel.Calculate(image, 1, MEHistogram::h_Overwrite);
      GreenChannel.Calculate(image, 2, MEHistogram::h_Overwrite);
      BlueChannel.Calculate(image, 3, MEHistogram::h_Overwrite);
      RedChannel.Stretch(StretchMode);
      GreenChannel.Stretch(StretchMode);
      BlueChannel.Stretch(StretchMode);
      if (time_mode == t_Discrete && !DiscreteStretchingDone)
      {
        DiscreteStretchingDone = true;
      }
    }
    unsigned char *ImageData = image.GetImageData();
    int RowStart = 0;
    int RowWidth = image.GetRowWidth();

    for (int i = image.GetHeight()-1; i >= 0; i--)
    {
      for (int i1 = image.GetWidth()*image.GetLayers()-3; i1 >= 0; i1 -= 3)
      {
        ImageData[RowStart+i1] = RedChannel.HistogramData[ImageData[RowStart+i1]];
        ImageData[RowStart+i1+1] = GreenChannel.HistogramData[ImageData[RowStart+i1+1]];
        ImageData[RowStart+i1+2] = BlueChannel.HistogramData[ImageData[RowStart+i1+2]];
      }
      RowStart += RowWidth;
    }
  }
}


void MEHistogramTransform::HistogramEqualize(MEImage& image)
{
  DiscreteStretchingDone = false;
  IplImage* cvDest8bitImg = NULL;
  IplImage* cvDestImg = NULL;

  switch (image.GetLayers())
  {
    case 1:
      // Grayscale image
      cvDest8bitImg = cvCreateImage(cvSize(image.GetWidth(), image.GetHeight()), 8, 1);
      cvEqualizeHist((IplImage*)image.GetIplImage(), cvDest8bitImg);
      image.SetIplImage((void*)cvDest8bitImg);
      cvReleaseImage(&cvDest8bitImg);
      break;

    case 3:
      // RGB image
      cvDestImg = cvCreateImage(cvSize(image.GetWidth(), image.GetHeight()), 8, 3);
      IplImage *cvR, *cvG, *cvB;

      cvR = cvCreateImage(cvSize(image.GetWidth(), image.GetHeight()), 8, 1);
      cvG = cvCreateImage(cvSize(image.GetWidth(), image.GetHeight()), 8, 1);
      cvB = cvCreateImage(cvSize(image.GetWidth(), image.GetHeight()), 8, 1);

      cvSplit((IplImage*)image.GetIplImage(), cvR, cvG, cvB, NULL);
      cvEqualizeHist(cvR, cvR);
      cvEqualizeHist(cvG, cvG);
      cvEqualizeHist(cvB, cvB);
      cvMerge(cvR, cvG, cvB, NULL, cvDestImg);

      image.SetIplImage((void*)cvDestImg);
      cvReleaseImage(&cvR);
      cvReleaseImage(&cvG);
      cvReleaseImage(&cvB);
      cvReleaseImage(&cvDestImg);
      break;

    default:
      break;
  }
}


void MEHistogramTransform::SetStretchProcessingMode(ProcessingType new_channel_mode,
                                                    MEHistogram::StretchType new_stretch_mode)
{
  DiscreteStretchingDone = false;

  switch(new_channel_mode)
  {
    case p_SeparateChannels:
      ChannelMode = new_channel_mode;
      break;

    case p_Average:
      ChannelMode = new_channel_mode;
      break;

    default:
      break;
  }

  switch(new_stretch_mode)
  {
    case MEHistogram::s_OwnMode:
      StretchMode = new_stretch_mode;
      break;

    case MEHistogram::s_GimpMode:
      StretchMode = new_stretch_mode;
      break;

    default:
      break;
  }
}
