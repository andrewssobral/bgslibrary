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
BGModelGauss.cpp

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

#include "BGModelGauss.h"

namespace lb_library
{
  namespace SimpleGaussian
  {
    BGModelGauss::BGModelGauss(int width, int height) : BGModel(width,height)
    {
      m_alpha = ALPHAGAUSS;
      m_threshold = THRESHGAUSS*THRESHGAUSS;
      m_noise = NOISEGAUSS;

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

    BGModelGauss::~BGModelGauss()
    {
      delete [] m_pMu;
      delete [] m_pVar;
    }

    void BGModelGauss::setBGModelParameter(int id, int value)
    {
      double dvalue = (double)value/255.0;

      switch(id)
      {
      case 0:
        m_threshold = 100.0*dvalue*dvalue;
        break;

      case 1:
        m_noise = 100.0*dvalue;
        break;

      case 2: 
        m_alpha = dvalue*dvalue*dvalue;
        break;
      }

      return;
    }

    void BGModelGauss::Init()
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

    void BGModelGauss::Update()
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

          // Mahalanobis distance 

          double dr = srcR - pMu->Red;
          double dg = srcG - pMu->Green;
          double db = srcB - pMu->Blue;

          double d2 = dr*dr/pVar->Red + dg*dg/pVar->Green + db*db/pVar->Blue;

          // Classify

          if(d2 < m_threshold)
            prgbFG[i][j].Red = prgbFG[i][j].Green = prgbFG[i][j].Blue = 0;		
          else
            prgbFG[i][j].Red = prgbFG[i][j].Green = prgbFG[i][j].Blue = 255;		

          // Update parameters

          if(dr*dr > DBL_MIN)
            pMu->Red += m_alpha*dr;

          if(dg*dg > DBL_MIN)
            pMu->Green += m_alpha*dg;

          if(db*db > DBL_MIN)
            pMu->Blue += m_alpha*db;

          double d;

          d = (srcR - pMu->Red)*(srcR - pMu->Red) - pVar->Red;
          if(d*d > DBL_MIN)
            pVar->Red += m_alpha*d;

          d = (srcG - pMu->Green)*(srcG - pMu->Green) - pVar->Green;
          if(d*d > DBL_MIN)
            pVar->Green += m_alpha*d;

          d = (srcB - pMu->Blue)*(srcB - pMu->Blue) - pVar->Blue;
          if(d*d > DBL_MIN)
            pVar->Blue += m_alpha*d;

          pVar->Red = (std::min)(pVar->Red,m_noise);
          pVar->Green = (std::min)(pVar->Green,m_noise);
          pVar->Blue = (std::min)(pVar->Blue,m_noise);

          // Set background

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