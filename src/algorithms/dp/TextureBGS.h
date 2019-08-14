#pragma once

#include <math.h>
#include "Image.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
      const int REGION_R = 5;			// Note: the code currently assumes this value is <= 7
      const int TEXTURE_POINTS = 6;	// Note: the code currently assumes this value is 6
      const int TEXTURE_R = 2;		// Note: the code currently assumes this value is 2
      const int NUM_BINS = 64;		// 2^TEXTURE_POINTS
      const int HYSTERSIS = 3;
      const double ALPHA = 0.05f;
      const double THRESHOLD = 0.5*(REGION_R + REGION_R + 1)*(REGION_R + REGION_R + 1)*NUM_CHANNELS;
      const int NUM_MODES = 1;		// The paper describes how multiple modes can be maintained,
      // but this implementation does not fully support more than one

      struct TextureHistogram
      {
        unsigned char r[NUM_BINS];	// histogram for red channel
        unsigned char g[NUM_BINS];	// histogram for green channel
        unsigned char b[NUM_BINS];	// histogram for blue channel
      };

      struct TextureArray
      {
        TextureHistogram mode[NUM_MODES];
      };

      class TextureBGS
      {
      public:
        TextureBGS();
        ~TextureBGS();

        void LBP(RgbImage& image, RgbImage& texture);
        void Histogram(RgbImage& texture, TextureHistogram* curTextureHist);
        int ProximityMeasure(TextureHistogram& bgTexture, TextureHistogram& curTextureHist);
        void BgsCompare(TextureArray* bgModel, TextureHistogram* curTextureHist,
          unsigned char* modeArray, float threshold, BwImage& fgMask);
        void UpdateModel(BwImage& fgMask, TextureArray* bgModel,
          TextureHistogram* curTextureHist, unsigned char* modeArray);
      };
    }
  }
}

#endif
