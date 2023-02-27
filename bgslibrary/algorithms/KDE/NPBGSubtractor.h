#pragma once

#include "NPBGmodel.h"
#include "KernelTable.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace kde
    {
      const int FALSE = 0;
      const int TRUE = 1;

      // kernal look up table settings
      const int KERNELHALFWIDTH = 255;
      const float SEGMAMAX = 36.5;
      const float SEGMAMIN = 0.5;
      const int SEGMABINS = 80;
      const float DEFAULTSEGMA = 1.0;

      typedef struct
      {
        unsigned char *Hist;
        unsigned char *MedianBins;
        unsigned char *MedianFreq;
        unsigned char *AccSum;
        unsigned char histbins;
        unsigned char histsum;
        unsigned int  imagesize;
      } DynamicMedianHistogram;

      typedef struct
      {
        unsigned int cnt;
        unsigned int *List;
      } ImageIndex;

      class NPBGSubtractor
      {
      private:
        unsigned int rows;
        unsigned int cols;
        unsigned int color_channels;
        unsigned int imagesize;
        // flags
        unsigned char UpdateBGFlag;
        unsigned char SdEstimateFlag;
        unsigned char UseColorRatiosFlag;
        unsigned char AdaptBGFlag;
        unsigned char SubsetFlag;
        //
        int UpdateSDRate;
        double Threshold;
        double AlphaValue;
        unsigned int TimeIndex;
        ImageIndex  *imageindex;
        unsigned char *tempFrame;
        KernelLUTable *KernelTable;
        NPBGmodel *BGModel;
        DynamicMedianHistogram AbsDiffHist;
        double *Pimage1;
        double *Pimage2;
        //
        void NPBGSubtraction_Subset_Kernel(unsigned char * image, unsigned char * FGImage, unsigned char * FilteredFGImage);
        void SequenceBGUpdate_Pairs(unsigned char * image, unsigned char * Mask);

      public:
        NPBGSubtractor();
        virtual ~NPBGSubtractor();
        //~NPBGSubtractor();

        int Intialize(unsigned int rows,
          unsigned int cols,
          unsigned int color_channels,
          unsigned int SequenceLength,
          unsigned int TimeWindowSize,
          unsigned char SDEstimationFlag,
          unsigned char UseColorRatiosFlag);

        void AddFrame(unsigned char * ImageBuffer);

        void Estimation();

        void NBBGSubtraction(unsigned char *Frame,
          unsigned char *FGImage,
          unsigned char *FilteredFGImage,
          unsigned char **DisplayBuffers);

        void Update(unsigned char *);

        void SetThresholds(double th, double alpha)
        {
          Threshold = th;
          AlphaValue = alpha;
        };

        void SetUpdateFlag(unsigned int bgflag) {
          UpdateBGFlag = bgflag;
        };
      };
    }
  }
}
