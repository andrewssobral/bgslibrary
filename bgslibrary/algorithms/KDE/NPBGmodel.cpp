#include "memory.h"

#include "NPBGmodel.h"

//#ifdef _DEBUG
//#undef THIS_FILE
//static char THIS_FILE[] = __FILE__;
////#define new DEBUG_NEW
//#endif

using namespace bgslibrary::algorithms::kde;

NPBGmodel::NPBGmodel() {}

NPBGmodel::~NPBGmodel() {
  delete Sequence;
  delete PixelQTop;
  delete TemporalBuffer;
  delete TemporalMask;
  delete AccMask;
  //delete SDbinsImage;
}

NPBGmodel::NPBGmodel(unsigned int Rows,
  unsigned int Cols,
  unsigned int ColorChannels,
  unsigned int Length,
  unsigned int pTimeWindowSize,
  unsigned int bg_suppression_time)
{
  imagesize = Rows*Cols*ColorChannels;

  rows = Rows;
  cols = Cols;
  color_channels = ColorChannels;

  SampleSize = Length;

  TimeWindowSize = pTimeWindowSize;

  Sequence = new unsigned char[imagesize*Length];
  Top = 0;
  memset(Sequence, 0, imagesize*Length);

  PixelQTop = new unsigned char[rows*cols];

  // temporalBuffer
  TemporalBufferLength = (TimeWindowSize / Length > 2 ? TimeWindowSize / Length : 2);
  TemporalBuffer = new unsigned char[imagesize*TemporalBufferLength];
  TemporalMask = new unsigned char[rows*cols*TemporalBufferLength];

  TemporalBufferTop = 0;

  AccMask = new unsigned int[rows*cols];

  ResetMaskTh = bg_suppression_time;
}

void NPBGmodel::AddFrame(unsigned char *ImageBuffer)
{
  memcpy(Sequence + Top*imagesize, ImageBuffer, imagesize);
  Top = (Top + 1) % SampleSize;

  memset(PixelQTop, (unsigned char)Top, rows*cols);

  memcpy(TemporalBuffer, ImageBuffer, imagesize);
}
