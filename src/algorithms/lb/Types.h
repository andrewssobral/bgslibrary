#pragma once

#include <opencv2/opencv.hpp>
// opencv legacy includes
#include <opencv2/core/core_c.h>

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lb
    {
      template<class T> class Image
      {
      private:
        IplImage* imgp;

      public:
        Image(IplImage* img=0) {imgp=img;}
        ~Image(){imgp=0;}
        
        void operator=(IplImage* img) {imgp=img;}
        
        inline T* operator[](const int rowIndx)
        {
          return ((T *)(imgp->imageData + rowIndx*imgp->widthStep));
        }
      };

      typedef struct{
        unsigned char b,g,r;
      } RgbPixel;

      typedef struct{
        unsigned char Blue,Green,Red;
      } BYTERGB;

      typedef struct{
        unsigned int Blue,Green,Red;
      } INTRGB;

      typedef struct{
        float b,g,r;
      }RgbPixelFloat;

      typedef struct{
        double Blue,Green,Red;
      } DBLRGB;

      typedef Image<RgbPixel>       RgbImage;
      typedef Image<RgbPixelFloat>  RgbImageFloat;
      typedef Image<unsigned char>  BwImage;
      typedef Image<float>          BwImageFloat;

      /*
        IplImage* img = cvCreateImage(cvSize(640,480), IPL_DEPTH_32F, 3);
        RgbImageFloat imgA(img);
        for(int i = 0; i < m_height; i++)
          for(int j = 0; j < m_width; j++)
            imgA[i][j].b = 111;
            imgA[i][j].g = 111;
            imgA[i][j].r = 111;
        */
    }
  }
}
