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
/*  Scene 1.0.1 -- Background subtraction and object tracking for complex environments  
BGModelMog.cpp

Copyright (C) 2011 Laurence Bender <lbender@untref.edu.ar>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "BGModelMog.h"

namespace lb_library
{
  namespace MixtureOfGaussians
  {
    BGModelMog::BGModelMog(int width, int height) : BGModel(width, height)
    {
      m_alpha = LEARNINGRATEMOG;
      m_threshold = THRESHOLDMOG*THRESHOLDMOG;
      m_noise = INITIALVARMOG;

      m_T = BGTHRESHOLDMOG;

      m_pMOG = new MOGDATA[NUMBERGAUSSIANS*m_width*m_height];
      m_pK = new int[m_width*m_height];

      MOGDATA *pMOG = m_pMOG;
      int *pK = m_pK;

      for(int i = 0; i < (m_width * m_height); i++)
      {
        for(int k = 0; k < NUMBERGAUSSIANS; k++)
        {
          pMOG->mu.Red = 0.0;
          pMOG->mu.Green = 0.0;
          pMOG->mu.Blue = 0.0;

          pMOG->var.Red = 0.0;
          pMOG->var.Green = 0.0;
          pMOG->var.Blue = 0.0;

          pMOG->w = 0.0;
          pMOG->sortKey = 0.0;

          pMOG++;
        }

        pK[i] = 0; 
      }
    }

    BGModelMog::~BGModelMog()
    {
      delete [] m_pMOG;
      delete [] m_pK;
    }

    void BGModelMog::setBGModelParameter(int id, int value)
    {
      double dvalue = (double)value/255.0;

      switch(id)
      {
      case 0:
        m_threshold = 100.0*dvalue*dvalue;
        break;

      case 1: 
        m_T = dvalue;
        break;

      case 2:
        m_alpha = dvalue*dvalue*dvalue;
        break;

      case 3: 
        m_noise = 100.0*dvalue;;
        break;
      }

      return;
    }

    void BGModelMog::Init()
    {
      MOGDATA *pMOG = m_pMOG;
      int *pK = m_pK;

      Image<BYTERGB> prgbSrc(m_SrcImage);

      int n = 0;
      for(int i = 0; i < m_height; i++)
      {
        for(int j = 0; j < m_width; j++)
        {
          pMOG[0].mu.Red = prgbSrc[i][j].Red;
          pMOG[0].mu.Green = prgbSrc[i][j].Green;
          pMOG[0].mu.Blue = prgbSrc[i][j].Blue;

          pMOG[0].var.Red = m_noise;
          pMOG[0].var.Green = m_noise;
          pMOG[0].var.Blue = m_noise;

          pMOG[0].w = 1.0;
          pMOG[0].sortKey = pMOG[0].w/sqrt(pMOG[0].var.Red+pMOG[0].var.Green+pMOG[0].var.Blue);

          pK[n] = 1;
          n++;

          pMOG += NUMBERGAUSSIANS;
        }
      }

      return;
    }

    void BGModelMog::Update()
    {
      int kBG;

      MOGDATA *pMOG = m_pMOG;
      int *pK = m_pK;

      Image<BYTERGB> prgbSrc(m_SrcImage);
      Image<BYTERGB> prgbBG(m_BGImage);
      Image<BYTERGB> prgbFG(m_FGImage);

      int n = 0;
      for(int i = 0; i < m_height; i++)
      {
        for(int j = 0; j < m_width; j++)
        {
          double srcR = (double) prgbSrc[i][j].Red;
          double srcG = (double) prgbSrc[i][j].Green;
          double srcB = (double) prgbSrc[i][j].Blue;

          // Find matching distribution

          int kHit = -1;

          for(int k = 0; k < pK[n]; k++)
          {
            // Mahalanobis distance
            double dr = srcR - pMOG[k].mu.Red;
            double dg = srcG - pMOG[k].mu.Green;
            double db = srcB - pMOG[k].mu.Blue;
            double d2 = dr*dr/pMOG[k].var.Red + dg*dg/pMOG[k].var.Green + db*db/pMOG[k].var.Blue;

            if(d2 < m_threshold)
            {
              kHit = k;
              break;
            }
          }

          // Adjust parameters

          // matching distribution found
          if(kHit != -1)
          {
            for(int k = 0; k < pK[n]; k++)
            {
              if(k == kHit)
              {
                pMOG[k].w = pMOG[k].w + m_alpha*(1.0f - pMOG[k].w);

                double d;

                d = srcR - pMOG[k].mu.Red;
                if(d*d > DBL_MIN)
                  pMOG[k].mu.Red += m_alpha*d;

                d = srcG - pMOG[k].mu.Green;
                if(d*d > DBL_MIN)
                  pMOG[k].mu.Green += m_alpha*d;

                d = srcB - pMOG[k].mu.Blue;
                if(d*d > DBL_MIN)
                  pMOG[k].mu.Blue += m_alpha*d;

                d = (srcR - pMOG[k].mu.Red)*(srcR - pMOG[k].mu.Red) - pMOG[k].var.Red;
                if(d*d > DBL_MIN)
                  pMOG[k].var.Red += m_alpha*d;

                d = (srcG - pMOG[k].mu.Green)*(srcG - pMOG[k].mu.Green) - pMOG[k].var.Green;
                if(d*d > DBL_MIN)
                  pMOG[k].var.Green += m_alpha*d;

                d = (srcB - pMOG[k].mu.Blue)*(srcB - pMOG[k].mu.Blue) - pMOG[k].var.Blue;
                if(d*d > DBL_MIN)
                  pMOG[k].var.Blue += m_alpha*d;

                pMOG[k].var.Red = (std::max)(pMOG[k].var.Red,m_noise);
                pMOG[k].var.Green = (std::max)(pMOG[k].var.Green,m_noise);
                pMOG[k].var.Blue = (std::max)(pMOG[k].var.Blue,m_noise);
              }
              else
                pMOG[k].w = (1.0 - m_alpha)*pMOG[k].w;
            }
          }
          // no match found... create new one
          else
          {
            if(pK[n] < NUMBERGAUSSIANS)
              pK[n]++;

            kHit = pK[n] - 1;

            if(pK[n] == 1)
              pMOG[kHit].w = 1.0;
            else
              pMOG[kHit].w = LEARNINGRATEMOG;

            pMOG[kHit].mu.Red = srcR;
            pMOG[kHit].mu.Green = srcG;
            pMOG[kHit].mu.Blue = srcB;

            pMOG[kHit].var.Red = m_noise;
            pMOG[kHit].var.Green = m_noise;
            pMOG[kHit].var.Blue = m_noise;
          }

          // Normalize weights

          double wsum = 0.0;

          for(int k = 0; k < pK[n]; k++) 
            wsum += pMOG[k].w;

          double wfactor = 1.0/wsum;

          for(int k = 0; k < pK[n]; k++)
          {
            pMOG[k].w *= wfactor;
            pMOG[k].sortKey = pMOG[k].w/sqrt(pMOG[k].var.Red+pMOG[k].var.Green+pMOG[k].var.Blue);
          }

          // Sort distributions

          for (int k = 0; k < kHit; k++)
          {
            if(pMOG[kHit].sortKey > pMOG[k].sortKey)
            {
              std::swap(pMOG[kHit],pMOG[k]);
              break;
            }
          }

          // Determine background distributions

          wsum = 0.0;

          for(int k = 0; k < pK[n]; k++)
          {
            wsum += pMOG[k].w;

            if(wsum > m_T)
            {
              kBG = k;
              break;
            }
          }

          if(kHit > kBG)
            prgbFG[i][j].Red = prgbFG[i][j].Green = prgbFG[i][j].Blue = 255;			
          else
            prgbFG[i][j].Red = prgbFG[i][j].Green = prgbFG[i][j].Blue = 0;			

          prgbBG[i][j].Red = (unsigned char)pMOG[0].mu.Red;
          prgbBG[i][j].Green = (unsigned char)pMOG[0].mu.Green;
          prgbBG[i][j].Blue = (unsigned char)pMOG[0].mu.Blue;

          pMOG += NUMBERGAUSSIANS;

          n++;
        }
      }

      return;
    }
  }
}