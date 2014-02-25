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
// BackgroundSubtraction.h: interface for the CBackgroundSubtraction class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_MULTI_LAYER_BGS_H_)
#define _MULTI_LAYER_BGS_H_

/*
Since the used fast cross bilateral filter codes can not be compiled under Windows,
we don't use the bilateral filter to remove the noise in the foreground detection
step. If you compile it under Linux, please uncomment it.
*/
//#define LINUX_BILATERAL_FILTER

#include "LocalBinaryPattern.h"
#include "BGS.h"
#include <stdio.h>
#include <stdarg.h>
#include "BlobResult.h"
#include "OpenCvDataConversion.h"

#include "BackgroundSubtractionAPI.h"

#ifdef LINUX_BILATERAL_FILTER
#include "CrossBilateralFilter.h"				// cross bilateral filter
#endif

#include <ctime>						// clock
#include <cstdlib>						// C standard library
#include <cstdio>						// C I/O (for sscanf)
#include <cstring>						// string manipulation
#include <fstream>						// file I/O
#include <cmath>						// math includes
#include <iostream>						// I/O streams

using namespace std;						// make std:: accessible

class CMultiLayerBGS : public CBackgroundSubtractionAPI
{
public:
  //-------------------------------------------------------------
  // TO CALL AT INITIALISATION: DEFINES THE SIZE OF THE INPUT IMAGES
  // NORMALLY, UNNECESSARY IF A CONFIGURATION FILE IS LOADED
  void   Init(int width, int height);

  //-------------------------------------------------------------
  // PROVIDE A MASK TO DEFINE THE SET OF POINTS WHERE BACKGROUND
  // SUBTRACTION DOES NOT NEED TO BE PERFORMED
  //
  //  mode is useful to specify if the points to remove from
  //  processing are in addition to the ones potentially
  //  removed according to the configuration file,
  //  or if they are the only ones to be removed
  //
  // mode=0 : provided points need to be removed
  //          in addition to those already removed
  // mode=1 : the provided points are the only one to remove
  //          from processing
  // Note:  maskImage(li,co)=0 indicate the points to remove
  //       from background processing
  void   SetValidPointMask(IplImage* maskImage, int mode);

  //-------------------------------------------------------------
  //
  //   set the frame rate, to adjust the update parameters
  //   to the actual frame rate.
  //   Can be called only once at initialisation,
  //   but in online cases, can be used to indicate
  //   the time interval during the last processed frame
  //
  //   frameDuration is in millisecond
  void   SetFrameRate(float    frameDuration);

  //-------------------------------------------------------------
  //
  //   set some main parameters for background model learning.
  //   in general, we can set large updating rates for background
  //   model learning and set small updating rates in foreground
  //   detection
  void SetParameters(int max_lbp_mode_num,		// maximal LBP mode number
    float mode_updating_learn_rate_per_second,	// background mode updating learning rate per second
    float weight_updating_learn_rate_per_second,	// mode's weight updating learning rate per second
    float low_init_mode_weight);			// the low initial mode weight

  //-------------------------------------------------------------
  //   PROVIDE A POINTER TO THE INPUT IMAGE
  //   -> INDICATE WHERE THE NEW IMAGE TO PROCESS IS STORED
  //
  //   Here assumes that the input image will contain RGB images.
  //   The memory of this image is handled by the caller.
  //
  //    The return value indicate whether the actual Background
  //    Subtraction algorithm handles RGB images (1) or not (0).
  //
  int   SetRGBInputImage(IplImage  *  inputImage, CvRect *roi = NULL);

  //-------------------------------------------------------------
  //   PROVIDE A POINTER TO THE RESULT IMAGE
  //   INDICATE WHERE THE BACKGROUND RESULT NEED TO BE STORED
  //
  int SetForegroundMaskImage(IplImage* fg_mask_img);
  int SetForegroundProbImage(IplImage* fg_prob_img);

  //-------------------------------------------------------------
  // This function should be called each time a new image is
  // available in the input image.
  //
  // The return value is 0 if everything goes well, a non-zero value
  // otherwise.
  //
  int   Process();

  //-------------------------------------------------------------
  // this function should save parameters and information of the model
  // (e.g. after a training of the model, or in such a way
  // that the model can be reload to process the next frame
  // type of save:
  //	0 - background model information (pixel by pixel)
  //	1 - background model parameters
  //	2 - both background information (pixel by pixel) and parameters
  void   Save(const char   *bg_model_fn, int save_type);
  void   Save(const char* bg_model_fn);

  //-------------------------------------------------------------
  // this function should load the parameters necessary
  // for the processing of the background subtraction or
  // load background model information
  bool   Load(const char  *bg_model_fn);


  void SetCurrentFrameNumber(unsigned long cur_frame_no);

  void GetForegroundMaskImage(IplImage *fg_mask_img);
  void GetForegroundImage(IplImage *fg_img, CvScalar bg_color = CV_RGB(0, 255, 0));
  void GetBackgroundImage(IplImage *bk_img);
  void GetForegroundProbabilityImage(IplImage* fg_prob_img);

  void GetBgLayerNoImage(IplImage *bg_layer_no_img, CvScalar* layer_colors = NULL, int layer_num = 0);
  void GetLayeredBackgroundImage(int layered_no, IplImage *layered_bg_img, CvScalar empty_color = CV_RGB(0, 0, 0));
  void GetCurrentLayeredBackgroundImage(int layered_no, IplImage *layered_bg_img, IplImage *layered_fg_img = NULL,
    CvScalar layered_bg_bk_color = CV_RGB(0, 0, 0), CvScalar layered_fg_color = CV_RGB(255, 0, 0),
    int smooth_win = 13, float smooth_sigma = 3.0f, float below_layer_noise = 0.5f, float above_layer_noise = 0.3f, int min_blob_size = 50);
  float DistLBP(LBPStruct *LBP1, LBPStruct *LBP2);
  void GetColoredBgMultiLayeredImage(IplImage *bg_multi_layer_img, CvScalar *layer_colors);
  void UpdatePatternColorDistWeights(float *cur_pattern, float *bg_pattern);
  void ExportLogMessage(char* msg);
  void Postprocessing();
  void GetFloatEdgeImage(IplImage *src, IplImage *dst);
  void RemoveBackgroundLayers(PixelLBPStruct *PLBP, bool *removed_modes = NULL);
  float CalColorRangeDist(unsigned char *cur_intensity, float *bg_intensity, float *max_intensity,
    float *min_intensity, float shadow_rate, float highlight_rate);
  float CalVectorsAngle(float *c1, unsigned char *c2, int length);
  float CalVectorsNoisedAngle(float *bg_color, unsigned char *noised_color, float offset, int length);
  void ComputeGradientImage(IplImage *src, IplImage *dst, bool bIsFloat);
  float CalColorBgDist(uchar *cur_intensity, float *bg_intensity, float *max_intensity, float *min_intensity);
  float CalPatternBgDist(float *cur_pattern, float *bg_pattern);

  void GetForegroundMaskMap(CvMat *fg_mask_mat);
  void Initialization(IplImage *first_img, int lbp_level_num, float *radiuses, int *neig_pt_nums);
  void GetCurrentBackgroundDistMap(CvMat *bk_dist_map);
  void BackgroundSubtractionProcess();
  void SetBkMaskImage(IplImage *mask_img);
  void SetNewImage(IplImage *new_img, CvRect *roi = NULL);

  void ResetAllParameters();
  void QuickSort(float *pData, unsigned short *pIdxes, long low, long high, bool bAscent);
  void UpdateBgPixelPattern(float *cur_pattern, float *bg_bg_pattern);
  void UpdateBgPixelColor(unsigned char* cur_intensity, float* bg_intensity);
  void Update_MAX_MIN_Intensity(unsigned char *cur_intensity, float *max_intensity, float *min_intensity);
  void MergeImages(int num, ...);

  int	m_nChannel;				/* most of opencv functions support 1,2,3 or 4 channels, for the input images */

  PixelLBPStruct*	m_pPixelLBPs;			/* the LBP texture patterns for each image */
  int	m_nMaxLBPModeNum;			/* the maximal number for the used LBP pattern models */
  float	m_fModeUpdatingLearnRate;		/* the background mode learning rate */
  float	m_fWeightUpdatingLearnRate;		/* the background mode weight updating rate */
  float	m_f1_ModeUpdatingLearnRate;		/* 1 - background_mode_learning_rate */
  float	m_f1_WeightUpdatingLearnRate;		/* 1 - background_mode_weight_updating_rate */
  float	m_fRobustColorOffset;			/* the intensity offset robust to noise */
  float	m_fLowInitialModeWeight;		/* the lowest weight of initial background mode */
  int	m_nLBPLength;				/* the length of texture LBP operator */
  float	m_fPatternColorDistBgThreshold;		/* the threshold value used to classify background and foreground */
  float	m_fPatternColorDistBgUpdatedThreshold;	/* the threshold value used to update the background modeling */
  float	m_fMinBgLayerWeight;			/* the minimal weight to remove background layers */

  int	m_nPatternDistSmoothNeigHalfSize;	/* the neighboring half size of gaussian window to remove the noise
                                        on the distance map */
  float	m_fPatternDistConvGaussianSigma;	/* the gaussian sigma used to remove the noise on the distance map */

  float	m_fBackgroundModelPercent;		/* the background mode percent, the first several background modes
                                      with high mode weights should be regarded as reliable background modes */

  float	m_fRobustShadowRate;			/* the minimal shadow rate, [0.4, 0.7] */
  float	m_fRobustHighlightRate;			/* the maximal highlight rate, [1.1, 1.4] */

  int	m_nLBPImgNum;				/* the number of images used for texture LBP feature */

  float	m_fMinLBPBinaryProb;			/* the minimal LBP binary probability */
  float	m_f1_MinLBPBinaryProb;			/* 1 - minimal_LBP_binary_probability */

  CvSize	m_cvImgSize;				/* the image size (width, height) */

  unsigned long	m_nCurImgFrameIdx;			/* the frame index of current image */

  bool	m_bUsedGradImage;			/* the boolean variable signaling whether the gradient image is used
                              or not for computing LBP operator */

  bool	m_bUsedColorLBP;			/* true - multi-channel color image for LBP operator,
                              false - gray-scale image for LBP operator  */

  CLocalBinaryPattern	m_cLBP;			/* the class instant for computing LBP (local binary pattern) texture feature */

  IplImage* m_pBkMaskImg;				/* the mask image corresponding to the input image,
                                i.e. all the masked pixels should be processed  */

  IplImage* m_pOrgImg;				/* the original image */
  IplImage** m_ppOrgLBPImgs;			/* the multi-layer images used for LBP feature extraction */
  IplImage* m_pFgImg;				/* the foreground image */
  IplImage* m_pBgImg;				/* the background image */
  IplImage* m_pFgMaskImg;				/* the foreground mask image */
  IplImage* m_pBgDistImg;				/* the background distance image (float) */
  IplImage* m_pEdgeImg;				/* the edge image used for cross bilateral filter */
  IplImage* m_pFgProbImg;				/* the foreground probability image (uchar) */

  IplImage* m_pFirstAppearingTimeMap;

#ifdef LINUX_BILATERAL_FILTER
  CCrossBilateralFilter	m_cCrossBF;		/* the class instant for cross bilateral filter
                                      which should be used to remove noise on the distance map */
#endif

  bool	m_disableLearning;
  float	m_fSigmaS;				/* sigma in the spatial domain for cross bilateral filter */
  float	m_fSigmaR;				/* sigma in the normalized intensity domain for cross bilateral filter */

  float	m_fTextureWeight;			/* the weight value of texture LBP feature
                              for background modeling & foreground detection */

  float	m_fColorWeight;				/* the weight value of color invariant feature
                              for background modeling & foreground detection */

  float	m_fWeightUpdatingConstant;		/* the constant ( >= 1 ) for 'hysteries' weight updating scheme
                                      (increase when matched, decrease when un-matched */

  float	m_fReliableBackgroundModeWeight;	/* the weight value for background mode
                                          which should be regarded as a reliable background mode,
                                          which is useful for multi-layer scheme */

  float	m_fMinNoisedAngle;			/* the minimal angle value between the background color
                                and the noised observed color */

  float	m_fMinNoisedAngleSine;			/* the minimal angle sine value between the background color
                                    and the noised observed color */

  float	m_fFrameDuration;			/* frame duration */

  float	m_fModeUpdatingLearnRatePerSecond;
  float	m_fWeightUpdatingLearnRatePerSecond;

  int m_nLBPLevelNum;
  float m_pLBPRadiuses[10];
  int m_pLBPMeigPointNums[10];

  CvRect* m_pROI;
  CMultiLayerBGS();
  virtual ~CMultiLayerBGS();
};

#endif // !defined(_MULTI_LAYER_BGS_H_)

