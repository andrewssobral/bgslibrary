#include "BGModelSom.h"

namespace lb_library
{
  namespace AdaptiveSOM
  {
    BGModelSom::BGModelSom(int width, int height) : BGModel(width, height)
    {
      m_offset = (KERNEL - 1) / 2;

      if (SPAN_NEIGHBORS)
        m_pad = 0;
      else
        m_pad = m_offset;

      // SOM models

      m_widthSOM = m_width*M + 2 * m_offset + (m_width - 1)*m_pad;
      m_heightSOM = m_height*N + 2 * m_offset + (m_height - 1)*m_pad;

      m_ppSOM = new DBLRGB*[m_heightSOM];
      for (int n = 0; n < m_heightSOM; n++)
        m_ppSOM[n] = new DBLRGB[m_widthSOM];

      for (int j = 0; j < m_heightSOM; j++)
      {
        for (int i = 0; i < m_widthSOM; i++)
        {
          m_ppSOM[j][i].Red = 0.0;
          m_ppSOM[j][i].Green = 0.0;
          m_ppSOM[j][i].Blue = 0.0;
        }
      }

      // Create weights

      m_ppW = new double*[KERNEL];
      for (int n = 0; n < KERNEL; n++)
        m_ppW[n] = new double[KERNEL];

      // Construct Gaussian kernel using Pascal's triangle

      int cM;
      int cN;
      m_Wmax = DBL_MIN;

      cN = 1;
      for (int j = 0; j < KERNEL; j++)
      {
        cM = 1;

        for (int i = 0; i < KERNEL; i++)
        {
          m_ppW[j][i] = cN*cM;

          if (m_ppW[j][i] > m_Wmax)
            m_Wmax = m_ppW[j][i];

          cM = cM * (KERNEL - 1 - i) / (i + 1);
        }

        cN = cN * (KERNEL - 1 - j) / (j + 1);
      }

      // Parameters

      m_epsilon1 = EPS1*EPS1;
      m_epsilon2 = EPS2*EPS2;

      m_alpha1 = C1 / m_Wmax;
      m_alpha2 = C2 / m_Wmax;

      m_K = 0;
      m_TSteps = TRAINING_STEPS;
    }

    BGModelSom::~BGModelSom()
    {
      for (int n = 0; n < m_heightSOM; n++)
        delete[] m_ppSOM[n];

      delete[] m_ppSOM;

      for (int n = 0; n < KERNEL; n++)
        delete[] m_ppW[n];

      delete[] m_ppW;
    }

    void BGModelSom::setBGModelParameter(int id, int value)
    {
      double dvalue = (double)value / 255.0;

      switch (id)
      {
      case 0:
        m_epsilon2 = 255.0*255.0*dvalue*dvalue*dvalue*dvalue;
        break;

      case 1:
        m_epsilon1 = 255.0*255.0*dvalue*dvalue*dvalue*dvalue;
        break;

      case 2:
        m_alpha2 = dvalue*dvalue*dvalue / m_Wmax;
        break;

      case 3:
        m_alpha1 = dvalue*dvalue*dvalue / m_Wmax;
        break;

      case 5:
        m_TSteps = (int)(255.0*dvalue);
        break;
      }

      return;
    }

    void BGModelSom::Init()
    {
      Image<BYTERGB> prgbSrc(m_SrcImage);

      for (unsigned int j = 0; j < m_height; j++)
      {
        int jj = m_offset + j*(N + m_pad);

        for (unsigned int i = 0; i < m_width; i++)
        {
          int ii = m_offset + i*(M + m_pad);

          for (int l = 0; l < N; l++)
          {
            for (int k = 0; k < M; k++)
            {
              m_ppSOM[jj + l][ii + k].Red = (double)prgbSrc[j][i].Red;
              m_ppSOM[jj + l][ii + k].Green = (double)prgbSrc[j][i].Green;
              m_ppSOM[jj + l][ii + k].Blue = (double)prgbSrc[j][i].Blue;
            }
          }
        }
      }

      m_K = 0;

      return;
    }

    void BGModelSom::Update()
    {
      double alpha, a;
      double epsilon;

      // calibration phase
      if (m_K <= m_TSteps)
      {
        epsilon = m_epsilon1;
        alpha = (m_alpha1 - m_K*(m_alpha1 - m_alpha2) / m_TSteps);
        m_K++;
      }
      else // online phase
      {
        epsilon = m_epsilon2;
        alpha = m_alpha2;
      }

      Image<BYTERGB> prgbSrc(m_SrcImage);
      Image<BYTERGB> prgbBG(m_BGImage);
      Image<BYTERGB> prgbFG(m_FGImage);

      for (unsigned int j = 0; j < m_height; j++)
      {
        int jj = m_offset + j*(N + m_pad);

        for (unsigned int i = 0; i < m_width; i++)
        {
          int ii = m_offset + i*(M + m_pad);

          double srcR = (double)prgbSrc[j][i].Red;
          double srcG = (double)prgbSrc[j][i].Green;
          double srcB = (double)prgbSrc[j][i].Blue;

          // Find BMU

          double d2min = DBL_MAX;
          int iiHit = ii;
          int jjHit = jj;

          for (int l = 0; l < N; l++)
          {
            for (int k = 0; k < M; k++)
            {
              double dr = srcR - m_ppSOM[jj + l][ii + k].Red;
              double dg = srcG - m_ppSOM[jj + l][ii + k].Green;
              double db = srcB - m_ppSOM[jj + l][ii + k].Blue;

              double d2 = dr*dr + dg*dg + db*db;

              if (d2 < d2min)
              {
                d2min = d2;
                iiHit = ii + k;
                jjHit = jj + l;
              }
            }
          }

          // Update SOM

          if (d2min <= epsilon) // matching model found
          {
            for (int l = (jjHit - m_offset); l <= (jjHit + m_offset); l++)
            {
              for (int k = (iiHit - m_offset); k <= (iiHit + m_offset); k++)
              {
                a = alpha*m_ppW[l - jjHit + m_offset][k - iiHit + m_offset];

                // speed hack.. avoid very small increment values. abs() is sloooow.

                double d;

                d = srcR - m_ppSOM[l][k].Red;
                if (d*d > DBL_MIN)
                  m_ppSOM[l][k].Red += a*d;

                d = srcG - m_ppSOM[l][k].Green;
                if (d*d > DBL_MIN)
                  m_ppSOM[l][k].Green += a*d;

                d = srcB - m_ppSOM[l][k].Blue;
                if (d*d > DBL_MIN)
                  m_ppSOM[l][k].Blue += a*d;
              }
            }

            // Set background image
            prgbBG[j][i].Red = m_ppSOM[jjHit][iiHit].Red;
            prgbBG[j][i].Green = m_ppSOM[jjHit][iiHit].Green;
            prgbBG[j][i].Blue = m_ppSOM[jjHit][iiHit].Blue;

            // Set foreground image
            prgbFG[j][i].Red = prgbFG[j][i].Green = prgbFG[j][i].Blue = 0;
          }
          else
          {
            // Set foreground image
            prgbFG[j][i].Red = prgbFG[j][i].Green = prgbFG[j][i].Blue = 255;
          }
        }
      }

      return;
    }
  }
}
