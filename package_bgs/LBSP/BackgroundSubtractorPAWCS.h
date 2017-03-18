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
#pragma once

#include "BackgroundSubtractorLBSP_.h"

//! defines the default value for BackgroundSubtractorLBSP_::m_fRelLBSPThreshold
#define BGSPAWCS_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD (0.333f)
//! defines the default value for BackgroundSubtractorPAWCS::m_nDescDistThresholdOffset
#define BGSPAWCS_DEFAULT_DESC_DIST_THRESHOLD_OFFSET (2)
//! defines the default value for BackgroundSubtractorPAWCS::m_nMinColorDistThreshold
#define BGSPAWCS_DEFAULT_MIN_COLOR_DIST_THRESHOLD (20)
//! defines the default value for BackgroundSubtractorPAWCS::m_nMaxLocalWords and m_nMaxGlobalWords
#define BGSPAWCS_DEFAULT_MAX_NB_WORDS (50)
//! defines the default value for BackgroundSubtractorPAWCS::m_nSamplesForMovingAvgs
#define BGSPAWCS_DEFAULT_N_SAMPLES_FOR_MV_AVGS (100)

/*!
    Pixel-based Adaptive Word Consensus Segmenter (PAWCS) change detection algorithm.

    Note: both grayscale and RGB/BGR images may be used with this extractor (parameters are adjusted automatically).
    For optimal grayscale results, use CV_8UC1 frames instead of CV_8UC3.

    For more details on the different parameters or on the algorithm itself, see P.-L. St-Charles et al.,
    "A Self-Adjusting Approach to Change Detection Based on Background Word Consensus", in WACV 2015.

    This algorithm is currently NOT thread-safe.
 */
class BackgroundSubtractorPAWCS : public BackgroundSubtractorLBSP_ {
public:
  //! full constructor
  BackgroundSubtractorPAWCS(float fRelLBSPThreshold = BGSPAWCS_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD,
    size_t nDescDistThresholdOffset = BGSPAWCS_DEFAULT_DESC_DIST_THRESHOLD_OFFSET,
    size_t nMinColorDistThreshold = BGSPAWCS_DEFAULT_MIN_COLOR_DIST_THRESHOLD,
    size_t nMaxNbWords = BGSPAWCS_DEFAULT_MAX_NB_WORDS,
    size_t nSamplesForMovingAvgs = BGSPAWCS_DEFAULT_N_SAMPLES_FOR_MV_AVGS);
  //! default destructor
  virtual ~BackgroundSubtractorPAWCS();
  //! (re)initiaization method; needs to be called before starting background subtraction
  virtual void initialize(const cv::Mat& oInitImg, const cv::Mat& oROI);
  //! refreshes all local (+ global) dictionaries based on the last analyzed frame
  virtual void refreshModel(size_t nBaseOccCount, float fOccDecrFrac, bool bForceFGUpdate = false);
  //! primary model update function; the learning param is used to override the internal learning speed (ignored when <= 0)
  virtual void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRateOverride = 0);
  //! returns a copy of the latest reconstructed background image
  virtual void getBackgroundImage(cv::OutputArray backgroundImage) const;
  //! returns a copy of the latest reconstructed background descriptors image
  virtual void getBackgroundDescriptorsImage(cv::OutputArray backgroundDescImage) const;

protected:
  template<size_t nChannels>
  struct ColorLBSPFeature {
    uchar anColor[nChannels];
    ushort anDesc[nChannels];
  };
  struct LocalWordBase {
    size_t nFirstOcc;
    size_t nLastOcc;
    size_t nOccurrences;
  };
  template<typename T>
  struct LocalWord : LocalWordBase {
    T oFeature;
  };
  struct GlobalWordBase {
    float fLatestWeight;
    cv::Mat oSpatioOccMap;
    uchar nDescBITS;
  };
  template<typename T>
  struct GlobalWord : GlobalWordBase {
    T oFeature;
  };
  typedef LocalWord<ColorLBSPFeature<1>> LocalWord_1ch;
  typedef LocalWord<ColorLBSPFeature<3>> LocalWord_3ch;
  typedef GlobalWord<ColorLBSPFeature<1>> GlobalWord_1ch;
  typedef GlobalWord<ColorLBSPFeature<3>> GlobalWord_3ch;
  struct PxInfo_PAWCS : PxInfoBase {
    size_t nGlobalWordMapLookupIdx;
    GlobalWordBase** apGlobalDictSortLUT;
  };
  //! absolute minimal color distance threshold ('R' or 'radius' in the original ViBe paper, used as the default/initial 'R(x)' value here)
  const size_t m_nMinColorDistThreshold;
  //! absolute descriptor distance threshold offset
  const size_t m_nDescDistThresholdOffset;
  //! max/curr number of local words used to build background submodels (for a single pixel, similar to 'N' in ViBe/PBAS, may vary based on img/channel size)
  size_t m_nMaxLocalWords, m_nCurrLocalWords;
  //! max/curr number of global words used to build the global background model (may vary based on img/channel size)
  size_t m_nMaxGlobalWords, m_nCurrGlobalWords;
  //! number of samples to use to compute the learning rate of moving averages
  const size_t m_nSamplesForMovingAvgs;
  //! last calculated non-flat region ratio
  float m_fLastNonFlatRegionRatio;
  //! current kernel size for median blur post-proc filtering
  int m_nMedianBlurKernelSize;
  //! specifies the downsampled frame size used for cam motion analysis & gword lookup maps
  cv::Size m_oDownSampledFrameSize_MotionAnalysis, m_oDownSampledFrameSize_GlobalWordLookup;
  //! downsampled version of the ROI used for cam motion analysis
  cv::Mat m_oDownSampledROI_MotionAnalysis;
  //! total pixel count for the downsampled ROIs
  size_t m_nDownSampledROIPxCount;
  //! current local word weight offset
  size_t m_nLocalWordWeightOffset;

  //! word lists & dictionaries
  LocalWordBase** m_apLocalWordDict;
  LocalWord_1ch* m_aLocalWordList_1ch, *m_pLocalWordListIter_1ch;
  LocalWord_3ch* m_aLocalWordList_3ch, *m_pLocalWordListIter_3ch;
  GlobalWordBase** m_apGlobalWordDict;
  GlobalWord_1ch* m_aGlobalWordList_1ch, *m_pGlobalWordListIter_1ch;
  GlobalWord_3ch* m_aGlobalWordList_3ch, *m_pGlobalWordListIter_3ch;
  PxInfo_PAWCS* m_aPxInfoLUT_PAWCS;

  //! a lookup map used to keep track of regions where illumination recently changed
  cv::Mat m_oIllumUpdtRegionMask;
  //! per-pixel update rates ('T(x)' in PBAS, which contains pixel-level 'sigmas', as referred to in ViBe)
  cv::Mat m_oUpdateRateFrame;
  //! per-pixel distance thresholds (equivalent to 'R(x)' in PBAS, but used as a relative value to determine both intensity and descriptor variation thresholds)
  cv::Mat m_oDistThresholdFrame;
  //! per-pixel distance threshold variation modulators ('v(x)', relative value used to modulate 'R(x)' and 'T(x)' variations)
  cv::Mat m_oDistThresholdVariationFrame;
  //! per-pixel mean minimal distances from the model ('D_min(x)' in PBAS, used to control variation magnitude and direction of 'T(x)' and 'R(x)')
  cv::Mat m_oMeanMinDistFrame_LT, m_oMeanMinDistFrame_ST;
  //! per-pixel mean downsampled distances between consecutive frames (used to analyze camera movement and force global model resets automatically)
  cv::Mat m_oMeanDownSampledLastDistFrame_LT, m_oMeanDownSampledLastDistFrame_ST;
  //! per-pixel mean raw segmentation results (used to detect unstable segmentation regions)
  cv::Mat m_oMeanRawSegmResFrame_LT, m_oMeanRawSegmResFrame_ST;
  //! per-pixel mean raw segmentation results (used to detect unstable segmentation regions)
  cv::Mat m_oMeanFinalSegmResFrame_LT, m_oMeanFinalSegmResFrame_ST;
  //! a lookup map used to keep track of unstable regions (based on segm. noise & local dist. thresholds)
  cv::Mat m_oUnstableRegionMask;
  //! per-pixel blink detection map ('Z(x)')
  cv::Mat m_oBlinksFrame;
  //! pre-allocated matrix used to downsample the input frame when needed
  cv::Mat m_oDownSampledFrame_MotionAnalysis;
  //! the foreground mask generated by the method at [t-1] (without post-proc, used for blinking px detection)
  cv::Mat m_oLastRawFGMask;

  //! pre-allocated CV_8UC1 matrices used to speed up morph ops
  cv::Mat m_oFGMask_PreFlood;
  cv::Mat m_oFGMask_FloodedHoles;
  cv::Mat m_oLastFGMask_dilated;
  cv::Mat m_oLastFGMask_dilated_inverted;
  cv::Mat m_oCurrRawFGBlinkMask;
  cv::Mat m_oLastRawFGBlinkMask;
  cv::Mat m_oTempGlobalWordWeightDiffFactor;
  cv::Mat m_oMorphExStructElement;

  //! internal cleanup function for the dictionary structures
  void CleanupDictionaries();
  //! internal weight lookup function for local words
  static float GetLocalWordWeight(const LocalWordBase* w, size_t nCurrFrame, size_t nOffset);
  //! internal weight lookup function for global words
  static float GetGlobalWordWeight(const GlobalWordBase* w);
};
