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
BGModelFuzzyGauss.cpp

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

#include "BGModelFuzzyGauss.h"

namespace lb_library
{
  namespace FuzzyGaussian
  {
    BGModelFuzzyGauss::BGModelFuzzyGauss(int width, int height) : BGModel(width,height)
    {
      m_alphamax = ALPHAFUZZYGAUSS;
      m_threshold = THRESHOLDFUZZYGAUSS * THRESHOLDFUZZYGAUSS;
      m_threshBG = THRESHOLDBG;
      m_noise = NOISEFUZZYGAUSS;

      m_pMu = new DBLRGB[m_width * m_height];
      m_pVar = new DBLRGB[m_width * m_height];

      DBLRGB *pMu = m_pMu;
      DBLRGB *pVar = m_pVar;

      for(int k = 0; k < (m_width * m_height); k++)
      {
        pMu->Red = 0.0;
        pMu->Green = 0.0;
        pMu->Blue = 0.0;

        pVar->Red = m_noise;
        pVar->Green = m_noise;
        pVar->Blue = m_noise;

        pMu++;
        pVar++;
      }
    }

    BGModelFuzzyGauss::~BGModelFuzzyGauss()
    {
      delete [] m_pMu;
      delete [] m_pVar;
    }

    void BGModelFuzzyGauss::setBGModelParameter(int id, int value)
    {
      double dvalue = (double)value/255.0;

      switch(id)
      {
      case 0:
        m_threshold = 100.0*dvalue*dvalue;
        break;

      case 1:
        m_threshBG = dvalue;
        break;

      case 2:
        m_alphamax = dvalue*dvalue*dvalue;
        break;

      case 3:
        m_noise = 100.0*dvalue;
        break;
      }

      return;
    }

    void BGModelFuzzyGauss::Init()
    {
      DBLRGB *pMu = m_pMu;
      DBLRGB *pVar = m_pVar;

      Image<BYTERGB> prgbSrc(m_SrcImage);

      for(int i = 0; i < m_height; i++)
      {
        for(int j = 0; j < m_width; j++)
        {
          pMu->Red = prgbSrc[i][j].Red;
          pMu->Green = prgbSrc[i][j].Green;
          pMu->Blue = prgbSrc[i][j].Blue;

          pVar->Red = m_noise;
          pVar->Green = m_noise;
          pVar->Blue = m_noise;

          pMu++;
          pVar++;
        }
      }

      return;
    }

    void BGModelFuzzyGauss::Update()
    {
      DBLRGB *pMu = m_pMu;
      DBLRGB *pVar = m_pVar;

      Image<BYTERGB> prgbSrc(m_SrcImage);
      Image<BYTERGB> prgbBG(m_BGImage);
      Image<BYTERGB> prgbFG(m_FGImage);

      for(int i = 0; i < m_height; i++)
      {
        for(int j = 0; j < m_width; j++)
        {
          double srcR = (double) prgbSrc[i][j].Red;
          double srcG = (double) prgbSrc[i][j].Green;
          double srcB = (double) prgbSrc[i][j].Blue;

          // Fuzzy background subtraction (Mahalanobis distance)

          double dr = srcR - pMu->Red;
          double dg = srcG - pMu->Green;
          double db = srcB - pMu->Blue;

          double d2 = dr*dr/pVar->Red + dg*dg/pVar->Green + db*db/pVar->Blue;

          double fuzzyBG = 1.0;

          if(d2 < m_threshold)
            fuzzyBG = d2/m_threshold;

          // Fuzzy running average

          double alpha = m_alphamax*exp(FUZZYEXP*fuzzyBG);

          if(dr*dr > DBL_MIN)
            pMu->Red += alpha*dr;

          if(dg*dg > DBL_MIN)
            pMu->Green += alpha*dg;

          if(db*db > DBL_MIN)
            pMu->Blue += alpha*db;

          double d;

          d = (srcR - pMu->Red)*(srcR - pMu->Red) - pVar->Red;
          if(d*d > DBL_MIN)
            pVar->Red += alpha*d;

          d = (srcG - pMu->Green)*(srcG - pMu->Green) - pVar->Green;
          if(d*d > DBL_MIN)
            pVar->Green += alpha*d;

          d = (srcB - pMu->Blue)*(srcB - pMu->Blue) - pVar->Blue;
          if(d*d > DBL_MIN)
            pVar->Blue += alpha*d;

          pVar->Red = (std::max)(pVar->Red,m_noise);
          pVar->Green = (std::max)(pVar->Green,m_noise);
          pVar->Blue = (std::max)(pVar->Blue,m_noise);

          // Set foreground and background

          if(fuzzyBG >= m_threshBG)
            prgbFG[i][j].Red = prgbFG[i][j].Green = prgbFG[i][j].Blue = 255;
          else
            prgbFG[i][j].Red = prgbFG[i][j].Green = prgbFG[i][j].Blue = 0;

          prgbBG[i][j].Red = (unsigned char)pMu->Red;			
          prgbBG[i][j].Green = (unsigned char)pMu->Green;			
          prgbBG[i][j].Blue = (unsigned char)pMu->Blue;

          pMu++;
          pVar++;
        }
      }

      return;
    }
  }
}