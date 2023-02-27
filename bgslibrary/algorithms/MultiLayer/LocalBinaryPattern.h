#pragma once

#include <cstdio>

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7

#include "BGS.h"
#include "OpenCvDataConversion.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace multilayer
    {
      /************************************************************************/
      /* two types of computing the LBP operators but currently GENERAL_LBP   */
      /* has been implemented.                                                */
      /************************************************************************/
      const int	GENERAL_LBP = 0;
      const int SYMMETRIC_LBP = 1;

      class CLocalBinaryPattern
      {
      public:
        void CalImageDifferenceMap(IplImage *cent_img, IplImage *neig_img, float *pattern, CvRect *roi = NULL);
        void CalNeigPixelOffset(float radius, int tot_neig_pts_num, int neig_pt_idx, int &offset_x, int &offset_y);
        void CalShiftedImage(IplImage *src, int offset_x, int offset_y, IplImage *dst, CvRect *roi = NULL);
        void FreeMemories();
        void ComputeLBP(PixelLBPStruct *PLBP, CvRect *roi = NULL);
        void SetNewImages(IplImage **new_imgs);

        IplImage** m_ppOrgImgs;			/* the original images used for computing the LBP operators */

        void Initialization(IplImage **first_imgs, int imgs_num,
          int level_num, float *radius, int *neig_pt_num,
          float robust_white_noise = 3.0f, int type = GENERAL_LBP);

        CLocalBinaryPattern();
        virtual ~CLocalBinaryPattern();

        float	m_fRobustWhiteNoise;		/* the robust noise value for computing the LBP operator in each channel */

      private:
        void SetShiftedMeshGrid(CvSize img_size, float offset_x, float offset_y, CvMat *grid_map_x, CvMat *grid_map_y);

        float*	m_pRadiuses;			/* the circle radiuses for the LBP operator */
        //int	m_nLBPType;			/* the type of computing LBP operator */
        int*	m_pNeigPointsNums;		/* the numbers of neighboring pixels on multi-level circles */
        int	m_nImgsNum;			/* the number of multi-channel image */
        int	m_nLBPLevelNum;			/* the number of multi-level LBP operator */
        CvSize	m_cvImgSize;			/* the image size (width, height) */

        CvPoint* m_pXYShifts;
        CvPoint	m_nMaxShift;

        IplImage* m_pShiftedImg;
      };
    }
  }
}

#endif
