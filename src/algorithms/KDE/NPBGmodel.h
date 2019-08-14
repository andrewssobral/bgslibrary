#pragma once

#include <iostream>

namespace bgslibrary
{
  namespace algorithms
  {
    namespace kde
    {
      class NPBGmodel
      {
      private:
        unsigned char *Sequence;
        unsigned int SampleSize;
        unsigned int TimeWindowSize;

        unsigned int rows, cols, color_channels;
        unsigned int imagesize;

        unsigned int Top;
        unsigned char *PixelQTop;

        //unsigned int *PixelUpdateCounter;

        unsigned char *SDbinsImage;

        unsigned char *TemporalBuffer;
        unsigned char TemporalBufferLength;
        unsigned char TemporalBufferTop;
        unsigned char *TemporalBufferMask;

        unsigned char *TemporalMask;
        unsigned char TemporalMaskLength;
        unsigned char TemporalMaskTop;

        unsigned int *AccMask;
        unsigned int ResetMaskTh;	// Max continous duration a pixel can be detected before
        // it is forced to be updated...

        double *weights;

      public:
        NPBGmodel();
        //~NPBGmodel();
        virtual ~NPBGmodel();

        NPBGmodel(unsigned int Rows,
          unsigned int Cols,
          unsigned int ColorChannels,
          unsigned int Length,
          unsigned int pTimeWindowSize,
          unsigned int bg_suppression_time);

        void AddFrame(unsigned char *ImageBuffer);

        friend class NPBGSubtractor;
      };
    }
  }
}
