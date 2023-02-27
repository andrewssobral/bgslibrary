#include "Image.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

//using namespace bgslibrary::algorithms::dp;

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
      ImageBase::~ImageBase()
      {
        if (imgp != NULL && m_bReleaseMemory)
          cvReleaseImage(&imgp);
        imgp = NULL;
      }

      void DensityFilter(BwImage& image, BwImage& filtered, int minDensity, unsigned char fgValue)
      {
        for (int r = 1; r < image.Ptr()->height - 1; ++r)
        {
          for (int c = 1; c < image.Ptr()->width - 1; ++c)
          {
            int count = 0;
            if (image(r, c) == fgValue)
            {
              if (image(r - 1, c - 1) == fgValue)
                count++;
              if (image(r - 1, c) == fgValue)
                count++;
              if (image(r - 1, c + 1) == fgValue)
                count++;
              if (image(r, c - 1) == fgValue)
                count++;
              if (image(r, c + 1) == fgValue)
                count++;
              if (image(r + 1, c - 1) == fgValue)
                count++;
              if (image(r + 1, c) == fgValue)
                count++;
              if (image(r + 1, c + 1) == fgValue)
                count++;

              if (count < minDensity)
                filtered(r, c) = 0;
              else
                filtered(r, c) = fgValue;
            }
            else
            {
              filtered(r, c) = 0;
            }
          }
        }
      }
    }
  }
}

#endif
