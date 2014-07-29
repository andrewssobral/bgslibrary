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
/****************************************************************************
*
* MeanBGS.hpp
*
* Purpose: Implementation of a simple temporal mean background 
*		  		 subtraction algorithm.
*
* Author: Donovan Parks, September 2007
*

Example:
Algorithms::BackgroundSubtraction::MeanParams params;
params.SetFrameSize(width, height);
params.LowThreshold() = 3*30*30;
params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
params.Alpha() = 1e-6f;
params.LearningFrames() = 30;

Algorithms::BackgroundSubtraction::MeanBGS bgs;
bgs.Initalize(params);
******************************************************************************/

#include "Bgs.h"

namespace Algorithms
{
  namespace BackgroundSubtraction
  {

    // --- Parameters used by the Mean BGS algorithm ---
    class MeanParams : public BgsParams
    {
    public:
      unsigned int &LowThreshold() { return m_low_threshold; }
      unsigned int &HighThreshold() { return m_high_threshold; }

      float &Alpha() { return m_alpha; }
      int &LearningFrames() { return m_learning_frames; }

    private:
      // A pixel is considered to be from the background if the squared distance between 
      // it and the background model is less than the threshold.
      unsigned int m_low_threshold;
      unsigned int m_high_threshold;

      float m_alpha;
      int m_learning_frames;
    };


    // --- Mean BGS algorithm ---
    class MeanBGS : public Bgs
    {
    public:
      virtual ~MeanBGS() {}

      void Initalize(const BgsParams& param);

      void InitModel(const RgbImage& data);
      void Subtract(int frame_num, const RgbImage& data,  
        BwImage& low_threshold_mask, BwImage& high_threshold_mask);	
      void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask);

      RgbImage* Background() { return &m_background; }

    private:	
      void SubtractPixel(int r, int c, const RgbPixel& pixel, 
        unsigned char& lowThreshold, unsigned char& highThreshold);

      MeanParams m_params;

      RgbImageFloat m_mean;
      RgbImage m_background;
    };

  }
}





