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
// BackgroundSubtraction.cpp: implementation of the CMultiLayerBGS class.
//
//////////////////////////////////////////////////////////////////////

#include "CMultiLayerBGS.h"

#include <ctime>						// clock
#include <cstdlib>						// C standard library
#include <cstdio>						// C I/O (for sscanf)
#include <cstring>						// string manipulation
#include <fstream>						// file I/O
#include <cmath>						// math includes
#include <iostream>                                             // I/O streams
#include <opencv2/legacy/compat.hpp>

using namespace Blob;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiLayerBGS::CMultiLayerBGS() {
  m_nMaxLBPModeNum = MAX_LBP_MODE_NUM;

  m_fModeUpdatingLearnRate = MODE_UPDATING_LEARN_RATE;
  m_f1_ModeUpdatingLearnRate = 1.0f - m_fModeUpdatingLearnRate;

  m_fWeightUpdatingLearnRate = WEIGHT_UPDATING_LEARN_RATE;
  m_f1_WeightUpdatingLearnRate = 1.0f - m_fWeightUpdatingLearnRate;

  m_fRobustColorOffset = ROBUST_COLOR_OFFSET;

  m_fLowInitialModeWeight = LOW_INITIAL_MODE_WEIGHT;

  m_fPatternColorDistBgThreshold = PATTERN_COLOR_DIST_BACKGROUND_THRESHOLD;
  m_fPatternColorDistBgUpdatedThreshold = PATTERN_COLOR_DIST_BACKGROUND_THRESHOLD;

  m_fBackgroundModelPercent = BACKGROUND_MODEL_PERCENT;

  m_nPatternDistSmoothNeigHalfSize = PATTERN_DIST_SMOOTH_NEIG_HALF_SIZE;
  m_fPatternDistConvGaussianSigma = PATTERN_DIST_CONV_GAUSSIAN_SIGMA;

  m_fRobustShadowRate = ROBUST_SHADOW_RATE;
  m_fRobustHighlightRate = ROBUST_HIGHLIGHT_RATE;

  m_nCurImgFrameIdx = 0;

  m_pBkMaskImg = NULL;

  m_bUsedColorLBP = false;
  m_bUsedGradImage = false;

  m_fMinLBPBinaryProb = 0.1f;
  m_f1_MinLBPBinaryProb = 1.0f - m_fMinLBPBinaryProb;

  m_pOrgImg = m_pFgImg = m_pBgImg = m_pFgMaskImg = m_pBgDistImg = m_pEdgeImg = NULL;
  m_ppOrgLBPImgs = NULL;

  m_disableLearning = false;
  m_fSigmaS = 3.0f;
  m_fSigmaR = 0.1f;

  m_fTextureWeight = 0.5f;
  m_fColorWeight = 1.0f - m_fTextureWeight;

  m_fWeightUpdatingConstant = 5.0f;

  m_fReliableBackgroundModeWeight = 0.9f;

  //m_fMinBgLayerWeight = m_fLowInitialModeWeight/50.0f;
  m_fMinBgLayerWeight = 0.0001f;
  //m_fMinBgLayerWeight = 0.88f;

  m_fMinNoisedAngle = 3.0f / 180.0f * PI;
  m_fMinNoisedAngleSine = sinf(m_fMinNoisedAngle);

  m_fFrameDuration = 1.0f / 25.0f; /* 25 frames per second */

  m_fModeUpdatingLearnRatePerSecond = 0.2f;
  m_fWeightUpdatingLearnRatePerSecond = 0.2f;

  m_pROI = NULL;
}

CMultiLayerBGS::~CMultiLayerBGS() {
  int img_length = m_cvImgSize.height * m_cvImgSize.width;
  PixelLBPStruct* PLBP = m_pPixelLBPs;
  for (int yx = 0; yx < img_length; yx++) {
    delete (*PLBP).cur_intensity;
    delete (*PLBP).cur_pattern;
    delete (*PLBP).lbp_idxes;
    for (int a = 0; a < m_nMaxLBPModeNum; a++) {
      delete (*PLBP).LBPs[a].bg_intensity;
      delete (*PLBP).LBPs[a].max_intensity;
      delete (*PLBP).LBPs[a].min_intensity;
      delete (*PLBP).LBPs[a].bg_pattern;
    }
    delete (*PLBP).LBPs;
    PLBP++;
  }
  delete m_pPixelLBPs;

  /* release memories */
  if (m_pFgImg != NULL)
    cvReleaseImage(&m_pFgImg);
  if (m_pBgImg != NULL)
    cvReleaseImage(&m_pBgImg);
  if (m_pBgDistImg != NULL)
    cvReleaseImage(&m_pBgDistImg);
  if (m_ppOrgLBPImgs != NULL) {
    int a;
    for (a = 0; a < m_nLBPImgNum; a++)
      cvReleaseImage(&m_ppOrgLBPImgs[a]);
    delete[] m_ppOrgLBPImgs;
  }
  if (m_pEdgeImg)
    cvReleaseImage(&m_pEdgeImg);
}

void CMultiLayerBGS::ResetAllParameters() {
  m_f1_ModeUpdatingLearnRate = 1.0f - m_fModeUpdatingLearnRate;
  m_f1_WeightUpdatingLearnRate = 1.0f - m_fWeightUpdatingLearnRate;
  m_f1_MinLBPBinaryProb = 1.0f - m_fMinLBPBinaryProb;

  m_fColorWeight = 1.0f - m_fTextureWeight;

  m_fMinNoisedAngleSine = sinf(m_fMinNoisedAngle);

  //m_fMinBgLayerWeight = m_fLowInitialModeWeight/50.0f;
  m_fMinBgLayerWeight = 0.0001f;

  m_cLBP.m_fRobustWhiteNoise = m_fRobustColorOffset;
}

void CMultiLayerBGS::MergeImages(int num, ...) {
  if (num < 1 || num > 9) {
    printf("Error: the number %d of merging images.\n", num);
    exit(0);
  }

  int nCols = 0, nRows = 0;
  switch (num) {
  case 1: nCols = nRows = 1;
    break;
  case 2: nCols = 1;
    nRows = 2;
    break;
  case 3:
  case 4: nCols = 2;
    nRows = 2;
    break;
  case 5:
  case 6: nCols = 3;
    nRows = 2;
    break;
  case 7:
  case 8:
  case 9: nCols = 3;
    nRows = 3;
    break;
  }

  int a, b;

  IplImage** ppIplImg = new IplImage*[num + 1];

  va_list arg_ptr;
  va_start(arg_ptr, num);
  for (a = 0; a < num + 1; a++)
    ppIplImg[a] = va_arg(arg_ptr, IplImage*);
  va_end(arg_ptr);

  CvRect imgROIRect;
  CvSize imgSize = cvGetSize(ppIplImg[0]);
  if (ppIplImg[num] == NULL) { // for the output video
    ppIplImg[num] = cvCreateImage(cvSize(imgSize.width*nCols, imgSize.height * nRows), IPL_DEPTH_8U, ppIplImg[0]->nChannels);
  }

  int img_idx = 0;
  for (a = 0; a < nRows; a++)
  for (b = 0; b < nCols; b++) {
    if (img_idx >= num)
      break;

    imgROIRect = cvRect(b * imgSize.width, a * imgSize.height, imgSize.width, imgSize.height);

    cvSetImageROI(ppIplImg[num], imgROIRect);
    cvCopyImage(ppIplImg[img_idx++], ppIplImg[num]);
    cvResetImageROI(ppIplImg[num]);
  }

  delete[] ppIplImg;
}

void CMultiLayerBGS::Update_MAX_MIN_Intensity(unsigned char *cur_intensity, float *max_intensity, float *min_intensity) {
  int a;
  float curI;
  for (a = 0; a < m_nChannel; a++) {
    curI = (float)cur_intensity[a];

    min_intensity[a] = MIN(curI, min_intensity[a]);
    max_intensity[a] = MAX(curI, max_intensity[a]);
  }
}

void CMultiLayerBGS::UpdateBgPixelColor(unsigned char *cur_intensity, float* bg_intensity) {
  int a;
  for (a = 0; a < m_nChannel; a++)
    bg_intensity[a] = m_f1_ModeUpdatingLearnRate * bg_intensity[a] + m_fModeUpdatingLearnRate * (float)cur_intensity[a];
}

void CMultiLayerBGS::UpdateBgPixelPattern(float *cur_pattern, float *bg_pattern) {
  int a;
  for (a = 0; a < m_nLBPLength; a++)
    bg_pattern[a] = m_f1_ModeUpdatingLearnRate * bg_pattern[a] + m_fModeUpdatingLearnRate * cur_pattern[a];
}

/* sort everything inbetween `low' <-> `high' */
void CMultiLayerBGS::QuickSort(float *pData, unsigned short *pIdxes, long low, long high, bool bAscent) {
  long i = low;
  long j = high;
  float y = 0;
  int idx = 0;

  /* compare value */
  float z = pData[(low + high) / 2];

  /* partition */
  do {
    if (bAscent) {
      /* find member above ... */
      while (pData[i] < z) i++;

      /* find element below ... */
      while (pData[j] > z) j--;
    }
    else {
      /* find member below ... */
      while (pData[i] > z) i++;

      /* find element above ... */
      while (pData[j] < z) j--;
    }

    if (i <= j) {
      /* swap two elements */
      y = pData[i];
      pData[i] = pData[j];
      pData[j] = y;

      idx = pIdxes[i];
      pIdxes[i] = pIdxes[j];
      pIdxes[j] = idx;

      i++;
      j--;
    }
  } while (i <= j);

  /* recurse */
  if (low < j)
    QuickSort(pData, pIdxes, low, j, bAscent);

  if (i < high)
    QuickSort(pData, pIdxes, i, high, bAscent);
}

float CMultiLayerBGS::DistLBP(LBPStruct *LBP1, LBPStruct *LBP2) {
  int a;

  float pattern_dist = 0;
  for (a = 0; a < m_nLBPLength; a++) {
    pattern_dist = fabsf(LBP1->bg_pattern[a] - LBP1->bg_pattern[a]);
  }
  pattern_dist /= (float)m_nLBPLength;

  float color_dist = 0;
  for (a = 0; a < m_nChannel; a++) {
    color_dist += fabsf((float)LBP1->bg_intensity[a] - (float)LBP2->bg_intensity[a]);
  }
  color_dist /= 3.0f * 125.0f;

  //return MAX(pattern_dist, color_dist);
  return color_dist;
}

void CMultiLayerBGS::SetNewImage(IplImage *new_img, CvRect *roi) {
  m_pOrgImg = new_img;
  m_pROI = roi;
  if (roi && (roi->width <= 0 || roi->height <= 0))
    return;

  if (roi) {
    cvSetImageROI(m_pOrgImg, *roi);
    for (int a = 0; a < m_nLBPImgNum; a++)
      cvSetImageROI(m_ppOrgLBPImgs[a], *roi);
  }

  switch (m_nLBPImgNum) {
  case 1:
    cvCvtColor(m_pOrgImg, m_ppOrgLBPImgs[0], CV_BGR2GRAY);
    break;
  case 2:
    cvCvtColor(m_pOrgImg, m_ppOrgLBPImgs[0], CV_BGR2GRAY);
    ComputeGradientImage(m_ppOrgLBPImgs[0], m_ppOrgLBPImgs[1], false);
    break;
  case 3:
    cvSplit(m_pOrgImg, m_ppOrgLBPImgs[0], m_ppOrgLBPImgs[1], m_ppOrgLBPImgs[2], NULL);
    break;
  case 4:
    cvSplit(m_pOrgImg, m_ppOrgLBPImgs[0], m_ppOrgLBPImgs[1], m_ppOrgLBPImgs[2], NULL);
    ComputeGradientImage(m_ppOrgLBPImgs[0], m_ppOrgLBPImgs[3], false);
    break;
  }

  if (roi) {
    cvResetImageROI(m_pOrgImg);
    for (int a = 0; a < m_nLBPImgNum; a++)
      cvResetImageROI(m_ppOrgLBPImgs[a]);
  }
  m_cLBP.SetNewImages(m_ppOrgLBPImgs);

  m_nCurImgFrameIdx++;
}

void CMultiLayerBGS::SetBkMaskImage(IplImage *mask_img) {
  if (m_pBkMaskImg == NULL) {
    m_pBkMaskImg = cvCreateImage(cvGetSize(mask_img), mask_img->depth, mask_img->nChannels);
  }
  cvCopyImage(mask_img, m_pBkMaskImg);
}

void CMultiLayerBGS::BackgroundSubtractionProcess() {
  CvRect *roi = m_pROI;

  if (roi && (roi->width <= 0 || roi->height <= 0))
    return;
  LBPStruct* LBPs;
  unsigned int bg_num;
  float* cur_pattern;
  unsigned char* cur_intensity;
  int a, b;
  unsigned int lbp_num;
  unsigned short* lbp_idxes;
  unsigned short cur_lbp_idx;
  bool bBackgroundUpdating;

  PixelLBPStruct *PLBP = m_pPixelLBPs;

  bool bFirstFrame = (PLBP[0].num == 0);
  float best_match_bg_dist, bg_pattern_dist, bg_color_dist, bg_pattern_color_dist;

  // compute the local binary pattern
  if (m_fTextureWeight > 0)
    m_cLBP.ComputeLBP(PLBP, roi);

  LBPStruct* curLBP;

  int data_length;

  if (roi)
    data_length = roi->width * roi->height;
  else
    data_length = m_cvImgSize.width * m_cvImgSize.height;

  int best_match_idx;

  COpencvDataConversion<uchar, uchar> ODC1;
  if (roi) {
    cvSetImageROI(m_pBkMaskImg, *roi);
    cvSetImageROI(m_pOrgImg, *roi);
  }
  uchar *_mask = ODC1.GetImageData(m_pBkMaskImg);
  uchar *_org_intensity = ODC1.GetImageData(m_pOrgImg);

  if (roi) {
    cvResetImageROI(m_pBkMaskImg);
    cvResetImageROI(m_pOrgImg);
  }

  COpencvDataConversion<float, float> ODC2;
  float *_bg_dist = new float[data_length];

  uchar *mask = _mask;
  uchar *org_intensity = _org_intensity;
  float *bg_dist = _bg_dist;

  bool removed_modes[10];

  // scanning the point via first x-axis and then y-axis
  int x, y;

  for (y = 0; y < (roi ? roi->height : m_cvImgSize.height); y++) {
    if (roi)
      PLBP = m_pPixelLBPs + (roi->y + y) * m_cvImgSize.width + roi->x;
    else
      PLBP = m_pPixelLBPs + y * m_cvImgSize.width;

    for (x = 0; x < (roi ? roi->width : m_cvImgSize.width); x++) {
      // check whether the current pixel is the pixel to be modeled
      if (*mask++ == 0) {
        PLBP++;
        *bg_dist++ = 0.0f; //m_fPatternColorDistBgThreshold*1.01f;
        org_intensity += m_nChannel;
        continue;
      }

      // removing the background layers
      if (!m_disableLearning) {
        RemoveBackgroundLayers(PLBP);
      }

      // check whether the current image is the first image
      bFirstFrame = ((*PLBP).num == 0);

      // get lbp information
      lbp_num = (*PLBP).num;
      LBPs = (*PLBP).LBPs;
      lbp_idxes = (*PLBP).lbp_idxes;

      (*PLBP).cur_bg_layer_no = 0;

      // set the current pixel's intensity
      cur_intensity = (*PLBP).cur_intensity;
      for (a = 0; a < m_nChannel; a++)
        cur_intensity[a] = *org_intensity++;

      // get the current lbp pattern
      cur_pattern = (*PLBP).cur_pattern;

      // first check whether the pixel is background or foreground and then update the background pattern model
      if (lbp_num == 0) { // empty pattern list
        curLBP = (&(LBPs[0]));
        for (a = 0; a < m_nLBPLength; a++) {
          curLBP->bg_pattern[a] = (float)cur_pattern[a];
        }

        curLBP->bg_layer_num = 0;
        curLBP->weight = m_fLowInitialModeWeight;
        curLBP->max_weight = m_fLowInitialModeWeight;

        curLBP->first_time = m_nCurImgFrameIdx;
        curLBP->last_time = m_nCurImgFrameIdx;
        curLBP->freq = 1;

        (*PLBP).matched_mode_first_time = (float)m_nCurImgFrameIdx;

        for (a = 0; a < m_nChannel; a++) {
          curLBP->bg_intensity[a] = (float)cur_intensity[a];
          curLBP->min_intensity[a] = (float)cur_intensity[a];
          curLBP->max_intensity[a] = (float)cur_intensity[a];
        }

        lbp_idxes[0] = 0;

        lbp_num++;
        (*PLBP).num = 1;
        (*PLBP).bg_num = 1;

        PLBP++;
        *bg_dist++ = 0.0f;
        continue;
      }
      else { // not empty pattern list
        /*
        // remove the background layers
        // end of removing the background layer
        */

        best_match_idx = -1;
        best_match_bg_dist = 999.0f;

        // find the best match
        for (a = 0; a < (int)lbp_num; a++) {
          // get the current index for lbp pattern
          cur_lbp_idx = lbp_idxes[a];

          // get the current LBP pointer
          curLBP = &(LBPs[cur_lbp_idx]);

          // compute the background probability based on lbp pattern
          bg_pattern_dist = 0.0f;
          if (m_fTextureWeight > 0)
            bg_pattern_dist = CalPatternBgDist(cur_pattern, curLBP->bg_pattern);

          // compute the color invariant probability based on RGB color
          bg_color_dist = 0.0f;
          if (m_fColorWeight > 0)
            bg_color_dist = CalColorBgDist(cur_intensity, curLBP->bg_intensity, curLBP->max_intensity, curLBP->min_intensity);

          // compute the joint background probability
          //bg_pattern_color_dist = sqrtf(bg_color_dist*bg_pattern_dist);

          //UpdatePatternColorDistWeights(cur_pattern, curLBP->bg_pattern);

          bg_pattern_color_dist = m_fColorWeight * bg_color_dist + m_fTextureWeight*bg_pattern_dist;
          //bg_pattern_color_dist = MAX(bg_color_dist, bg_pattern_dist);

          //bg_pattern_color_dist = 1.0f - (1.0f-bg_color_dist)*(1.0-bg_pattern_dist);
          //bg_pattern_color_dist = bg_pattern_dist;

          //bg_pattern_color_dist = bg_color_dist;

          if (bg_pattern_color_dist < best_match_bg_dist) {
            best_match_bg_dist = bg_pattern_color_dist;
            best_match_idx = a;
          }
        }

        bg_num = (*PLBP).bg_num;

        // check
        bBackgroundUpdating = ((best_match_bg_dist < m_fPatternColorDistBgUpdatedThreshold));

        // reset the weight of the mode
        if (best_match_idx >= (int)bg_num && LBPs[lbp_idxes[best_match_idx]].max_weight < m_fReliableBackgroundModeWeight) // found not in the background models
          best_match_bg_dist = MAX(best_match_bg_dist, m_fPatternColorDistBgThreshold * 2.5f);

        *bg_dist = best_match_bg_dist;
      }
      if (m_disableLearning) {
        // no creation or update when learning is disabled
      }
      else if (!bBackgroundUpdating) { // no match

        for (a = 0; a < (int)lbp_num; a++) { // decrease the weights
          curLBP = &(LBPs[lbp_idxes[a]]);
          curLBP->weight *= (1.0f - m_fWeightUpdatingLearnRate / (1.0f + m_fWeightUpdatingConstant * curLBP->max_weight));
        }

        if ((int)lbp_num < m_nMaxLBPModeNum) { // add a new pattern
          // find the pattern index for addition
          int add_lbp_idx = 0;
          bool bFound;
          for (a = 0; a < m_nMaxLBPModeNum; a++) {
            bFound = true;
            for (b = 0; b < (int)lbp_num; b++)
              bFound &= (a != lbp_idxes[b]);
            if (bFound) {
              add_lbp_idx = a;
              break;
            }
          }
          curLBP = &(LBPs[add_lbp_idx]);

          curLBP->first_time = m_nCurImgFrameIdx;
          curLBP->last_time = m_nCurImgFrameIdx;
          curLBP->freq = 1;
          curLBP->layer_time = -1;

          (*PLBP).matched_mode_first_time = (float)m_nCurImgFrameIdx;

          for (a = 0; a < m_nLBPLength; a++) {
            curLBP->bg_pattern[a] = (float)cur_pattern[a];
          }

          curLBP->bg_layer_num = 0;
          curLBP->weight = m_fLowInitialModeWeight;
          curLBP->max_weight = m_fLowInitialModeWeight;

          for (a = 0; a < m_nChannel; a++) {
            curLBP->bg_intensity[a] = (float)cur_intensity[a];
            curLBP->min_intensity[a] = (float)cur_intensity[a];
            curLBP->max_intensity[a] = (float)cur_intensity[a];
          }

          lbp_idxes[lbp_num] = add_lbp_idx;

          lbp_num++;
          (*PLBP).num = lbp_num;
        }
        else { // replacing the pattern with the minimal weight
          // find the replaced pattern index
          /*
          int rep_pattern_idx = -1;
          for ( a = m_nLBPLength-1 ; a >= 0 ; a-- ) {
          if ( LBPs[lbp_idxes[a]].bg_layer_num == 0 )
          rep_pattern_idx = lbp_idxes[a];
          }
          if ( rep_pattern_idx < 0 ) {
          rep_pattern_idx = lbp_idxes[m_nMaxLBPModeNum-1];
          for ( a = 0 ; a < m_nLBPLength ; a++ ) {
          if ( LBPs[lbp_idxes[a]].bg_layer_num > LBPs[rep_pattern_idx].bg_layer_num )
          LBPs[lbp_idxes[a]].bg_layer_num--;
          }
          }
          */
          int rep_pattern_idx = lbp_idxes[m_nMaxLBPModeNum - 1];

          curLBP = &(LBPs[rep_pattern_idx]);

          curLBP->first_time = m_nCurImgFrameIdx;
          curLBP->last_time = m_nCurImgFrameIdx;
          curLBP->freq = 1;
          curLBP->layer_time = -1;

          (*PLBP).matched_mode_first_time = (float)m_nCurImgFrameIdx;

          for (a = 0; a < m_nLBPLength; a++) {
            curLBP->bg_pattern[a] = (float)cur_pattern[a];
          }

          curLBP->bg_layer_num = 0;
          curLBP->weight = m_fLowInitialModeWeight;
          curLBP->max_weight = m_fLowInitialModeWeight;

          for (a = 0; a < m_nChannel; a++) {
            curLBP->bg_intensity[a] = (float)cur_intensity[a];
            curLBP->min_intensity[a] = (float)cur_intensity[a];
            curLBP->max_intensity[a] = (float)cur_intensity[a];
          }
        }
      }
      else { // find match
        // updating the background pattern model
        cur_lbp_idx = lbp_idxes[best_match_idx];
        curLBP = &(LBPs[cur_lbp_idx]);

        curLBP->first_time = MAX(MIN(curLBP->first_time, m_nCurImgFrameIdx), 0);
        (*PLBP).matched_mode_first_time = curLBP->first_time;

        curLBP->last_time = m_nCurImgFrameIdx;
        curLBP->freq++;

        if (m_fColorWeight > 0) {
          // update the color information
          UpdateBgPixelColor(cur_intensity, curLBP->bg_intensity);
          // update the MAX and MIN color intensity
          Update_MAX_MIN_Intensity(cur_intensity, curLBP->max_intensity, curLBP->min_intensity);
        }

        // update the texture information
        if (m_fTextureWeight > 0)
          UpdateBgPixelPattern(cur_pattern, curLBP->bg_pattern);


        // increase the weight of the best matched mode
        float increasing_weight_factor = m_fWeightUpdatingLearnRate * (1.0f + m_fWeightUpdatingConstant * curLBP->max_weight);
        curLBP->weight = (1.0f - increasing_weight_factor) * curLBP->weight + increasing_weight_factor; //*expf(-best_match_dist/m_fPatternColorDistBgThreshold);

        // update the maximal weight for the best matched mode
        curLBP->max_weight = MAX(curLBP->weight, curLBP->max_weight);

        // calculate the number of background layer
        if (curLBP->bg_layer_num > 0) {
          bool removed_bg_layers = false;
          if (curLBP->weight > curLBP->max_weight * 0.2f) {
            for (a = 0; a < (int)lbp_num; a++) {
              removed_modes[a] = false;
              if (LBPs[lbp_idxes[a]].bg_layer_num > curLBP->bg_layer_num &&
                LBPs[lbp_idxes[a]].weight < LBPs[lbp_idxes[a]].max_weight * 0.9f) { /* remove layers */
                //LBPs[lbp_idxes[a]].bg_layer_num = 0;
                removed_modes[a] = true;
                removed_bg_layers = true;
              }
            }
          }

          if (removed_bg_layers) {
            RemoveBackgroundLayers(PLBP, removed_modes);
            lbp_num = (*PLBP).num;
          }
        }
        else if (curLBP->max_weight > m_fReliableBackgroundModeWeight && curLBP->bg_layer_num == 0) {
          int max_bg_layer_num = LBPs[lbp_idxes[0]].bg_layer_num;
          for (a = 1; a < (int)lbp_num; a++)
            max_bg_layer_num = MAX(max_bg_layer_num, LBPs[lbp_idxes[a]].bg_layer_num);
          curLBP->bg_layer_num = max_bg_layer_num + 1;
          curLBP->layer_time = m_nCurImgFrameIdx;
        }

        (*PLBP).cur_bg_layer_no = curLBP->bg_layer_num;

        // decrease the weights of non-best matched modes
        for (a = 0; a < (int)lbp_num; a++) {
          if (a != best_match_idx) {
            curLBP = &(LBPs[lbp_idxes[a]]);
            curLBP->weight *= (1.0f - m_fWeightUpdatingLearnRate / (1.0f + m_fWeightUpdatingConstant * curLBP->max_weight));
          }
        }
      }

      // sort the list of modes based on the weights of modes
      if ((int)lbp_num > 1 && !m_disableLearning) {
        float weights[100], tot_weights = 0;
        for (a = 0; a < (int)lbp_num; a++) {
          weights[a] = LBPs[lbp_idxes[a]].weight;
          tot_weights += weights[a];
        }

        // sort weights in the descent order
        QuickSort(weights, lbp_idxes, 0, (int)lbp_num - 1, false);

        // calculate the first potential background modes number, bg_num
        float threshold_weight = m_fBackgroundModelPercent*tot_weights;
        tot_weights = 0;
        for (a = 0; a < (int)lbp_num; a++) {
          tot_weights += LBPs[lbp_idxes[a]].weight;
          if (tot_weights > threshold_weight) {
            bg_num = a + 1;
            break;
          }
        }
        (*PLBP).bg_num = bg_num;
      }

      PLBP++;
      bg_dist++;
    }
  }

  if (bFirstFrame) { // check whether it is the first frame for background modeling
    if (m_pFgMaskImg)
      cvSetZero(m_pFgMaskImg);
    cvSetZero(m_pBgDistImg);
  }
  else {
    // set the image data
    if (roi) {
      cvSetZero(m_pBgDistImg);
      cvSetImageROI(m_pBgDistImg, *roi);
    }
    ODC2.SetImageData(m_pBgDistImg, _bg_dist);

    // do gaussian smooth
    if (m_nPatternDistSmoothNeigHalfSize >= 0)
      cvSmooth(m_pBgDistImg, m_pBgDistImg, CV_GAUSSIAN, (2 * m_nPatternDistSmoothNeigHalfSize + 1), (2 * m_nPatternDistSmoothNeigHalfSize + 1), m_fPatternDistConvGaussianSigma);

    if (roi)
      cvResetImageROI(m_pBgDistImg);
#ifdef LINUX_BILATERAL_FILTER
    // do cross bilateral filter
    fprintf(stderr, "%f %f\n", m_fSigmaS, m_fSigmaR);
    if (m_fSigmaS > 0 && m_fSigmaR > 0) {
      GetFloatEdgeImage(m_ppOrgLBPImgs[0], m_pEdgeImg);
      //ComputeGradientImage(m_ppOrgLBPImgs[0], m_pEdgeImg, true);
      m_cCrossBF.SetNewImages(m_pBgDistImg, m_pEdgeImg);
      m_cCrossBF.FastCrossBF();
      m_cCrossBF.GetFilteredImage(m_pBgDistImg);
    }
#endif

    // get the foreground mask by thresholding
    if (m_pFgMaskImg)
      cvThreshold(m_pBgDistImg, m_pFgMaskImg, m_fPatternColorDistBgThreshold, 255, CV_THRESH_BINARY);

    // get the foreground probability image (uchar)
    if (m_pFgProbImg)
      GetForegroundProbabilityImage(m_pFgProbImg);

    // do post-processing
    //Postprocessing();
  }

  // release memories
  delete[] _mask;
  delete[] _bg_dist;
  delete[] _org_intensity;
}

void CMultiLayerBGS::GetBackgroundImage(IplImage *bk_img) {
  IplImage *bg_img = m_pBgImg;
  uchar *c1;
  float *c2;
  int bg_img_idx;
  int channel;
  int img_length = m_cvImgSize.height * m_cvImgSize.width;
  int yx;

  COpencvDataConversion<uchar, uchar> ODC;
  uchar *org_data = ODC.GetImageData(bg_img);
  c1 = org_data;

  //c1 = (uchar*)(bg_img->imageData);

  PixelLBPStruct* PLBP = m_pPixelLBPs;

  for (yx = 0; yx < img_length; yx++) {
    // the newest background image
    bg_img_idx = (*PLBP).lbp_idxes[0];
    if ((*PLBP).num == 0) {
      for (channel = 0; channel < m_nChannel; channel++)
        *c1++ = 0;
    }
    else {
      c2 = (*PLBP).LBPs[bg_img_idx].bg_intensity;
      for (channel = 0; channel < m_nChannel; channel++)
        *c1++ = cvRound(*c2++);
    }
    PLBP++;
  }

  ODC.SetImageData(bg_img, org_data);
  delete[] org_data;

  cvCopyImage(m_pBgImg, bk_img);
}

void CMultiLayerBGS::GetForegroundImage(IplImage *fg_img, CvScalar bg_color) {
  if (m_pROI && (m_pROI->width <= 0 || m_pROI->height <= 0))
    return;
  IplImage* org_img;
  IplImage* fg_mask_img; // the Mat pointer of the foreground mask matrices at different levels

  org_img = m_pOrgImg;
  fg_mask_img = m_pFgMaskImg;

  cvSet(fg_img, bg_color);
  if (m_pROI) {
    cvSetImageROI(org_img, *m_pROI);
    cvSetImageROI(fg_img, *m_pROI);
    cvSetImageROI(fg_mask_img, *m_pROI);
    cvCopy(org_img, fg_img, fg_mask_img);
    cvResetImageROI(org_img);
    cvResetImageROI(fg_img);
    cvResetImageROI(fg_mask_img);
  }
  else
    cvCopy(org_img, fg_img, fg_mask_img);
}

void CMultiLayerBGS::GetForegroundMaskImage(IplImage *fg_mask_img) {
  if (m_pROI && (m_pROI->width <= 0 || m_pROI->height <= 0))
    return;

  //cvCopyImage(m_pFgMaskImg, fg_mask_img);
  if (m_pROI) {
    cvSetImageROI(m_pFgMaskImg, *m_pROI);
    cvSetImageROI(fg_mask_img, *m_pROI);
    cvThreshold(m_pFgMaskImg, fg_mask_img, 0, 255, CV_THRESH_BINARY);
    cvResetImageROI(m_pFgMaskImg);
    cvResetImageROI(fg_mask_img);
  }
  else
    cvThreshold(m_pFgMaskImg, fg_mask_img, 0, 255, CV_THRESH_BINARY);
}

void CMultiLayerBGS::GetForegroundMaskMap(CvMat *fg_mask_mat) {
  COpencvDataConversion<uchar, uchar> ODC;
  ODC.ConvertData(m_pFgMaskImg, fg_mask_mat);
}

void CMultiLayerBGS::GetCurrentBackgroundDistMap(CvMat *bk_dist_map) {
  cvCopy(m_pBgDistImg, bk_dist_map);
}

void CMultiLayerBGS::Initialization(IplImage *first_img, int lbp_level_num, float *radiuses, int *neig_pt_nums) {
  int a;

  m_nLBPLength = 0;
  m_nLBPLevelNum = lbp_level_num;
  for (a = 0; a < lbp_level_num; a++) {
    m_nLBPLength += neig_pt_nums[a];
    m_pLBPRadiuses[a] = radiuses[a];
    m_pLBPMeigPointNums[a] = neig_pt_nums[a];
  }

  m_pFgImg = NULL;
  m_pFgMaskImg = NULL;
  m_pBgDistImg = NULL;
  m_pOrgImg = NULL;
  m_pBgImg = NULL;
  m_ppOrgLBPImgs = NULL;
  m_pFgProbImg = NULL;

  m_cvImgSize = cvGetSize(first_img);

  m_nChannel = first_img->nChannels;

  m_pOrgImg = first_img;

  if (m_bUsedColorLBP && m_bUsedGradImage)
    m_nLBPImgNum = 4;
  else if (m_bUsedColorLBP && !m_bUsedGradImage)
    m_nLBPImgNum = 3;
  else if (!m_bUsedColorLBP && m_bUsedGradImage)
    m_nLBPImgNum = 2;
  else
    m_nLBPImgNum = 1;

  m_nLBPLength *= m_nLBPImgNum;

  m_ppOrgLBPImgs = new IplImage*[m_nLBPImgNum];
  for (a = 0; a < m_nLBPImgNum; a++)
    m_ppOrgLBPImgs[a] = cvCreateImage(m_cvImgSize, IPL_DEPTH_8U, 1);

  m_pBgImg = cvCreateImage(m_cvImgSize, IPL_DEPTH_8U, m_nChannel);
  m_pFgImg = cvCreateImage(m_cvImgSize, IPL_DEPTH_8U, m_nChannel);
  m_pEdgeImg = cvCreateImage(m_cvImgSize, IPL_DEPTH_32F, 1);
  m_pBgDistImg = cvCreateImage(m_cvImgSize, IPL_DEPTH_32F, 1);

  ResetAllParameters();

  int img_length = m_cvImgSize.height * m_cvImgSize.width;
  m_pPixelLBPs = new PixelLBPStruct[img_length];
  PixelLBPStruct* PLBP = m_pPixelLBPs;
  int yx;
  for (yx = 0; yx < img_length; yx++) {
    (*PLBP).cur_intensity = new unsigned char[m_nChannel];
    (*PLBP).cur_pattern = new float[m_nLBPLength];
    (*PLBP).LBPs = new LBPStruct[m_nMaxLBPModeNum];
    (*PLBP).lbp_idxes = new unsigned short[m_nMaxLBPModeNum];
    (*PLBP).lbp_idxes[0] = 0;
    (*PLBP).num = 0;
    (*PLBP).cur_bg_layer_no = 0;
    (*PLBP).matched_mode_first_time = 0;
    for (a = 0; a < m_nMaxLBPModeNum; a++) {
      (*PLBP).LBPs[a].bg_intensity = new float[m_nChannel];
      (*PLBP).LBPs[a].max_intensity = new float[m_nChannel];
      (*PLBP).LBPs[a].min_intensity = new float[m_nChannel];
      (*PLBP).LBPs[a].bg_pattern = new float[m_nLBPLength];
      (*PLBP).LBPs[a].first_time = -1;
      (*PLBP).LBPs[a].last_time = -1;
      (*PLBP).LBPs[a].freq = -1;
      (*PLBP).LBPs[a].layer_time = -1;
    }
    PLBP++;
  }

  m_pBkMaskImg = cvCreateImage(m_cvImgSize, IPL_DEPTH_8U, 1);
  cvSet(m_pBkMaskImg, cvScalar(1));

  m_cLBP.Initialization(m_ppOrgLBPImgs, m_nLBPImgNum, lbp_level_num, radiuses, neig_pt_nums, m_fRobustColorOffset);

#ifdef LINUX_BILATERAL_FILTER
  if (m_fSigmaS > 0 && m_fSigmaR > 0)
    m_cCrossBF.Initialization(m_pBgDistImg, m_pBgDistImg, m_fSigmaS, m_fSigmaR);
#endif
}

float CMultiLayerBGS::CalPatternBgDist(float *cur_pattern, float *bg_pattern) {
  float bg_hamming_dist = 0;
  int a;
  for (a = 0; a < m_nLBPLength; a++)
    bg_hamming_dist += fabsf(cur_pattern[a] - bg_pattern[a]) > m_f1_MinLBPBinaryProb;

  bg_hamming_dist /= (float)m_nLBPLength;

  return bg_hamming_dist;
}

float CMultiLayerBGS::CalColorBgDist(uchar *cur_intensity, float *bg_intensity, float *max_intensity, float *min_intensity) {
  float noised_angle, range_dist, bg_color_dist;

  range_dist = CalColorRangeDist(cur_intensity, bg_intensity, max_intensity, min_intensity, m_fRobustShadowRate, m_fRobustHighlightRate);

  if (range_dist == 1.0f)
    bg_color_dist = range_dist;
  else {
    noised_angle = CalVectorsNoisedAngle(bg_intensity, cur_intensity, MAX(m_fRobustColorOffset, 5.0f), m_nChannel);
    bg_color_dist = (1.0f - expf(-100.0f * noised_angle * noised_angle));
  }

  //float bg_color_dist = (expf(-100.0f*noised_angle*noised_angle)*(1.0f-range_dist);

  //float bg_color_dist =  0.5f*(1.0f-expf(-noised_angle*noised_angle/0.005f)) + 0.5f*range_dist;
  //float bg_color_dist =  MAX((float)(noised_angle>0.08f), range_dist);

  return bg_color_dist;
}

void CMultiLayerBGS::ComputeGradientImage(IplImage *src, IplImage *dst, bool bIsFloat) {
  if (src->nChannels != 1 || dst->nChannels != 1) {
    printf("Input images error for computing gradient images!");
    exit(1);
  }

  int a;

  IplImage* _dX = cvCreateImage(cvGetSize(dst), IPL_DEPTH_16S, 1);
  IplImage* _dY = cvCreateImage(cvGetSize(dst), IPL_DEPTH_16S, 1);

  int aperture_size = 3;

  cvSobel(src, _dX, 1, 0, aperture_size);
  cvSobel(src, _dY, 0, 1, aperture_size);

  COpencvDataConversion<short, short> ODC1;
  COpencvDataConversion<uchar, uchar> ODC2;
  COpencvDataConversion<float, float> ODC3;

  short* dX_data = ODC1.GetImageData(_dX);
  short* dY_data = ODC1.GetImageData(_dY);

  uchar* dst_u_data = NULL;
  float* dst_f_data = NULL;

  if (bIsFloat)
    dst_f_data = ODC3.GetImageData(dst);
  else
    dst_u_data = ODC2.GetImageData(dst);

  short* dX = dX_data;
  short* dY = dY_data;
  uchar *uSrc = dst_u_data;
  float *fSrc = dst_f_data;

  /*
  short* dX = (short*)(_dX->imageData);
  short* dY = (short*)(_dY->imageData);
  uchar *dSrc = (uchar*)(dst->imageData);
  */

  int length;
  if (src->roi)
    length = dst->width * dst->height;
  else
    length = dst->roi->width * dst->roi->height;
  /*
  int x, y;
  uchar *x_u_data;
  float *x_f_data;
  */

  if (bIsFloat) {
    for (a = 0; a < length; a++) {
      *fSrc = cvSqrt((float)((*dX)*(*dX) + (*dY)*(*dY)) / (32.0f * 255.0f));
      fSrc++;
      dX++;
      dY++;
    }
    ODC3.SetImageData(dst, dst_f_data);
    delete[] dst_f_data;
  }
  else {
    for (a = 0; a < length; a++) {
      *uSrc = cvRound(cvSqrt((float)((*dX)*(*dX) + (*dY)*(*dY)) / 32.0f));
      uSrc++;
      dX++;
      dY++;
    }
    ODC2.SetImageData(dst, dst_u_data);
    delete[] dst_u_data;
  }

  delete[] dX_data;
  delete[] dY_data;

  cvReleaseImage(&_dX);
  cvReleaseImage(&_dY);
}

float CMultiLayerBGS::CalVectorsNoisedAngle(float *bg_color, unsigned char *noised_color, float offset, int length) {
  float org_angle = CalVectorsAngle(bg_color, noised_color, length);
  float norm_color = 0, elem, noised_angle;
  int a;
  for (a = 0; a < length; a++) {
    elem = bg_color[a];
    norm_color += elem*elem;
  }
  norm_color = sqrtf(norm_color);
  if (norm_color == 0)
    noised_angle = PI;
  else {
    float sin_angle = offset / norm_color;
    if (sin_angle < m_fMinNoisedAngleSine)
      noised_angle = m_fMinNoisedAngle;
    else
      //noised_angle = ( sin_angle >= 1 ? PI : asinf(sin_angle));
      noised_angle = (sin_angle >= 1 ? PI : sin_angle);
  }

  float angle = org_angle - noised_angle;
  if (angle < 0)
    angle = 0;
  return angle;

  /*
  float org_angle = CalVectorsAngle(bg_color, noised_color, length);
  float max_norm_color, bg_norm_color = 0, noised_norm_color = 0, elem, noised_angle;
  int a;
  for ( a = 0 ; a <  length ; a++ ) {
  elem = bg_color[a];
  bg_norm_color += elem*elem;
  elem = (float)noised_color[a];
  noised_norm_color += elem*elem;
  }
  max_norm_color = MIN(bg_norm_color, noised_norm_color);
  max_norm_color = sqrtf(max_norm_color);
  if ( max_norm_color == 0 )
  noised_angle = PI;
  else {
  float sin_angle = offset/max_norm_color;
  noised_angle = ( sin_angle >= 1 ? PI : asinf(sin_angle));
  }

  float angle = org_angle-noised_angle;
  if ( angle < 0 )
  angle = 0;
  return angle;
  */
}

float CMultiLayerBGS::CalVectorsAngle(float *c1, unsigned char *c2, int length) {
  float angle;
  float dot2, norm1, norm2, elem1, elem2;

  dot2 = norm1 = norm2 = 0;

  int a;
  for (a = 0; a < length; a++) {
    elem1 = (float)(c1[a]);
    elem2 = (float)(c2[a]);
    dot2 += elem1*elem2;
    norm1 += elem1*elem1;
    norm2 += elem2*elem2;
  }

  //angle = (norm1*norm2==0 ? 0 : acosf(dot2/sqrtf(norm1*norm2)));
  //angle = (norm1 * norm2 == 0 ? 0 : sqrtf(fmax(1.0f - dot2 * dot2 / (norm1 * norm2), 0.f)));
  angle = (norm1 * norm2 == 0 ? 0 : sqrtf(std::max(1.0f - dot2 * dot2 / (norm1 * norm2), 0.f)));

  return angle;
}

float CMultiLayerBGS::CalColorRangeDist(unsigned char *cur_intensity, float *bg_intensity, float *max_intensity, float *min_intensity, float shadow_rate, float highlight_rate) {
  float dist = 0.0f, minI, maxI, bgI, curI;
  int channel;
  //float cdist;


  for (channel = 0; channel < m_nChannel; channel++) {
    bgI = bg_intensity[channel];

    /*
    minI = MIN(MIN(min_intensity[channel], bgI*shadow_rate), min_intensity[channel]-15.0f);
    maxI = MAX(MAX(max_intensity[channel], bgI*highlight_rate), max_intensity[channel]+15.0f);
    */

    minI = MIN(min_intensity[channel], bgI * shadow_rate - 5.0f);
    maxI = MAX(max_intensity[channel], bgI * highlight_rate + 5.0f);

    /*
    if ( rand()/((double)RAND_MAX+1) > 0.999 ) {
    char msg[200];
    sprintf(msg, "%d\t%d\n", min_intensity[channel]<bgI*shadow_rate-5.0f ? 1 : 0, max_intensity[channel]>bgI*highlight_rate+5.0f ? 1 : 0);
    ExportLogMessage(msg);
    }
    */

    /*
    minI = max_intensity[channel]*shadow_rate;
    maxI = MAX(bg_intensity[channel]+m_fRobustColorOffset, MIN(max_intensity[channel]*highlight_rate,min_intensity[channel]/shadow_rate));
    */

    curI = (float)(cur_intensity[channel]);

    /*
    //cdist = fabsf(bgI-curI)/512.0f;
    if ( curI > bgI )
    cdist = fabsf(bgI-curI)/256.0f;
    else
    cdist = fabsf(bgI-curI)/512.0f;

    if ( curI > maxI )
    //cdist += (curI-maxI)/(2.0f*(255.0f-maxI))*0.5f;
    cdist += (1.0f-expf(10.0f*(maxI-curI)/MAX(255.0f-maxI,10.0f)))*0.5f;
    else if ( curI < minI )
    //cdist += (minI-curI)/(2.0f*minI)*0.5f;
    cdist += (1.0f-expf(10.0f*(curI-minI)/MAX(minI,10.0f)))*0.5f;
    //dist += cdist;
    if ( cdist > dist )
    dist = cdist;
    */

    if (curI > maxI || curI < minI) {
      dist = 1.0f;
      break;
    }
  }
  //dist = powf(dist, 1.0f/(float)m_nChannel);
  //dist /= (float)m_nChannel;

  return dist;
}

void CMultiLayerBGS::GetLayeredBackgroundImage(int layered_no, IplImage *layered_bg_img, CvScalar empty_color) {
  PixelLBPStruct *PLBP = m_pPixelLBPs;
  LBPStruct* LBPs;
  unsigned short* lbp_idxes;

  int a, b, c;
  int img_length = m_pOrgImg->width * m_pOrgImg->height;

  cvSet(layered_bg_img, empty_color);

  COpencvDataConversion<uchar, uchar> ODC;

  uchar *bg_img_data = ODC.GetImageData(layered_bg_img);
  uchar *_bg_img_data = bg_img_data;
  float *cur_bg_intensity;
  int lbp_num;

  for (a = 0; a < img_length; a++) {
    // get lbp information
    LBPs = (*PLBP).LBPs;
    lbp_idxes = (*PLBP).lbp_idxes;
    lbp_num = (int)((*PLBP).num);
    bool found = false;
    for (b = 0; b < lbp_num; b++) {
      if (LBPs[lbp_idxes[b]].bg_layer_num == layered_no) {
        cur_bg_intensity = LBPs[lbp_idxes[b]].bg_intensity;
        for (c = 0; c < m_pOrgImg->nChannels; c++)
          *_bg_img_data++ = (uchar)* cur_bg_intensity++;
        found = true;
        break;
      }
    }
    if (!found)
      _bg_img_data += m_pOrgImg->nChannels;

    PLBP++;
  }

  ODC.SetImageData(layered_bg_img, bg_img_data);

  delete[] bg_img_data;
}

void CMultiLayerBGS::GetBgLayerNoImage(IplImage *bg_layer_no_img, CvScalar *layer_colors, int layer_num) {
  if (layer_num != 0 && layer_num != m_nMaxLBPModeNum) {
    printf("Must be set in %d layers in function GetBgLayerNoImage!\n", m_nMaxLBPModeNum);
    exit(1);
  }

  CvScalar *bg_layer_colors;
  int bg_layer_color_num = layer_num;
  if (bg_layer_color_num == 0)
    bg_layer_color_num = m_nMaxLBPModeNum;
  bg_layer_colors = new CvScalar[bg_layer_color_num];
  if (layer_colors) {
    for (int l = 0; l < layer_num; l++)
      bg_layer_colors[l] = layer_colors[l];
  }
  else {
    int rgb[3];
    rgb[0] = rgb[1] = rgb[2] = 0;
    int rgb_idx = 0;
    for (int l = 0; l < bg_layer_color_num; l++) {
      bg_layer_colors[l] = CV_RGB(rgb[0], rgb[1], rgb[2]);
      rgb[rgb_idx] += 200;
      rgb[rgb_idx] %= 255;
      rgb_idx++;
      rgb_idx %= 3;
    }
  }

  int img_length = m_pOrgImg->width * m_pOrgImg->height;
  uchar *bg_layer_data = new uchar[img_length * bg_layer_no_img->nChannels];
  uchar *_bg_layer_data = bg_layer_data;

  PixelLBPStruct *PLBP = m_pPixelLBPs;
  unsigned int cur_bg_layer_no;

  for (int a = 0; a < img_length; a++) {
    cur_bg_layer_no = (*PLBP).cur_bg_layer_no;
    for (int b = 0; b < bg_layer_no_img->nChannels; b++) {
      *_bg_layer_data++ = (uchar)(bg_layer_colors[cur_bg_layer_no].val[b]);
    }
    PLBP++;
  }

  COpencvDataConversion<uchar, uchar> ODC;
  ODC.SetImageData(bg_layer_no_img, bg_layer_data);

  delete[] bg_layer_data;
  delete[] bg_layer_colors;
}

void CMultiLayerBGS::GetCurrentLayeredBackgroundImage(int layered_no, IplImage *layered_bg_img, IplImage *layered_fg_img, CvScalar layered_bg_bk_color, CvScalar layered_fg_color,
  int smooth_win, float smooth_sigma, float below_layer_noise, float above_layer_noise, int min_blob_size) {
  PixelLBPStruct *PLBP = m_pPixelLBPs;
  LBPStruct* LBPs;
  unsigned short* lbp_idxes;

  int a;
  int img_length = m_pOrgImg->width * m_pOrgImg->height;

  float *bg_layer_mask = new float[img_length];
  float *_bg_layer_mask = bg_layer_mask;

  for (a = 0; a < img_length; a++) {
    // get lbp information
    LBPs = (*PLBP).LBPs;
    lbp_idxes = (*PLBP).lbp_idxes;
    *_bg_layer_mask++ = (float)(*PLBP).cur_bg_layer_no;
    PLBP++;
  }

  COpencvDataConversion<float, float> ODC;
  IplImage* bg_layer_float_mask_img = cvCreateImage(cvGetSize(m_pOrgImg), IPL_DEPTH_32F, 1);
  IplImage* bg_layer_low_mask_img = cvCreateImage(cvGetSize(m_pOrgImg), IPL_DEPTH_8U, 1);
  IplImage* bg_layer_high_mask_img = cvCreateImage(cvGetSize(m_pOrgImg), IPL_DEPTH_8U, 1);
  IplImage* bg_layer_mask_img = cvCreateImage(cvGetSize(m_pOrgImg), IPL_DEPTH_8U, 1);

  ODC.SetImageData(bg_layer_float_mask_img, bg_layer_mask);

  /* method 1 using smooth */
  /*
  cvSmooth(bg_layer_float_mask_img, bg_layer_float_mask_img, CV_GAUSSIAN, smooth_win, smooth_win, smooth_sigma);

  cvThreshold(bg_layer_float_mask_img, bg_layer_low_mask_img, (float)layered_no-below_layer_noise, 1, CV_THRESH_BINARY);
  cvThreshold(bg_layer_float_mask_img, bg_layer_high_mask_img, (float)layered_no+above_layer_noise, 1, CV_THRESH_BINARY_INV);

  cvAnd(bg_layer_low_mask_img, bg_layer_high_mask_img, bg_layer_mask_img);
  */

  /* method 2 using dilate, erode, blob removing */
  cvSmooth(bg_layer_float_mask_img, bg_layer_float_mask_img, CV_GAUSSIAN, smooth_win, smooth_win, smooth_sigma);

  cvThreshold(bg_layer_float_mask_img, bg_layer_low_mask_img, (float)layered_no - below_layer_noise, 1, CV_THRESH_BINARY);
  cvThreshold(bg_layer_float_mask_img, bg_layer_high_mask_img, (float)layered_no + above_layer_noise, 1, CV_THRESH_BINARY_INV);
  cvAnd(bg_layer_low_mask_img, bg_layer_high_mask_img, bg_layer_mask_img);

  cvDilate(bg_layer_mask_img, bg_layer_mask_img, 0, 2);
  cvErode(bg_layer_mask_img, bg_layer_mask_img, 0, 2);

  //cvMorphologyEx(bg_layer_mask_img, bg_layer_mask_img, NULL, 0, CV_MOP_OPEN|CV_MOP_CLOSE, 1);

  // Extract the blobs using a threshold of 100 in the image
  CBlobResult blobs = CBlobResult(bg_layer_mask_img, NULL, 0, true);
  // discard the blobs with less area than 100 pixels
  // ( the criteria to filter can be any class derived from COperadorBlob )
  blobs.Filter(blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, min_blob_size);

  CBlob filtered_blob;
  cvSetZero(bg_layer_mask_img);
  for (a = 0; a < blobs.GetNumBlobs(); a++) {
    filtered_blob = blobs.GetBlob(a);
    filtered_blob.FillBlob(bg_layer_mask_img, cvScalar(1));
  }
  blobs.GetNthBlob(CBlobGetArea(), 0, filtered_blob);
  filtered_blob.FillBlob(bg_layer_mask_img, cvScalar(0));


  cvSet(layered_bg_img, layered_bg_bk_color);
  cvCopy(m_pBgImg, layered_bg_img, bg_layer_mask_img);

  if (layered_fg_img) {
    cvCopy(m_pOrgImg, layered_fg_img);
    cvSet(layered_fg_img, layered_fg_color, bg_layer_mask_img);
  }

  cvReleaseImage(&bg_layer_float_mask_img);
  cvReleaseImage(&bg_layer_low_mask_img);
  cvReleaseImage(&bg_layer_high_mask_img);
  cvReleaseImage(&bg_layer_mask_img);
  delete[] bg_layer_mask;
}

void CMultiLayerBGS::GetColoredBgMultiLayeredImage(IplImage *bg_multi_layer_img, CvScalar *layer_colors) {
  cvCopyImage(m_pOrgImg, bg_multi_layer_img);

  COpencvDataConversion<uchar, uchar> ODC;

  uchar *bg_ml_imgD = ODC.GetImageData(bg_multi_layer_img);
  uchar *fg_maskD = ODC.GetImageData(m_pFgMaskImg);

  uchar *_bg_ml_imgD = bg_ml_imgD;
  uchar *_fg_maskD = fg_maskD;

  PixelLBPStruct *PLBP = m_pPixelLBPs;
  LBPStruct* LBPs;
  unsigned short* lbp_idxes;
  unsigned int lbp_num;
  int bg_layer_num;

  int a, c;
  int img_length = m_pOrgImg->width * m_pOrgImg->height;
  int channels = m_pOrgImg->nChannels;
  bool bLayeredBg;

  for (a = 0; a < img_length; a++) {
    // get lbp information
    lbp_num = (*PLBP).num;
    LBPs = (*PLBP).LBPs;
    lbp_idxes = (*PLBP).lbp_idxes;
    bLayeredBg = false;

    if ((*_fg_maskD == 0)) {
      bg_layer_num = LBPs[lbp_idxes[0]].bg_layer_num;
      int first_layer_idx = 0;
      for (c = 0; c < (int)lbp_num; c++) {
        if (LBPs[lbp_idxes[c]].bg_layer_num == 1) {
          first_layer_idx = c;
          break;
        }
      }
      if (bg_layer_num > 1 && DistLBP(&(LBPs[lbp_idxes[0]]), &(LBPs[first_layer_idx])) > 0.1f) {
        for (c = 0; c < channels; c++)
          *_bg_ml_imgD++ = (uchar)(layer_colors[bg_layer_num].val[c]);
        bLayeredBg = true;
      }

      if (!bLayeredBg)
        _bg_ml_imgD += channels;
    }
    else {
      _bg_ml_imgD += channels;
    }

    PLBP++;
    _fg_maskD++;
  }

  ODC.SetImageData(bg_multi_layer_img, bg_ml_imgD);

  delete[] fg_maskD;
  delete[] bg_ml_imgD;
}

void CMultiLayerBGS::GetForegroundProbabilityImage(IplImage *fg_dist_img) {
  COpencvDataConversion<float, float> ODC1;
  COpencvDataConversion<uchar, uchar> ODC2;

  float *_fg_distD = ODC1.GetImageData(m_pBgDistImg);
  uchar *_fg_progI = ODC2.GetImageData(fg_dist_img);
  float *fg_distD = _fg_distD;
  uchar *fg_progI = _fg_progI;

  /*
  float *fg_distD = (float*)(m_pBgDistImg->imageData);
  uchar *fg_progI = (uchar*)(fg_dist_img->imageData);
  */

  int channels = fg_dist_img->nChannels;

  int a, b;
  int img_length = fg_dist_img->width * fg_dist_img->height;
  uchar temp;
  for (a = 0; a < img_length; a++) {
    temp = cvRound(255.0f * ((*fg_distD++)));
    for (b = 0; b < channels; b++)
      *fg_progI++ = temp;
  }

  ODC2.SetImageData(fg_dist_img, _fg_progI);

  delete[] _fg_distD;
  delete[] _fg_progI;
}

void CMultiLayerBGS::RemoveBackgroundLayers(PixelLBPStruct *PLBP, bool *removed_modes) {
  int a, b;
  int lbp_num = PLBP->num;

  /*
  if ( lbp_num < m_nMaxLBPModeNum )
  return;
  */

  /* testing */

  unsigned short* lbp_idxes = PLBP->lbp_idxes;
  if (!removed_modes) {
    int removed_bg_layer_num = 0;
    for (a = 0; a < lbp_num; a++) {
      if (PLBP->LBPs[lbp_idxes[a]].bg_layer_num && PLBP->LBPs[lbp_idxes[a]].weight < m_fMinBgLayerWeight) { // should be removed
        removed_bg_layer_num = PLBP->LBPs[lbp_idxes[a]].bg_layer_num;
        lbp_num--;
        for (b = a; b < lbp_num; b++)
          lbp_idxes[b] = lbp_idxes[b + 1];
        break;
      }
    }
    if (removed_bg_layer_num) {
      for (a = 0; a < lbp_num; a++) {
        if (PLBP->LBPs[lbp_idxes[a]].bg_layer_num > removed_bg_layer_num)
          PLBP->LBPs[lbp_idxes[a]].bg_layer_num--;
      }
    }
  }
  else {
    int removed_bg_layer_nums[10];
    int removed_layer_num = 0;
    for (a = 0; a < lbp_num; a++) {
      if (removed_modes[a] && PLBP->LBPs[lbp_idxes[a]].bg_layer_num) { // should be removed
        removed_bg_layer_nums[removed_layer_num++] = PLBP->LBPs[lbp_idxes[a]].bg_layer_num;
      }
    }

    for (a = 0; a < lbp_num; a++) {
      if (removed_modes[a]) { // should be removed
        lbp_num--;
        for (b = a; b < lbp_num; b++)
          lbp_idxes[b] = lbp_idxes[b + 1];
      }
    }

    for (a = 0; a < lbp_num; a++) {
      for (b = 0; b < removed_layer_num; b++) {
        if (PLBP->LBPs[lbp_idxes[a]].bg_layer_num > removed_bg_layer_nums[b])
          PLBP->LBPs[lbp_idxes[a]].bg_layer_num--;
      }
    }
  }

  // sort the list of modes based on the weights of modes
  if (lbp_num != (int)PLBP->num) {
    float weights[100], tot_weights = 0;
    for (a = 0; a < (int)lbp_num; a++) {
      weights[a] = PLBP->LBPs[lbp_idxes[a]].weight;
      tot_weights += weights[a];
    }

    // sort weights in the descent order
    QuickSort(weights, lbp_idxes, 0, (int)lbp_num - 1, false);

    // calculate the first potential background modes number, bg_num
    float threshold_weight = m_fBackgroundModelPercent*tot_weights;
    int bg_num = 0;
    tot_weights = 0;
    for (a = 0; a < (int)lbp_num; a++) {
      tot_weights += PLBP->LBPs[lbp_idxes[a]].weight;
      if (tot_weights > threshold_weight) {
        bg_num = a + 1;
        break;
      }
    }
    (*PLBP).bg_num = bg_num;

  }

  PLBP->num = lbp_num;

  float bg_layer_data[10];
  unsigned short bg_layer_idxes[10];
  int bg_layer_num;
  int tot_bg_layer_num = 0;
  for (a = 0; a < lbp_num; a++) {
    bg_layer_num = PLBP->LBPs[lbp_idxes[a]].bg_layer_num;
    if (bg_layer_num) {
      bg_layer_data[tot_bg_layer_num] = (float)bg_layer_num;
      bg_layer_idxes[tot_bg_layer_num++] = lbp_idxes[a];
    }
  }
  if (tot_bg_layer_num == 1) {
    PLBP->LBPs[bg_layer_idxes[0]].bg_layer_num = 1;
  }
  else if (tot_bg_layer_num) {
    // sort weights in the descent order
    QuickSort(bg_layer_data, bg_layer_idxes, 0, tot_bg_layer_num - 1, true);
    for (a = 0; a < tot_bg_layer_num; a++)
      PLBP->LBPs[bg_layer_idxes[a]].bg_layer_num = a + 1;
  }

  /*
  int max_bg_layer_num = 0;
  for ( a = 0 ; a < lbp_num ; a++ )
  max_bg_layer_num = MAX(max_bg_layer_num, PLBP->LBPs[lbp_idxes[a]].bg_layer_num);
  if ( max_bg_layer_num >= 2 ) {
  bool find_first_layer = false;
  for ( a = 0 ; a < lbp_num ; a++ ) {
  max_bg_layer_num = MAX(max_bg_layer_num, PLBP->LBPs[lbp_idxes[a]].bg_layer_num);
  if ( PLBP->LBPs[lbp_idxes[a]].bg_layer_num == 1 ) {
  find_first_layer = true;
  break;
  }
  }
  if ( !find_first_layer ) {
  printf("\n===============================================\n");
  printf(" have second layer, no first layer \n");
  printf("\n===============================================\n");
  exit(1);
  }
  }
  */
}

void CMultiLayerBGS::Postprocessing() {
  // post-processing for background subtraction results
  cvDilate(m_pFgMaskImg, m_pFgMaskImg, 0, 2);
  cvErode(m_pFgMaskImg, m_pFgMaskImg, 0, 2);

  /** Example of extracting and filtering the blobs of an image */

  // object that will contain blobs of inputImage
  CBlobResult blobs;

  IplImage *inputImage = m_pFgMaskImg;

  // Extract the blobs using a threshold of 100 in the image
  blobs = CBlobResult(inputImage, NULL, 0, true);

  // create a file with some of the extracted features
  //blobs.PrintBlobs( ".\\blobs.txt" );

  // discard the blobs with less area than 100 pixels
  // ( the criteria to filter can be any class derived from COperadorBlob )
  blobs.Filter(blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, 100);

  // create a file with filtered results
  //blobs.PrintBlobs( ".\\filteredBlobs.txt" );

  // build an output image equal to the input but with 3 channels (to draw the coloured blobs)
  IplImage *outputImage;
  outputImage = cvCreateImage(cvSize(inputImage->width, inputImage->height), IPL_DEPTH_8U, 1);
  cvSet(outputImage, cvScalar(0));

  // plot the selected blobs in a output image
  CBlob filtered_blob;
  //cvSet(outputImage, CV_RGB(0,0,255));
  int a;
  for (a = 0; a < blobs.GetNumBlobs(); a++) {
    filtered_blob = blobs.GetBlob(a);
    filtered_blob.FillBlob(outputImage, cvScalar(255));
  }
  blobs.GetNthBlob(CBlobGetArea(), 0, filtered_blob);
  filtered_blob.FillBlob(outputImage, cvScalar(0));

  /*
  char *win_name = "blob filtered image";
  cvNamedWindow(win_name);
  cvShowImage(win_name, outputImage);
  cvWaitKey(3);
  */

  cvReleaseImage(&outputImage);
}

void CMultiLayerBGS::GetFloatEdgeImage(IplImage *src, IplImage *dst) {
  if (src->nChannels > 1) {
    printf("Error: the input source image must be single-channel image!\n");
    exit(1);
  }
  if (dst->depth != IPL_DEPTH_32F) {
    printf("Error: the output edge image must be float image ranging in [0,1]!\n");
    exit(1);
  }

  uchar *src_x_data;
  float *dst_x_data;

  int x, y;
  for (y = 0; y < dst->height; y++) {
    src_x_data = (uchar*)(src->imageData + src->widthStep * y);
    dst_x_data = (float*)(dst->imageData + dst->widthStep * y);
    for (x = 0; x < dst->width; x++) {
      *dst_x_data++ = (float)(*src_x_data++) / 255.0f;
    }
  }
}

void CMultiLayerBGS::ExportLogMessage(char *msg) {
  const char *log_fn = "log_message.txt";
  ofstream fout(log_fn, ios::app);
  if (fout.fail()) {
    printf("Error opening log output file %s.\n", log_fn);
    fout.close();
    exit(0);
  }

  fout << msg;
  fout.close();
}

void CMultiLayerBGS::UpdatePatternColorDistWeights(float *cur_pattern, float *bg_pattern) {
  return;

  int cur_true_num = 0, cur_false_num = 0, bg_true_num = 0, bg_false_num = 0;
  int a;

  for (a = 0; a < m_nLBPLength; a++) {
    cur_true_num += (cur_pattern[a] > 0.5f ? 1 : 0);
    cur_false_num += (cur_pattern[a] < 0.5f ? 0 : 1);
    bg_true_num += (bg_pattern[a] > 0.5f);
    bg_false_num += (bg_pattern[a] < 0.5f);
  }
  m_fTextureWeight = expf(-(fabsf(cur_true_num - cur_false_num) + fabsf(bg_true_num - bg_false_num) + 0.8f) / (float)m_nLBPLength);
  m_fTextureWeight = MAX(MIN(m_fTextureWeight, 0.5f), 0.1f);
  m_fColorWeight = 1.0f - m_fTextureWeight;
}

void CMultiLayerBGS::Save(const char *bg_model_fn) {
  Save(bg_model_fn, 2);
}

void CMultiLayerBGS::Save(const char *bg_model_fn, int save_type) {
  FILE * fout = fopen(bg_model_fn, "w");
  if (!fout) {
    printf("Error opening background model output file %s.\n", bg_model_fn);
    fclose(fout);
    //exit(0);
    return;
  }

  int i, j;
  if (save_type == 0) { /* save the background model information */
    fprintf(fout, "FILE_TYPE:  MODEL_INFO\n\n");

    fprintf(fout, "MAX_MODEL_NUM: %5d\n", m_nMaxLBPModeNum);
    fprintf(fout, "LBP_LENGTH: %5d\n", m_nLBPLength);
    fprintf(fout, "CHANNELS_NUM: %5d\n", m_nChannel);
    fprintf(fout, "IMAGE_SIZE: %5d %5d\n\n", m_cvImgSize.width, m_cvImgSize.height);

    fprintf(fout, "MODEL_INFO_PIXEL_BY_PIXEL:\n");

    int img_length = m_cvImgSize.height * m_cvImgSize.width;
    PixelLBPStruct* PLBP = m_pPixelLBPs;

    for (int yx = 0; yx < img_length; yx++) {
      fprintf(fout, "%3d %3d %3d", (*PLBP).num, (*PLBP).bg_num, (*PLBP).cur_bg_layer_no);
      for (i = 0; i < (int)(*PLBP).num; i++)
        fprintf(fout, " %3d", (*PLBP).lbp_idxes[i]);
      for (i = 0; i < (int)(*PLBP).num; i++) {
        int li = (*PLBP).lbp_idxes[i];
        for (j = 0; j < m_nChannel; j++) {
          fprintf(fout, " %7.1f %7.1f %7.1f", (*PLBP).LBPs[li].bg_intensity[j],
            (*PLBP).LBPs[li].max_intensity[j], (*PLBP).LBPs[li].min_intensity[j]);
        }
        for (j = 0; j < m_nLBPLength; j++)
          fprintf(fout, " %7.3f", (*PLBP).LBPs[li].bg_pattern[j]);
        fprintf(fout, " %10.5f", (*PLBP).LBPs[li].weight);
        fprintf(fout, " %10.5f", (*PLBP).LBPs[li].max_weight);
        fprintf(fout, " %3d", (*PLBP).LBPs[li].bg_layer_num);
        fprintf(fout, " %20lu", (*PLBP).LBPs[li].first_time);
        fprintf(fout, " %20lu", (*PLBP).LBPs[li].last_time);
        fprintf(fout, " %8d", (*PLBP).LBPs[li].freq);
      }
      fprintf(fout, "\n");
      PLBP++;
    }
  }
  else if (save_type == 1) { /* save current parameters for background subtraction */
    fprintf(fout, "FILE_TYPE:  MODEL_PARAS\n\n");

    fprintf(fout, "MAX_MODEL_NUM: %5d\n", m_nMaxLBPModeNum);
    fprintf(fout, "FRAME_DURATION: %f\n", m_fFrameDuration);
    fprintf(fout, "MODEL_UPDATING_LEARN_RATE: %f\n", m_fModeUpdatingLearnRate);
    fprintf(fout, "WEIGHT_UPDATING_LEARN_RATE: %f\n", m_fWeightUpdatingLearnRate);
    fprintf(fout, "WEIGHT_UPDATING_CONSTANT: %f\n", m_fWeightUpdatingConstant);
    fprintf(fout, "LOW_INITIAL_MODE_WEIGHT: %f\n", m_fLowInitialModeWeight);
    fprintf(fout, "RELIABLE_BACKGROUND_MODE_WEIGHT: %f\n", m_fReliableBackgroundModeWeight);
    fprintf(fout, "ROBUST_COLOR_OFFSET: %f\n", m_fRobustColorOffset);
    fprintf(fout, "BACKGROUND_MODEL_PERCENT: %f\n", m_fBackgroundModelPercent);
    fprintf(fout, "ROBUST_SHADOW_RATE: %f\n", m_fRobustShadowRate);
    fprintf(fout, "ROBUST_HIGHLIGHT_RATE: %f\n", m_fRobustHighlightRate);
    fprintf(fout, "PATTERN_COLOR_DIST_BACKGROUND_THRESHOLD: %f\n", m_fPatternColorDistBgThreshold);
    fprintf(fout, "PATTERN_COLOR_DIST_BACKGROUND_UPDATED_THRESHOLD: %f\n", m_fPatternColorDistBgUpdatedThreshold);
    fprintf(fout, "MIN_BACKGROUND_LAYER_WEIGHT: %f\n", m_fMinBgLayerWeight);
    fprintf(fout, "PATTERN_DIST_SMOOTH_NEIG_HALF_SIZE: %d\n", m_nPatternDistSmoothNeigHalfSize);
    fprintf(fout, "PATTERN_DIST_CONV_GAUSSIAN_SIGMA: %f\n", m_fPatternDistConvGaussianSigma);
    fprintf(fout, "TEXTURE_WEIGHT: %f\n", m_fTextureWeight);
    fprintf(fout, "MIN_NOISED_ANGLE: %f\n", m_fMinNoisedAngle);
    fprintf(fout, "MIN_NOISED_ANGLE_SINE: %f\n", m_fMinNoisedAngleSine);
    fprintf(fout, "BILATERAL_SIGMA_S: %f\n", m_fSigmaS);
    fprintf(fout, "BILATERAL_SIGMA_R: %f\n", m_fSigmaR);
    fprintf(fout, "LBP_LENGTH: %5d\n", m_nLBPLength);
    fprintf(fout, "LBP_LEVEL_NUM: %5d\n", m_nLBPLevelNum);
    fprintf(fout, "LBP_RADIUSES: ");
    for (i = 0; i < m_nLBPLevelNum; i++)
      fprintf(fout, "%10.5f", m_pLBPRadiuses[i]);
    fprintf(fout, "\nLBP_NEIG_POINT_NUMS: ");
    for (i = 0; i < m_nLBPLevelNum; i++)
      fprintf(fout, "%6d", m_pLBPMeigPointNums[i]);
  }
  else if (save_type == 2) { /* save the background model information and parameters */
    fprintf(fout, "FILE_TYPE:  MODEL_PARAS_INFO\n\n");

    fprintf(fout, "MAX_MODEL_NUM: %5d\n", m_nMaxLBPModeNum);
    fprintf(fout, "FRAME_DURATION: %f\n", m_fFrameDuration);
    fprintf(fout, "MODEL_UPDATING_LEARN_RATE: %f\n", m_fModeUpdatingLearnRate);
    fprintf(fout, "WEIGHT_UPDATING_LEARN_RATE: %f\n", m_fWeightUpdatingLearnRate);
    fprintf(fout, "WEIGHT_UPDATING_CONSTANT: %f\n", m_fWeightUpdatingConstant);
    fprintf(fout, "LOW_INITIAL_MODE_WEIGHT: %f\n", m_fLowInitialModeWeight);
    fprintf(fout, "RELIABLE_BACKGROUND_MODE_WEIGHT: %f\n", m_fReliableBackgroundModeWeight);
    fprintf(fout, "ROBUST_COLOR_OFFSET: %f\n", m_fRobustColorOffset);
    fprintf(fout, "BACKGROUND_MODEL_PERCENT: %f\n", m_fBackgroundModelPercent);
    fprintf(fout, "ROBUST_SHADOW_RATE: %f\n", m_fRobustShadowRate);
    fprintf(fout, "ROBUST_HIGHLIGHT_RATE: %f\n", m_fRobustHighlightRate);
    fprintf(fout, "PATTERN_COLOR_DIST_BACKGROUND_THRESHOLD: %f\n", m_fPatternColorDistBgThreshold);
    fprintf(fout, "PATTERN_COLOR_DIST_BACKGROUND_UPDATED_THRESHOLD: %f\n", m_fPatternColorDistBgUpdatedThreshold);
    fprintf(fout, "MIN_BACKGROUND_LAYER_WEIGHT: %f\n", m_fMinBgLayerWeight);
    fprintf(fout, "PATTERN_DIST_SMOOTH_NEIG_HALF_SIZE: %d\n", m_nPatternDistSmoothNeigHalfSize);
    fprintf(fout, "PATTERN_DIST_CONV_GAUSSIAN_SIGMA: %f\n", m_fPatternDistConvGaussianSigma);
    fprintf(fout, "TEXTURE_WEIGHT: %f\n", m_fTextureWeight);
    fprintf(fout, "MIN_NOISED_ANGLE: %f\n", m_fMinNoisedAngle);
    fprintf(fout, "MIN_NOISED_ANGLE_SINE: %f\n", m_fMinNoisedAngleSine);
    fprintf(fout, "BILATERAL_SIGMA_S: %f\n", m_fSigmaS);
    fprintf(fout, "BILATERAL_SIGMA_R: %f\n", m_fSigmaR);
    fprintf(fout, "LBP_LENGTH: %5d\n", m_nLBPLength);
    fprintf(fout, "LBP_LEVEL_NUM: %5d\n", m_nLBPLevelNum);
    fprintf(fout, "LBP_RADIUSES: ");
    for (i = 0; i < m_nLBPLevelNum; i++)
      fprintf(fout, "%10.5f", m_pLBPRadiuses[i]);
    fprintf(fout, "\nLBP_NEIG_POINT_NUMS: ");
    for (i = 0; i < m_nLBPLevelNum; i++)
      fprintf(fout, "%6d", m_pLBPMeigPointNums[i]);

    fprintf(fout, "\nMAX_MODEL_NUM: %5d\n", m_nMaxLBPModeNum);
    fprintf(fout, "LBP_LENGTH: %5d\n", m_nLBPLength);
    fprintf(fout, "CHANNELS_NUM: %5d\n", m_nChannel);
    fprintf(fout, "IMAGE_SIZE: %5d %5d\n\n", m_cvImgSize.width, m_cvImgSize.height);

    fprintf(fout, "MODEL_INFO_PIXEL_BY_PIXEL:\n");

    int img_length = m_cvImgSize.height * m_cvImgSize.width;
    PixelLBPStruct* PLBP = m_pPixelLBPs;

    for (int yx = 0; yx < img_length; yx++) {
      fprintf(fout, "%3d %3d %3d", (*PLBP).num, (*PLBP).bg_num, (*PLBP).cur_bg_layer_no);
      for (i = 0; i < (int)(*PLBP).num; i++)
        fprintf(fout, " %3d", (*PLBP).lbp_idxes[i]);
      for (i = 0; i < (int)(*PLBP).num; i++) {
        int li = (*PLBP).lbp_idxes[i];
        for (j = 0; j < m_nChannel; j++) {
          fprintf(fout, " %7.1f %7.1f %7.1f", (*PLBP).LBPs[li].bg_intensity[j],
            (*PLBP).LBPs[li].max_intensity[j], (*PLBP).LBPs[li].min_intensity[j]);
        }
        for (j = 0; j < m_nLBPLength; j++)
          fprintf(fout, " %7.3f", (*PLBP).LBPs[li].bg_pattern[j]);
        fprintf(fout, " %10.5f", (*PLBP).LBPs[li].weight);
        fprintf(fout, " %10.5f", (*PLBP).LBPs[li].max_weight);
        fprintf(fout, " %3d", (*PLBP).LBPs[li].bg_layer_num);
        fprintf(fout, " %20lu", (*PLBP).LBPs[li].first_time);
        fprintf(fout, " %20lu", (*PLBP).LBPs[li].last_time);
        fprintf(fout, " %8d", (*PLBP).LBPs[li].freq);
      }
      fprintf(fout, "\n");
      PLBP++;
    }
  }
  else { /* wrong save type */
    printf("Please input correct save type: 0 - model_info  1 - model_paras  2 - model_paras_info\n");
    fclose(fout);
    exit(0);
  }

  fclose(fout);
}

bool CMultiLayerBGS::Load(const char *bg_model_fn) {
  ifstream fin(bg_model_fn, ios::in);
  if (fin.fail()) {
    printf("Error opening background model file %s.\n", bg_model_fn);
    fin.close();
    return false;
  }

  char para_name[1024], model_type[1024];

  fin >> para_name >> model_type;

  int i, j;
  CvSize img_size;
  int img_length = m_cvImgSize.width * m_cvImgSize.height;
  int max_lbp_mode_num = m_nMaxLBPModeNum;

  if (!strcmp(model_type, "MODEL_INFO")) {
    fin >> para_name >> m_nMaxLBPModeNum;
    fin >> para_name >> m_nLBPLength;
    fin >> para_name >> m_nChannel;
    fin >> para_name >> img_size.width >> img_size.height;

    if (m_cvImgSize.width != img_size.width || m_cvImgSize.height != img_size.height) {
      printf("Image size is not matched!\n");
      return false;
    }

    if (max_lbp_mode_num != m_nMaxLBPModeNum) {
      PixelLBPStruct* PLBP = m_pPixelLBPs;
      for (int yx = 0; yx < img_length; yx++) {
        delete[](*PLBP).LBPs;
        delete[](*PLBP).lbp_idxes;
        (*PLBP).LBPs = new LBPStruct[m_nMaxLBPModeNum];
        (*PLBP).lbp_idxes = new unsigned short[m_nMaxLBPModeNum];
      }
    }

    fin >> para_name;

    int img_length = m_cvImgSize.height * m_cvImgSize.width;
    PixelLBPStruct* PLBP = m_pPixelLBPs;

    for (int yx = 0; yx < img_length; yx++) {
      fin >> (*PLBP).num >> (*PLBP).bg_num >> (*PLBP).cur_bg_layer_no;
      for (i = 0; i < (int)(*PLBP).num; i++)
        fin >> (*PLBP).lbp_idxes[i];
      for (i = 0; i < (int)(*PLBP).num; i++) {
        int li = (*PLBP).lbp_idxes[i];
        for (j = 0; j < m_nChannel; j++) {
          fin >> (*PLBP).LBPs[li].bg_intensity[j] >>
            (*PLBP).LBPs[li].max_intensity[j] >> (*PLBP).LBPs[li].min_intensity[j];
        }
        for (j = 0; j < m_nLBPLength; j++)
          fin >> (*PLBP).LBPs[li].bg_pattern[j];
        fin >> (*PLBP).LBPs[li].weight >> (*PLBP).LBPs[li].max_weight >> (*PLBP).LBPs[li].bg_layer_num
          >> (*PLBP).LBPs[li].first_time >> (*PLBP).LBPs[li].last_time >> (*PLBP).LBPs[li].freq;
      }
      PLBP++;
    }
  }
  else if (!strcmp(model_type, "MODEL_PARAS")) {
    fin >> para_name >> m_nMaxLBPModeNum;
    fin >> para_name >> m_fFrameDuration;
    fin >> para_name >> m_fModeUpdatingLearnRate;
    fin >> para_name >> m_fWeightUpdatingLearnRate;
    fin >> para_name >> m_fWeightUpdatingConstant;
    fin >> para_name >> m_fLowInitialModeWeight;
    fin >> para_name >> m_fReliableBackgroundModeWeight;
    fin >> para_name >> m_fRobustColorOffset;
    fin >> para_name >> m_fBackgroundModelPercent;
    fin >> para_name >> m_fRobustShadowRate;
    fin >> para_name >> m_fRobustHighlightRate;
    fin >> para_name >> m_fPatternColorDistBgThreshold;
    fin >> para_name >> m_fPatternColorDistBgUpdatedThreshold;
    fin >> para_name >> m_fMinBgLayerWeight;
    fin >> para_name >> m_nPatternDistSmoothNeigHalfSize;
    fin >> para_name >> m_fPatternDistConvGaussianSigma;
    fin >> para_name >> m_fTextureWeight;
    fin >> para_name >> m_fMinNoisedAngle;
    fin >> para_name >> m_fMinNoisedAngleSine;
    fin >> para_name >> m_fSigmaS;
    fin >> para_name >> m_fSigmaR;
    fin >> para_name >> m_nLBPLength;
    fin >> para_name >> m_nLBPLevelNum;
    fin >> para_name;
    for (i = 0; i < m_nLBPLevelNum; i++)
      fin >> m_pLBPRadiuses[i];
    fin >> para_name;
    for (i = 0; i < m_nLBPLevelNum; i++)
      fin >> m_pLBPMeigPointNums[i];
  }
  else if (!strcmp(model_type, "MODEL_PARAS_INFO")) {
    fin >> para_name >> m_nMaxLBPModeNum;
    fin >> para_name >> m_fFrameDuration;
    fin >> para_name >> m_fModeUpdatingLearnRate;
    fin >> para_name >> m_fWeightUpdatingLearnRate;
    fin >> para_name >> m_fWeightUpdatingConstant;
    fin >> para_name >> m_fLowInitialModeWeight;
    fin >> para_name >> m_fReliableBackgroundModeWeight;
    fin >> para_name >> m_fRobustColorOffset;
    fin >> para_name >> m_fBackgroundModelPercent;
    fin >> para_name >> m_fRobustShadowRate;
    fin >> para_name >> m_fRobustHighlightRate;
    fin >> para_name >> m_fPatternColorDistBgThreshold;
    fin >> para_name >> m_fPatternColorDistBgUpdatedThreshold;
    fin >> para_name >> m_fMinBgLayerWeight;
    fin >> para_name >> m_nPatternDistSmoothNeigHalfSize;
    fin >> para_name >> m_fPatternDistConvGaussianSigma;
    fin >> para_name >> m_fTextureWeight;
    fin >> para_name >> m_fMinNoisedAngle;
    fin >> para_name >> m_fMinNoisedAngleSine;
    fin >> para_name >> m_fSigmaS;
    fin >> para_name >> m_fSigmaR;
    fin >> para_name >> m_nLBPLength;
    fin >> para_name >> m_nLBPLevelNum;
    fin >> para_name;
    for (i = 0; i < m_nLBPLevelNum; i++)
      fin >> m_pLBPRadiuses[i];
    fin >> para_name;
    for (i = 0; i < m_nLBPLevelNum; i++)
      fin >> m_pLBPMeigPointNums[i];

    fin >> para_name >> m_nMaxLBPModeNum;
    fin >> para_name >> m_nLBPLength;
    fin >> para_name >> m_nChannel;
    fin >> para_name >> img_size.width >> img_size.height;

    if (m_cvImgSize.width != img_size.width || m_cvImgSize.height != img_size.height) {
      printf("Image size is not matched!\n");
      return false;
    }

    if (max_lbp_mode_num != m_nMaxLBPModeNum) {
      PixelLBPStruct* PLBP = m_pPixelLBPs;
      for (int yx = 0; yx < img_length; yx++) {
        delete[](*PLBP).LBPs;
        delete[](*PLBP).lbp_idxes;
        (*PLBP).LBPs = new LBPStruct[m_nMaxLBPModeNum];
        (*PLBP).lbp_idxes = new unsigned short[m_nMaxLBPModeNum];
      }
    }

    fin >> para_name;

    int img_length = m_cvImgSize.height * m_cvImgSize.width;
    PixelLBPStruct* PLBP = m_pPixelLBPs;

    for (int yx = 0; yx < img_length; yx++) {
      fin >> (*PLBP).num >> (*PLBP).bg_num >> (*PLBP).cur_bg_layer_no;
      for (i = 0; i < (int)(*PLBP).num; i++)
        fin >> (*PLBP).lbp_idxes[i];
      for (i = 0; i < (int)(*PLBP).num; i++) {
        int li = (*PLBP).lbp_idxes[i];
        for (j = 0; j < m_nChannel; j++) {
          fin >> (*PLBP).LBPs[li].bg_intensity[j] >>
            (*PLBP).LBPs[li].max_intensity[j] >> (*PLBP).LBPs[li].min_intensity[j];
        }
        for (j = 0; j < m_nLBPLength; j++)
          fin >> (*PLBP).LBPs[li].bg_pattern[j];
        fin >> (*PLBP).LBPs[li].weight >> (*PLBP).LBPs[li].max_weight >> (*PLBP).LBPs[li].bg_layer_num
          >> (*PLBP).LBPs[li].first_time >> (*PLBP).LBPs[li].last_time >> (*PLBP).LBPs[li].freq;
      }
      PLBP++;
    }
  }
  else {
    printf("Not correct model save type!\n");
    fin.close();
    exit(0);
  }

  fin.close();

  ResetAllParameters();

  return true;
}

void CMultiLayerBGS::SetValidPointMask(IplImage *maskImage, int mode) {
  if (mode == 1)
    SetBkMaskImage(maskImage);
  else
    cvAnd(m_pBkMaskImg, maskImage, m_pBkMaskImg);
}

void CMultiLayerBGS::SetFrameRate(float frameDuration) {
  m_fModeUpdatingLearnRate = m_fModeUpdatingLearnRatePerSecond*frameDuration;
  m_fWeightUpdatingLearnRate = m_fWeightUpdatingLearnRatePerSecond*frameDuration;

  m_fFrameDuration = frameDuration;

  m_f1_ModeUpdatingLearnRate = 1.0f - m_fModeUpdatingLearnRate;
  m_f1_WeightUpdatingLearnRate = 1.0f - m_fWeightUpdatingLearnRate;
}

void CMultiLayerBGS::Init(int width, int height) {
  IplImage* first_img = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
  int lbp_level_num = 1;
  float radiuses[] = { 2.0f };
  int neig_pt_nums[] = { 6 };
  Initialization(first_img, lbp_level_num, radiuses, neig_pt_nums);
  cvReleaseImage(&first_img);
}

int CMultiLayerBGS::SetRGBInputImage(IplImage *inputImage, CvRect *roi) {
  if (!inputImage) {
    printf("Please allocate the IplImage memory!\n");
    return 0;
  }
  if (inputImage->width != m_cvImgSize.width ||
    inputImage->height != m_cvImgSize.height ||
    inputImage->depth != IPL_DEPTH_8U ||
    inputImage->nChannels != 3) {
    printf("Please provide the correct IplImage pointer, \ne.g. inputImage = cvCreateImage(imgSize, IPL_DEPTH_8U, 3);\n");
    return 0;
  }
  SetNewImage(inputImage, roi);
  return 1;
}

void CMultiLayerBGS::SetParameters(int max_lbp_mode_num, float mode_updating_learn_rate_per_second, float weight_updating_learn_rate_per_second, float low_init_mode_weight) {
  m_nMaxLBPModeNum = max_lbp_mode_num;
  m_fModeUpdatingLearnRate = mode_updating_learn_rate_per_second*m_fFrameDuration;
  m_fWeightUpdatingLearnRate = weight_updating_learn_rate_per_second*m_fFrameDuration;
  m_fLowInitialModeWeight = low_init_mode_weight;

  m_fModeUpdatingLearnRatePerSecond = mode_updating_learn_rate_per_second;
  m_fWeightUpdatingLearnRatePerSecond = weight_updating_learn_rate_per_second;

  m_f1_ModeUpdatingLearnRate = 1.0f - m_fModeUpdatingLearnRate;
  m_f1_WeightUpdatingLearnRate = 1.0f - m_fWeightUpdatingLearnRate;
}

int CMultiLayerBGS::Process() {
  BackgroundSubtractionProcess();
  return 1;
}

int CMultiLayerBGS::SetForegroundMaskImage(IplImage* fg_mask_img) {
  if (!fg_mask_img) {
    printf("Please allocate the IplImage memory!\n");
    return 0;
  }
  if (fg_mask_img->width != m_cvImgSize.width ||
    fg_mask_img->height != m_cvImgSize.height ||
    fg_mask_img->depth != IPL_DEPTH_8U ||
    fg_mask_img->nChannels != 1) {
    printf("Please provide the correct IplImage pointer, \ne.g. fg_mask_img = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);\n");
    return 0;
  }

  m_pFgMaskImg = fg_mask_img;

  return 1;
}

int CMultiLayerBGS::SetForegroundProbImage(IplImage* fg_prob_img) {
  if (!fg_prob_img) {
    printf("Please allocate the IplImage memory!\n");
    return 0;
  }
  if (fg_prob_img->width != m_cvImgSize.width ||
    fg_prob_img->height != m_cvImgSize.height ||
    fg_prob_img->depth != IPL_DEPTH_8U) {
    printf("Please provide the correct IplImage pointer, \ne.g. fg_prob_img = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);\n");
    return 0;
  }

  m_pFgProbImg = fg_prob_img;

  return 1;
}

void CMultiLayerBGS::SetCurrentFrameNumber(unsigned long cur_frame_no) {
  m_nCurImgFrameIdx = cur_frame_no;
}
