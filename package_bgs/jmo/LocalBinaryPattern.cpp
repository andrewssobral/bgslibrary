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
/* --- --- ---
* Copyright (C) 2008--2010 Idiap Research Institute (.....@idiap.ch)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// LocalBinaryPattern.cpp: implementation of the CLocalBinaryPattern class.
//
//////////////////////////////////////////////////////////////////////

#include "LocalBinaryPattern.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLocalBinaryPattern::CLocalBinaryPattern()
{
  m_ppOrgImgs = NULL;
  m_pRadiuses = NULL;
  m_fRobustWhiteNoise = 3.0f;
  m_pNeigPointsNums = NULL;
  m_pXYShifts = NULL;
  m_pShiftedImg = NULL;
}

CLocalBinaryPattern::~CLocalBinaryPattern()
{
  FreeMemories();
}

void CLocalBinaryPattern::Initialization(IplImage **first_imgs, int imgs_num, int level_num, float *radius, int *neig_pt_num, float robust_white_noise, int type)
{

  m_nImgsNum = imgs_num;

  m_nLBPLevelNum = level_num;

  m_pRadiuses = new float[m_nLBPLevelNum];
  m_pNeigPointsNums = new int[m_nLBPLevelNum];
  m_ppOrgImgs = first_imgs;

  int a, b;
  for (a = 0; a < m_nImgsNum; a++) {
    m_cvImgSize = cvGetSize(first_imgs[a]);

    if (first_imgs[a]->nChannels > 1) {
      printf("Input image channel must be 1!");
      exit(1);
    }
  }

  int tot_neig_pts_num = 0;
  for (a = 0; a < m_nLBPLevelNum; a++) {
    m_pRadiuses[a] = radius[a];
    m_pNeigPointsNums[a] = neig_pt_num[a];
    tot_neig_pts_num += neig_pt_num[a];
    if (m_pNeigPointsNums[a] % 2 != 0) {
      printf("Even number must be given for points number for LBP!\n");
      exit(1);
    }
  }

  m_pShiftedImg = cvCloneImage(m_ppOrgImgs[0]);

  m_pXYShifts = new CvPoint[tot_neig_pts_num];

  m_nMaxShift.x = 0;
  m_nMaxShift.y = 0;
  int shift_idx = 0;
  for (a = 0; a < m_nLBPLevelNum; a++)
  for (b = 0; b < m_pNeigPointsNums[a]; b++) {
    // compute the offset of neig point
    CalNeigPixelOffset(m_pRadiuses[a], m_pNeigPointsNums[a], b, m_pXYShifts[shift_idx].x, m_pXYShifts[shift_idx].y);
    m_nMaxShift.x = MAX(m_nMaxShift.x, m_pXYShifts[shift_idx].x);
    m_nMaxShift.y = MAX(m_nMaxShift.y, m_pXYShifts[shift_idx].y);
    shift_idx++;
  }

  m_fRobustWhiteNoise = robust_white_noise;
}

void CLocalBinaryPattern::SetNewImages(IplImage **new_imgs)
{
  m_ppOrgImgs = new_imgs;
}

void CLocalBinaryPattern::ComputeLBP(PixelLBPStruct *PLBP, CvRect *roi)
{
  float *dif_pattern;
  float *_dif_pattern;
  PixelLBPStruct *_PLBP;
  int data_length;
  float *cur_pattern;

  // allocate memories
  if (roi)
    data_length = roi->height*roi->width;
  else
    data_length = m_cvImgSize.width*m_cvImgSize.height;

  dif_pattern = new float[data_length];

  int img_idx, pt_idx, yx, level;
  int pattern_idx = 0;
  for (img_idx = 0; img_idx < m_nImgsNum; img_idx++) {
    for (level = 0; level < m_nLBPLevelNum; level++) {
      for (pt_idx = 0; pt_idx < m_pNeigPointsNums[level]; pt_idx++) {

        // computing the shifted image
        CalShiftedImage(m_ppOrgImgs[img_idx], m_pXYShifts[pattern_idx].x, m_pXYShifts[pattern_idx].y, m_pShiftedImg, roi);

        // computing the different binary images
        CalImageDifferenceMap(m_ppOrgImgs[img_idx], m_pShiftedImg, dif_pattern, roi);

        // set the binary values
        _PLBP = PLBP;
        _dif_pattern = dif_pattern;

        if (roi) {
          int x, y;
          for (y = 0; y < roi->height; y++)  {
            _PLBP = PLBP + (y + roi->y)*m_cvImgSize.width + roi->x;
            for (x = 0; x < roi->width; x++) {
              cur_pattern = (*_PLBP++).cur_pattern;
              cur_pattern[pattern_idx] = *_dif_pattern++;
            }
          }
        }
        else {
          for (yx = 0; yx < data_length; yx++) {
            cur_pattern = (*_PLBP++).cur_pattern;
            cur_pattern[pattern_idx] = *_dif_pattern++;
          }
        }

        pattern_idx++;

      }
    }
  }

  // release memories
  delete[] dif_pattern;

  //delete [] shifted_dif_pattern;
  //cvReleaseImage(&shifted_img);
  //cvReleaseImage(&pattern_img);
}

void CLocalBinaryPattern::FreeMemories()
{
  delete[] m_pRadiuses;
  delete[] m_pNeigPointsNums;
  delete[] m_pXYShifts;
  cvReleaseImage(&m_pShiftedImg);

  m_pXYShifts = NULL;
  m_pRadiuses = NULL;
  m_pNeigPointsNums = NULL;
  m_pShiftedImg = NULL;
}

void CLocalBinaryPattern::SetShiftedMeshGrid(CvSize img_size, float offset_x, float offset_y, CvMat *grid_map_x, CvMat *grid_map_y)
{
  float *gX = (float*)(grid_map_x->data.ptr);
  float *gY = (float*)(grid_map_y->data.ptr);

  int x, y;
  for (y = 0; y < img_size.height; y++) {
    for (x = 0; x < img_size.width; x++) {
      *gX++ = (float)x + offset_x;
      *gY++ = (float)y + offset_y;
    }
  }
}

void CLocalBinaryPattern::CalShiftedImage(IplImage *src, int offset_x, int offset_y, IplImage *dst, CvRect *roi)
{
  CvRect src_roi, dst_roi;
  int roi_width, roi_height;

  if (roi) {
    src_roi.x = MAX(offset_x + roi->x, 0);
    src_roi.y = MAX(offset_y + roi->y, 0);

    dst_roi.x = MAX(-(offset_x + roi->x), roi->x);
    dst_roi.y = MAX(-(offset_y + roi->y), roi->y);

    roi_width = MIN(MIN(roi->width + (int)fabsf((float)offset_x), src->width - src_roi.x), dst->width - dst_roi.x);
    roi_height = MIN(MIN(roi->height + (int)fabsf((float)offset_y), src->height - src_roi.y), dst->height - dst_roi.y);

    src_roi.width = roi_width;
    src_roi.height = roi_height;

    dst_roi.width = roi_width;
    dst_roi.height = roi_height;
  }
  else {
    roi_width = src->width - (int)fabsf((float)offset_x);
    roi_height = src->height - (int)fabsf((float)offset_y);

    src_roi.x = MAX(offset_x, 0);
    src_roi.y = MAX(offset_y, 0);
    src_roi.width = roi_width;
    src_roi.height = roi_height;

    dst_roi.x = MAX(-offset_x, 0);
    dst_roi.y = MAX(-offset_y, 0);
    dst_roi.width = roi_width;
    dst_roi.height = roi_height;
  }

  cvSet(dst, cvScalar(0));

  if (roi_width <= 0 || roi_height <= 0)
    return;

  cvSetImageROI(src, src_roi);
  cvSetImageROI(dst, dst_roi);
  cvCopy(src, dst);
  cvResetImageROI(src);
  cvResetImageROI(dst);
}

void CLocalBinaryPattern::CalNeigPixelOffset(float radius, int tot_neig_pts_num, int neig_pt_idx, int &offset_x, int &offset_y)
{
  float angle = (float)neig_pt_idx / (float)tot_neig_pts_num*2.0f*PI;
  offset_x = cvRound(radius*cosf(angle));
  offset_y = cvRound(-radius*sinf(angle));
}

void CLocalBinaryPattern::CalImageDifferenceMap(IplImage *cent_img, IplImage *neig_img, float *pattern, CvRect *roi)
{
  COpencvDataConversion<uchar, uchar> ODC;

  if (roi) {
    cvSetImageROI(cent_img, *roi);
    cvSetImageROI(neig_img, *roi);
  }

  uchar *_centI = ODC.GetImageData(cent_img);
  uchar *_neigI = ODC.GetImageData(neig_img);
  uchar *centI = _centI;
  uchar *neigI = _neigI;

  float *tmp_pattern = pattern;

  int xy;
  int length;

  if (roi)
    length = roi->height*roi->width;
  else
    length = cent_img->height*cent_img->width;

  for (xy = 0; xy < length; xy++) {
    *tmp_pattern = (float)BINARY_PATTERM_ELEM(*neigI, *centI, m_fRobustWhiteNoise);
    tmp_pattern++;
    centI++;
    neigI++;
  }

  if (roi) {
    cvResetImageROI(cent_img);
    cvResetImageROI(neig_img);
  }

  // release memories
  delete[] _centI;
  delete[] _neigI;

}

