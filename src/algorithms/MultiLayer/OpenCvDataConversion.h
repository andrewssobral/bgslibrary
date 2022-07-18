#pragma once

#include <stdio.h>

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7

// opencv legacy includes
#include "OpenCvLegacyIncludes.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace multilayer
    {
      template <class TI, class TM>		/* class TI - the type of image data, class TM - the type of matrix data */
      class COpencvDataConversion
      {
      public:

        /* get the image data */
        TI * GetImageData(IplImage *img)
        {
          if (!img->roi) {	/* no ROI used, i.e. the whole image */
            int y; //, x;
            TI* img_data = new TI[img->width*img->height*img->nChannels];
            TI* temp = img_data;
            TI* x_data;

            for (y = 0; y < img->height; y++) {
              x_data = (TI*)(img->imageData + img->widthStep*y);
              int row_length = img->width*img->nChannels;
              memcpy(temp, x_data, sizeof(TI)*row_length);
              temp += row_length;
              /*
              for ( x = 0 ; x < img->width*img->nChannels ; x++ )
              *temp++ = *x_data++;
              */
            }

            return img_data;
          }
          else {	/* get image data only in ROI */
            int y;//, x;
            TI* img_data = new TI[img->roi->width*img->roi->height*img->nChannels];
            TI* temp = img_data;
            TI* x_data;
            for (y = img->roi->yOffset; y < img->roi->yOffset + img->roi->height; y++) {
              x_data = (TI*)(img->imageData + img->widthStep*y + img->roi->xOffset*sizeof(TI)*img->nChannels);
              int row_length = img->roi->width*img->nChannels;
              memcpy(temp, x_data, sizeof(TI)*row_length);
              temp += row_length;
              /*
              for ( x = 0 ; x < img->roi->width*img->nChannels ; x++ )
              *temp++ = *x_data++;
              */
            }
            return img_data;
          }
        };

        /* set the image data */
        void SetImageData(IplImage *img, TI *img_data)
        {
          if (!img->roi) {	/* no ROI used, i.e. the whole image */
            int y;//, x;
            TI* temp = img_data;
            TI* x_data;
            for (y = 0; y < img->height; y++) {
              x_data = (TI*)(img->imageData + img->widthStep*y);
              int row_length = img->width*img->nChannels;
              memcpy(x_data, temp, sizeof(TI)*row_length);
              temp += row_length;
              /*
              for ( x = 0 ; x < img->width*img->nChannels ; x++ )
              *x_data++ = *temp++;
              */
            }
          }
          else {	/* set image data only in ROI */
            int y;//, x;
            TI* temp = img_data;
            TI* x_data;
            for (y = img->roi->yOffset; y < img->roi->yOffset + img->roi->height; y++) {
              x_data = (TI*)(img->imageData + img->widthStep*y + img->roi->xOffset*sizeof(TI)*img->nChannels);
              int row_length = img->roi->width*img->nChannels;
              memcpy(x_data, temp, sizeof(TI)*row_length);
              temp += row_length;
              /*
              for ( x = 0 ; x < img->roi->width*img->nChannels ; x++ )
              *x_data++ = *temp++;
              */
            }
          }
        }

        /* get the matrix data */
        TM * GetMatData(CvMat *mat)
        {
          TM* mat_data = new TM[mat->width*mat->height];
          memcpy(mat_data, mat->data.ptr, sizeof(TM)*mat->width*mat->height);
          return mat_data;

          /*
          int y, x;
          TM* mat_data = new TM[mat->width*mat->height];
          TM* temp = mat_data;
          TM* x_data;
          for ( y = 0 ; y < mat->height ; y++ ) {
          x_data = (TM*)(mat->data.ptr + mat->step*y);
          for ( x = 0 ; x < mat->width ; x++ )
          *temp++ = *x_data++;
          }
          return mat_data;
          */
        };

        /* set the matrix data */
        void SetMatData(CvMat *mat, TM *mat_data)
        {
          memcpy(mat->data.ptr, mat_data, sizeof(TM)*mat->width*mat->height);

          /*
          int y, x;
          TM* temp = mat_data;
          TM* x_data;
          for ( y = 0 ; y < mat->height ; y++ ) {
          x_data = (TM*)(mat->data.ptr + mat->step*y);
          for ( x = 0 ; x < mat->width ; x++ )
          *x_data++ = *temp++;
          }
          */
        }

        /* convert the image data to the matrix data */
        void ConvertData(IplImage *img_src, CvMat *mat_dst)
        {
          if (img_src->nChannels > 1) {
            printf("Must be one-channel image for ConvertImageData!\n");
            exit(1);
          }

          TI* _img_data = GetImageData(img_src);
          TM* _mat_data = new TM[img_src->width*img_src->height];

          TI* img_data = _img_data;
          TM* mat_data = _mat_data;
          int i;
          for (i = 0; i < img_src->width*img_src->height; i++)
            *mat_data++ = (TM)(*img_data++);

          SetMatData(mat_dst, _mat_data);

          delete[] _img_data;
          delete[] _mat_data;
        }

        /* convert the matrix data to the image data */
        void ConvertData(CvMat *mat_src, IplImage *img_dst)
        {
          if (img_dst->nChannels > 1) {
            printf("Must be one-channel image for ConvertImageData!\n");
            exit(1);
          }

          TM* _mat_data = GetMatData(mat_src);
          TI* _img_data = new TI[mat_src->width*mat_src->height];

          TM* mat_data = _mat_data;
          TI* img_data = _img_data;

          int i;
          for (i = 0; i < mat_src->width*mat_src->height; i++)
            *img_data++ = (TI)(*mat_data++);

          SetImageData(img_dst, _img_data);

          delete[] _img_data;
          delete[] _mat_data;
        }

        COpencvDataConversion() {};
        virtual ~COpencvDataConversion() {};
      };
    }
  }
}

#endif
