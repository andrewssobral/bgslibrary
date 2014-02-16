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
#include "TextureBGS.h"

TextureBGS::TextureBGS(){}
TextureBGS::~TextureBGS(){}

void TextureBGS::LBP(RgbImage& image, RgbImage& texture)
{
  for(int y = TEXTURE_R; y < image.Ptr()->height-TEXTURE_R; ++y)
  {
    for(int x = TEXTURE_R; x < image.Ptr()->width-TEXTURE_R; ++x)
    {		
      for(int ch = 0; ch < NUM_CHANNELS; ++ch)
      {
        unsigned char textureCode = 0;
        int centerValue = (int)image(y, x, ch);

        // this only works for a texture radius of 2
        if(centerValue - (int)image(y-2, x, ch) + HYSTERSIS >= 0)
          textureCode += 1;

        if(centerValue - (int)image(y-1, x-2, ch) + HYSTERSIS >= 0)
          textureCode += 2;

        if(centerValue - (int)image(y-1, x+2, ch) + HYSTERSIS >= 0)
          textureCode += 4;

        if(centerValue - (int)image(y+1, x-2, ch) + HYSTERSIS >= 0)
          textureCode += 8;

        if(centerValue - (int)image(y+1, x+2, ch) + HYSTERSIS >= 0)
          textureCode += 16;

        if(centerValue - (int)image(y+2, x, ch) + HYSTERSIS >= 0)
          textureCode += 32;

        texture(y,x,ch) = textureCode;
      }
    }
  }
}

void TextureBGS::Histogram(RgbImage& texture, TextureHistogram* curTextureHist)
{
  // calculate histogram within a 2*REGION_R square
  for(int y = REGION_R+TEXTURE_R; y < texture.Ptr()->height-REGION_R-TEXTURE_R; ++y)
  {
    for(int x = REGION_R+TEXTURE_R; x < texture.Ptr()->width-REGION_R-TEXTURE_R; ++x)
    {	
      int index = x+y*(texture.Ptr()->width);

      // clear histogram
      for(int i = 0; i < NUM_BINS; ++i)
      {
        curTextureHist[index].r[i] = 0;
        curTextureHist[index].g[i] = 0;
        curTextureHist[index].b[i] = 0;
      }

      // calculate histogram
      for(int j = -REGION_R; j <= REGION_R; ++j)
      {
        for(int i = -REGION_R; i <= REGION_R; ++i)
        {
          curTextureHist[index].r[texture(y+j,x+i,2)]++;
          curTextureHist[index].g[texture(y+j,x+i,1)]++;
          curTextureHist[index].b[texture(y+j,x+i,0)]++;
        }
      }
    }
  }
}

int TextureBGS::ProximityMeasure(TextureHistogram& bgTexture, TextureHistogram& curTextureHist)
{
  int proximity = 0;	
  for(int i = 0; i < NUM_BINS; ++i)
  {
    proximity += std::min(bgTexture.r[i], curTextureHist.r[i]);
    proximity += std::min(bgTexture.g[i], curTextureHist.g[i]);
    proximity += std::min(bgTexture.b[i], curTextureHist.b[i]);
  }

  return proximity;	
}

void TextureBGS::BgsCompare(TextureArray* bgModel, TextureHistogram* curTextureHist, 
                unsigned char* modeArray, float threshold, BwImage& fgMask)
{
  cvZero(fgMask.Ptr());

  for(int y = REGION_R+TEXTURE_R; y < fgMask.Ptr()->height-REGION_R-TEXTURE_R; ++y)
  {
    for(int x = REGION_R+TEXTURE_R; x < fgMask.Ptr()->width-REGION_R-TEXTURE_R; ++x)
    {	
      int index = x+y*(fgMask.Ptr()->width);

      // find closest matching texture in background model
      int maxProximity = -1;

      for(int m = 0; m < NUM_MODES; ++m)
      {
        int proximity = ProximityMeasure(bgModel[index].mode[m], curTextureHist[index]);

        if(proximity > maxProximity)
        {
          maxProximity = proximity;
          modeArray[index] = m;
        }
      }

      if(maxProximity < threshold)
        fgMask(y,x) = 255;
    }
  }
}

void TextureBGS::UpdateModel(BwImage& fgMask, TextureArray* bgModel, 
                 TextureHistogram* curTextureHist, unsigned char* modeArray)
{
  for(int y = REGION_R+TEXTURE_R; y < fgMask.Ptr()->height-REGION_R-TEXTURE_R; ++y)
  {
    for(int x = REGION_R+TEXTURE_R; x < fgMask.Ptr()->width-REGION_R-TEXTURE_R; ++x)
    {		
      int index = x+y*(fgMask.Ptr()->width);

      if(fgMask(x,y) == 0)
      {
        for(int i = 0; i < NUM_BINS; ++i)
        {
          bgModel[index].mode[modeArray[index]].r[i]
          = (unsigned char)(ALPHA*curTextureHist[index].r[i]
          + (1-ALPHA)*bgModel[index].mode[modeArray[index]].r[i] + 0.5);
        }
      }				
    }
  }	
}
