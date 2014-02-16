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
#include <math.h>
#include "Image.h"

const int REGION_R = 5;			// Note: the code currently assumes this value is <= 7
const int TEXTURE_POINTS = 6;	// Note: the code currently assumes this value is 6
const int TEXTURE_R = 2;		// Note: the code currently assumes this value is 2
const int NUM_BINS = 64;		// 2^TEXTURE_POINTS
const int HYSTERSIS = 3;
const double ALPHA = 0.05f;
const double THRESHOLD = 0.5*(REGION_R+REGION_R+1)*(REGION_R+REGION_R+1)*NUM_CHANNELS;
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
