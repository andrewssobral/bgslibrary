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
* T2FGMM.h
*
* Purpose: Implementation of the T2 Fuzzy Gaussian Mixture Models (T2GMMs) 
* "Modeling of Dynamic Backgrounds by Type-2 Fuzzy Gaussians Mixture Models"
* Author: Fida El Baf, Thierry Bouwmans, September 2008
*
* This code is based on code by Z. Zivkovic's written for his enhanced GMM
* background subtraction algorithm: 
*
* Zivkovic's code can be obtained at: www.zoranz.net
******************************************************************************/

#ifndef T2F_GMM_
#define T2F_GMM_

#include "../dp/Bgs.h"
#include "../dp/GrimsonGMM.h"

namespace Algorithms
{
  namespace BackgroundSubtraction
  {
    const int TYPE_T2FGMM_UM = 0;
    const int TYPE_T2FGMM_UV = 1;

    // --- User adjustable parameters used by the T2F GMM BGS algorithm ---
    class T2FGMMParams : public BgsParams
    {
    public:
      float &LowThreshold() { return m_low_threshold; }
      float &HighThreshold() { return m_high_threshold; }

      float &Alpha() { return m_alpha; }
      int &MaxModes() { return m_max_modes; }
      int &Type() { return m_type; }
      float &KM() { return m_km; }
      float &KV() { return m_kv; }

    private:
      // Threshold on the squared dist. to decide when a sample is close to an existing 
      // components. If it is not close to any a new component will be generated. 
      // Smaller threshold values lead to more generated components and higher threshold values 
      // lead to a small number of components but they can grow too large.
      //
      // It is usual easiest to think of these thresholds as being the number of variances away
      // from the mean of a pixel before it is considered to be from the foreground.
      float m_low_threshold;
      float m_high_threshold;

      // alpha - speed of update - if the time interval you want to average over is T
      // set alpha=1/T. 
      float m_alpha;

      // Maximum number of modes (Gaussian components) that will be used per pixel
      int m_max_modes;

      // T2FGMM_UM / T2FGMM_UV
      int m_type;

      // Factor control for the T2FGMM-UM
      float m_km;

      // Factor control for the T2FGMM-UV
      float m_kv;
    };

    // --- T2FGMM BGS algorithm ---
    class T2FGMM : public Bgs
    {
    public:
      T2FGMM();
      ~T2FGMM();

      void Initalize(const BgsParams& param);

      void InitModel(const BgsRgbImage& data);
      void Subtract(int frame_num, const BgsRgbImage& data, BgsBwImage& low_threshold_mask, BgsBwImage& high_threshold_mask);	
      void Update(int frame_num, const BgsRgbImage& data, const BgsBwImage& update_mask);

      BgsRgbImage* Background();

    private:	
      void SubtractPixel(long posPixel, const BgsRgbPixel& pixel, unsigned char& numModes, unsigned char& lowThreshold, unsigned char& highThreshold);

      // User adjustable parameters
      T2FGMMParams m_params;

      // Threshold when the component becomes significant enough to be included into
      // the background model. It is the TB = 1-cf from the paper. So I use cf=0.1 => TB=0.9
      // For alpha=0.001 it means that the mode should exist for approximately 105 frames before
      // it is considered foreground
      float m_bg_threshold; //1-cf from the paper

      // Initial variance for the newly generated components. 
      // It will will influence the speed of adaptation. A good guess should be made. 
      // A simple way is to estimate the typical standard deviation from the images.
      float m_variance;

      // Dynamic array for the mixture of Gaussians
      GMM* m_modes;

      // Number of Gaussian components per pixel
      BgsBwImage m_modes_per_pixel;

      // Current background model
      BgsRgbImage m_background;

      // Factor control for the T2FGMM-UM
      float km;

      // Factor control for the T2FGMM-UV
      float kv;
    };
  }
}

#endif
