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
#include "BackgroundSubtractorPAWCS.h"
#include "DistanceUtils.h"
#include "RandUtils.h"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iomanip>

/*
 *
 * Intrinsic configuration parameters are defined here; tuning these for better
 * performance should not be required in most cases -- although improvements in
 * very specific scenarios are always possible.
 *
 */
 //! parameter used for dynamic distance threshold adjustments ('R(x)')
#define FEEDBACK_R_VAR (0.01f)
//! parameters used to adjust the variation step size of 'v(x)'
#define FEEDBACK_V_INCR  (1.000f)
#define FEEDBACK_V_DECR  (0.100f)
//! parameters used to scale dynamic learning rate adjustments  ('T(x)')
#define FEEDBACK_T_DECR  (0.2500f)
#define FEEDBACK_T_INCR  (0.5000f)
#define FEEDBACK_T_LOWER (1.0000f)
#define FEEDBACK_T_UPPER (256.00f)
//! parameters used to define 'unstable' regions, based on segm noise/bg dynamics and local dist threshold values
#define UNSTABLE_REG_RATIO_MIN (0.100f)
#define UNSTABLE_REG_RDIST_MIN (3.000f)
//! parameters used to scale the relative LBSP intensity threshold used for internal comparisons
#define LBSPDESC_RATIO_MIN (0.100f)
#define LBSPDESC_RATIO_MAX (0.500f)
//! parameters used to trigger auto model resets in our frame-level component
#define FRAMELEVEL_MIN_L1DIST_THRES (45)
#define FRAMELEVEL_MIN_CDIST_THRES (FRAMELEVEL_MIN_L1DIST_THRES/10)
#define FRAMELEVEL_DOWNSAMPLE_RATIO (8)
//! parameters used to downscale gword maps & scale thresholds to make comparisons easier
#define GWORD_LOOKUP_MAPS_DOWNSAMPLE_RATIO (2)
#define GWORD_DEFAULT_NB_INIT_SAMPL_PASSES (2)
#define GWORD_DESC_THRES_BITS_MATCH_FACTOR (4)

// local define used to specify the default frame size (320x240 = QVGA)
#define DEFAULT_FRAME_SIZE cv::Size(320,240)
// local define used to specify the default lword/gword update rate (16 = like vibe)
#define DEFAULT_RESAMPLING_RATE (16)
// local define used to specify the bootstrap window size for faster model stabilization
#define DEFAULT_BOOTSTRAP_WIN_SIZE (500)
// local define for the amount of weight offset to apply to words, making sure new words aren't always better than old ones
#define DEFAULT_LWORD_WEIGHT_OFFSET (DEFAULT_BOOTSTRAP_WIN_SIZE*2)
// local define used to set the default local word occurrence increment
#define DEFAULT_LWORD_OCC_INCR 1
// local define for the maximum weight a word can achieve before cutting off occ incr (used to make sure model stays good for long-term uses)
#define DEFAULT_LWORD_MAX_WEIGHT (1.0f)
// local define for the initial weight of a new word (used to make sure old words aren't worse off than new seeds)
#define DEFAULT_LWORD_INIT_WEIGHT (1.0f/m_nLocalWordWeightOffset)
// local define used to specify the desc dist threshold offset used for unstable regions
#define UNSTAB_DESC_DIST_OFFSET (m_nDescDistThresholdOffset)
// local define used to specify the min descriptor bit count for flat regions
#define FLAT_REGION_BIT_COUNT (s_nDescMaxDataRange_1ch/8)

static const size_t s_nColorMaxDataRange_1ch = UCHAR_MAX;
static const size_t s_nDescMaxDataRange_1ch = LBSP_::DESC_SIZE * 8;
static const size_t s_nColorMaxDataRange_3ch = s_nColorMaxDataRange_1ch * 3;
static const size_t s_nDescMaxDataRange_3ch = s_nDescMaxDataRange_1ch * 3;

BackgroundSubtractorPAWCS::BackgroundSubtractorPAWCS(float fRelLBSPThreshold
  , size_t nDescDistThresholdOffset
  , size_t nMinColorDistThreshold
  , size_t nMaxNbWords
  , size_t nSamplesForMovingAvgs)
  : BackgroundSubtractorLBSP_(fRelLBSPThreshold)
  , m_nMinColorDistThreshold(nMinColorDistThreshold)
  , m_nDescDistThresholdOffset(nDescDistThresholdOffset)
  , m_nMaxLocalWords(nMaxNbWords)
  , m_nCurrLocalWords(0)
  , m_nMaxGlobalWords(nMaxNbWords / 2)
  , m_nCurrGlobalWords(0)
  , m_nSamplesForMovingAvgs(nSamplesForMovingAvgs)
  , m_fLastNonFlatRegionRatio(0.0f)
  , m_nMedianBlurKernelSize(m_nDefaultMedianBlurKernelSize)
  , m_nDownSampledROIPxCount(0)
  , m_nLocalWordWeightOffset(DEFAULT_LWORD_WEIGHT_OFFSET)
  , m_apLocalWordDict(nullptr)
  , m_aLocalWordList_1ch(nullptr)
  , m_pLocalWordListIter_1ch(nullptr)
  , m_aLocalWordList_3ch(nullptr)
  , m_pLocalWordListIter_3ch(nullptr)
  , m_apGlobalWordDict(nullptr)
  , m_aGlobalWordList_1ch(nullptr)
  , m_pGlobalWordListIter_1ch(nullptr)
  , m_aGlobalWordList_3ch(nullptr)
  , m_pGlobalWordListIter_3ch(nullptr)
  , m_aPxInfoLUT_PAWCS(nullptr) {
  CV_Assert(m_nMaxLocalWords > 0 && m_nMaxGlobalWords > 0);
}

BackgroundSubtractorPAWCS::~BackgroundSubtractorPAWCS() {
  CleanupDictionaries();
}

void BackgroundSubtractorPAWCS::initialize(const cv::Mat& oInitImg, const cv::Mat& oROI) {
  // == init
  CV_Assert(!oInitImg.empty() && oInitImg.cols > 0 && oInitImg.rows > 0);
  CV_Assert(oInitImg.isContinuous());
  CV_Assert(oInitImg.type() == CV_8UC3 || oInitImg.type() == CV_8UC1);
  if (oInitImg.type() == CV_8UC3) {
    std::vector<cv::Mat> voInitImgChannels;
    cv::split(oInitImg, voInitImgChannels);
    if (!cv::countNonZero((voInitImgChannels[0] != voInitImgChannels[1]) | (voInitImgChannels[2] != voInitImgChannels[1])))
      std::cout << std::endl << "\tBackgroundSubtractorPAWCS : Warning, grayscale images should always be passed in CV_8UC1 format for optimal performance." << std::endl;
  }
  cv::Mat oNewBGROI;
  if (oROI.empty() && (m_oROI.empty() || oROI.size() != oInitImg.size())) {
    oNewBGROI.create(oInitImg.size(), CV_8UC1);
    oNewBGROI = cv::Scalar_<uchar>(UCHAR_MAX);
  }
  else if (oROI.empty())
    oNewBGROI = m_oROI;
  else {
    CV_Assert(oROI.size() == oInitImg.size() && oROI.type() == CV_8UC1);
    CV_Assert(cv::countNonZero((oROI < UCHAR_MAX)&(oROI > 0)) == 0);
    oNewBGROI = oROI.clone();
    cv::Mat oTempROI;
    cv::dilate(oNewBGROI, oTempROI, cv::Mat(), cv::Point(-1, -1), LBSP_::PATCH_SIZE / 2);
    cv::bitwise_or(oNewBGROI, oTempROI / 2, oNewBGROI);
  }
  const size_t nOrigROIPxCount = (size_t)cv::countNonZero(oNewBGROI);
  CV_Assert(nOrigROIPxCount > 0);
  LBSP_::validateROI(oNewBGROI);
  const size_t nFinalROIPxCount = (size_t)cv::countNonZero(oNewBGROI);
  CV_Assert(nFinalROIPxCount > 0);
  CleanupDictionaries();
  m_oROI = oNewBGROI;
  m_oImgSize = oInitImg.size();
  m_nImgType = oInitImg.type();
  m_nImgChannels = oInitImg.channels();
  m_nTotPxCount = m_oImgSize.area();
  m_nTotRelevantPxCount = nFinalROIPxCount;
  m_nFrameIndex = 0;
  m_nFramesSinceLastReset = 0;
  m_nModelResetCooldown = 0;
  m_bUsingMovingCamera = false;
  m_oDownSampledFrameSize_MotionAnalysis = cv::Size(m_oImgSize.width / FRAMELEVEL_DOWNSAMPLE_RATIO, m_oImgSize.height / FRAMELEVEL_DOWNSAMPLE_RATIO);
  m_oDownSampledFrameSize_GlobalWordLookup = cv::Size(m_oImgSize.width / GWORD_LOOKUP_MAPS_DOWNSAMPLE_RATIO, m_oImgSize.height / GWORD_LOOKUP_MAPS_DOWNSAMPLE_RATIO);
  cv::resize(m_oROI, m_oDownSampledROI_MotionAnalysis, m_oDownSampledFrameSize_MotionAnalysis, 0, 0, cv::INTER_AREA);
  m_fLastNonFlatRegionRatio = 0.0f;
  m_nCurrLocalWords = m_nMaxLocalWords;
  if (nOrigROIPxCount >= m_nTotPxCount / 2 && (int)m_nTotPxCount >= DEFAULT_FRAME_SIZE.area()) {
    const float fRegionSizeScaleFactor = (float)m_nTotPxCount / DEFAULT_FRAME_SIZE.area();
    const int nRawMedianBlurKernelSize = std::min((int)floor(0.5f + fRegionSizeScaleFactor) + m_nDefaultMedianBlurKernelSize, m_nDefaultMedianBlurKernelSize + 4);
    m_nMedianBlurKernelSize = (nRawMedianBlurKernelSize % 2) ? nRawMedianBlurKernelSize : nRawMedianBlurKernelSize - 1;
    m_nCurrGlobalWords = m_nMaxGlobalWords;
    m_oDownSampledROI_MotionAnalysis |= UCHAR_MAX / 2;
  }
  else {
    const float fRegionSizeScaleFactor = (float)nOrigROIPxCount / DEFAULT_FRAME_SIZE.area();
    const int nRawMedianBlurKernelSize = std::min((int)floor(0.5f + m_nDefaultMedianBlurKernelSize*fRegionSizeScaleFactor * 2) + (m_nDefaultMedianBlurKernelSize - 4), m_nDefaultMedianBlurKernelSize);
    m_nMedianBlurKernelSize = (nRawMedianBlurKernelSize % 2) ? nRawMedianBlurKernelSize : nRawMedianBlurKernelSize - 1;
    m_nCurrGlobalWords = std::min((size_t)std::pow(m_nMaxGlobalWords*fRegionSizeScaleFactor, 2) + 1, m_nMaxGlobalWords);
  }
  if (m_nImgChannels == 1) {
    m_nCurrLocalWords = std::max(m_nCurrLocalWords / 2, (size_t)1);
    m_nCurrGlobalWords = std::max(m_nCurrGlobalWords / 2, (size_t)1);
  }
  m_nDownSampledROIPxCount = (size_t)cv::countNonZero(m_oDownSampledROI_MotionAnalysis);
  m_nLocalWordWeightOffset = DEFAULT_LWORD_WEIGHT_OFFSET;
  m_oIllumUpdtRegionMask.create(m_oImgSize, CV_8UC1);
  m_oIllumUpdtRegionMask = cv::Scalar_<uchar>(0);
  m_oUpdateRateFrame.create(m_oImgSize, CV_32FC1);
  m_oUpdateRateFrame = cv::Scalar(FEEDBACK_T_LOWER);
  m_oDistThresholdFrame.create(m_oImgSize, CV_32FC1);
  m_oDistThresholdFrame = cv::Scalar(2.0f);
  m_oDistThresholdVariationFrame.create(m_oImgSize, CV_32FC1);
  m_oDistThresholdVariationFrame = cv::Scalar(FEEDBACK_V_INCR * 10);
  m_oMeanMinDistFrame_LT.create(m_oImgSize, CV_32FC1);
  m_oMeanMinDistFrame_LT = cv::Scalar(0.0f);
  m_oMeanMinDistFrame_ST.create(m_oImgSize, CV_32FC1);
  m_oMeanMinDistFrame_ST = cv::Scalar(0.0f);
  m_oMeanDownSampledLastDistFrame_LT.create(m_oDownSampledFrameSize_MotionAnalysis, CV_32FC((int)m_nImgChannels));
  m_oMeanDownSampledLastDistFrame_LT = cv::Scalar(0.0f);
  m_oMeanDownSampledLastDistFrame_ST.create(m_oDownSampledFrameSize_MotionAnalysis, CV_32FC((int)m_nImgChannels));
  m_oMeanDownSampledLastDistFrame_ST = cv::Scalar(0.0f);
  m_oMeanRawSegmResFrame_LT.create(m_oImgSize, CV_32FC1);
  m_oMeanRawSegmResFrame_LT = cv::Scalar(0.0f);
  m_oMeanRawSegmResFrame_ST.create(m_oImgSize, CV_32FC1);
  m_oMeanRawSegmResFrame_ST = cv::Scalar(0.0f);
  m_oMeanFinalSegmResFrame_LT.create(m_oImgSize, CV_32FC1);
  m_oMeanFinalSegmResFrame_LT = cv::Scalar(0.0f);
  m_oMeanFinalSegmResFrame_ST.create(m_oImgSize, CV_32FC1);
  m_oMeanFinalSegmResFrame_ST = cv::Scalar(0.0f);
  m_oUnstableRegionMask.create(m_oImgSize, CV_8UC1);
  m_oUnstableRegionMask = cv::Scalar_<uchar>(0);
  m_oBlinksFrame.create(m_oImgSize, CV_8UC1);
  m_oBlinksFrame = cv::Scalar_<uchar>(0);
  m_oDownSampledFrame_MotionAnalysis.create(m_oDownSampledFrameSize_MotionAnalysis, CV_8UC((int)m_nImgChannels));
  m_oDownSampledFrame_MotionAnalysis = cv::Scalar_<uchar>::all(0);
  m_oLastColorFrame.create(m_oImgSize, CV_8UC((int)m_nImgChannels));
  m_oLastColorFrame = cv::Scalar_<uchar>::all(0);
  m_oLastDescFrame.create(m_oImgSize, CV_16UC((int)m_nImgChannels));
  m_oLastDescFrame = cv::Scalar_<ushort>::all(0);
  m_oLastRawFGMask.create(m_oImgSize, CV_8UC1);
  m_oLastRawFGMask = cv::Scalar_<uchar>(0);
  m_oLastFGMask.create(m_oImgSize, CV_8UC1);
  m_oLastFGMask = cv::Scalar_<uchar>(0);
  m_oLastFGMask_dilated.create(m_oImgSize, CV_8UC1);
  m_oLastFGMask_dilated = cv::Scalar_<uchar>(0);
  m_oLastFGMask_dilated_inverted.create(m_oImgSize, CV_8UC1);
  m_oLastFGMask_dilated_inverted = cv::Scalar_<uchar>(0);
  m_oFGMask_FloodedHoles.create(m_oImgSize, CV_8UC1);
  m_oFGMask_FloodedHoles = cv::Scalar_<uchar>(0);
  m_oFGMask_PreFlood.create(m_oImgSize, CV_8UC1);
  m_oFGMask_PreFlood = cv::Scalar_<uchar>(0);
  m_oCurrRawFGBlinkMask.create(m_oImgSize, CV_8UC1);
  m_oCurrRawFGBlinkMask = cv::Scalar_<uchar>(0);
  m_oLastRawFGBlinkMask.create(m_oImgSize, CV_8UC1);
  m_oLastRawFGBlinkMask = cv::Scalar_<uchar>(0);
  m_oTempGlobalWordWeightDiffFactor.create(m_oDownSampledFrameSize_GlobalWordLookup, CV_32FC1);
  m_oTempGlobalWordWeightDiffFactor = cv::Scalar(-0.1f);
  m_oMorphExStructElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
  m_aPxIdxLUT = new size_t[m_nTotRelevantPxCount];
  memset(m_aPxIdxLUT, 0, sizeof(size_t)*m_nTotRelevantPxCount);
  m_aPxInfoLUT_PAWCS = new PxInfo_PAWCS[m_nTotPxCount];
  memset(m_aPxInfoLUT_PAWCS, 0, sizeof(PxInfo_PAWCS)*m_nTotPxCount);
  m_aPxInfoLUT = m_aPxInfoLUT_PAWCS;
  m_apLocalWordDict = new LocalWordBase*[m_nTotRelevantPxCount*m_nCurrLocalWords];
  memset(m_apLocalWordDict, 0, sizeof(LocalWordBase*)*m_nTotRelevantPxCount*m_nCurrLocalWords);
  m_apGlobalWordDict = new GlobalWordBase*[m_nCurrGlobalWords];
  memset(m_apGlobalWordDict, 0, sizeof(GlobalWordBase*)*m_nCurrGlobalWords);
  if (m_nImgChannels == 1) {
    CV_DbgAssert(m_oLastColorFrame.step.p[0] == (size_t)m_oImgSize.width && m_oLastColorFrame.step.p[1] == 1);
    CV_DbgAssert(m_oLastDescFrame.step.p[0] == m_oLastColorFrame.step.p[0] * 2 && m_oLastDescFrame.step.p[1] == m_oLastColorFrame.step.p[1] * 2);
    m_aLocalWordList_1ch = new LocalWord_1ch[m_nTotRelevantPxCount*m_nCurrLocalWords];
    memset(m_aLocalWordList_1ch, 0, sizeof(LocalWord_1ch)*m_nTotRelevantPxCount*m_nCurrLocalWords);
    m_pLocalWordListIter_1ch = m_aLocalWordList_1ch;
    m_aGlobalWordList_1ch = new GlobalWord_1ch[m_nCurrGlobalWords];
    m_pGlobalWordListIter_1ch = m_aGlobalWordList_1ch;
    for (size_t t = 0; t <= UCHAR_MAX; ++t)
      m_anLBSPThreshold_8bitLUT[t] = cv::saturate_cast<uchar>((m_nLBSPThresholdOffset + t*m_fRelLBSPThreshold) / 3);
    for (size_t nPxIter = 0, nModelIter = 0; nPxIter < m_nTotPxCount; ++nPxIter) {
      if (m_oROI.data[nPxIter]) {
        m_aPxIdxLUT[nModelIter] = nPxIter;
        m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y = (int)nPxIter / m_oImgSize.width;
        m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X = (int)nPxIter%m_oImgSize.width;
        m_aPxInfoLUT_PAWCS[nPxIter].nModelIdx = nModelIter;
        m_aPxInfoLUT_PAWCS[nPxIter].nGlobalWordMapLookupIdx = (size_t)((m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y / GWORD_LOOKUP_MAPS_DOWNSAMPLE_RATIO)*m_oDownSampledFrameSize_GlobalWordLookup.width + (m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X / GWORD_LOOKUP_MAPS_DOWNSAMPLE_RATIO)) * 4;
        m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT = new GlobalWordBase*[m_nCurrGlobalWords];
        for (size_t nGlobalWordIdxIter = 0; nGlobalWordIdxIter < m_nCurrGlobalWords; ++nGlobalWordIdxIter)
          m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordIdxIter] = &(m_aGlobalWordList_1ch[nGlobalWordIdxIter]);
        m_oLastColorFrame.data[nPxIter] = oInitImg.data[nPxIter];
        const size_t nDescIter = nPxIter * 2;
        LBSP_::computeGrayscaleDescriptor(oInitImg, oInitImg.data[nPxIter], m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X, m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y, m_anLBSPThreshold_8bitLUT[oInitImg.data[nPxIter]], *((ushort*)(m_oLastDescFrame.data + nDescIter)));
        ++nModelIter;
      }
    }
  }
  else { //m_nImgChannels==3
    CV_DbgAssert(m_oLastColorFrame.step.p[0] == (size_t)m_oImgSize.width * 3 && m_oLastColorFrame.step.p[1] == 3);
    CV_DbgAssert(m_oLastDescFrame.step.p[0] == m_oLastColorFrame.step.p[0] * 2 && m_oLastDescFrame.step.p[1] == m_oLastColorFrame.step.p[1] * 2);
    m_aLocalWordList_3ch = new LocalWord_3ch[m_nTotRelevantPxCount*m_nCurrLocalWords];
    memset(m_aLocalWordList_3ch, 0, sizeof(LocalWord_3ch)*m_nTotRelevantPxCount*m_nCurrLocalWords);
    m_pLocalWordListIter_3ch = m_aLocalWordList_3ch;
    m_aGlobalWordList_3ch = new GlobalWord_3ch[m_nCurrGlobalWords];
    m_pGlobalWordListIter_3ch = m_aGlobalWordList_3ch;
    for (size_t t = 0; t <= UCHAR_MAX; ++t)
      m_anLBSPThreshold_8bitLUT[t] = cv::saturate_cast<uchar>(m_nLBSPThresholdOffset + t*m_fRelLBSPThreshold);
    for (size_t nPxIter = 0, nModelIter = 0; nPxIter < m_nTotPxCount; ++nPxIter) {
      if (m_oROI.data[nPxIter]) {
        m_aPxIdxLUT[nModelIter] = nPxIter;
        m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y = (int)nPxIter / m_oImgSize.width;
        m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X = (int)nPxIter%m_oImgSize.width;
        m_aPxInfoLUT_PAWCS[nPxIter].nModelIdx = nModelIter;
        m_aPxInfoLUT_PAWCS[nPxIter].nGlobalWordMapLookupIdx = (size_t)((m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y / GWORD_LOOKUP_MAPS_DOWNSAMPLE_RATIO)*m_oDownSampledFrameSize_GlobalWordLookup.width + (m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X / GWORD_LOOKUP_MAPS_DOWNSAMPLE_RATIO)) * 4;
        m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT = new GlobalWordBase*[m_nCurrGlobalWords];
        for (size_t nGlobalWordIdxIter = 0; nGlobalWordIdxIter < m_nCurrGlobalWords; ++nGlobalWordIdxIter)
          m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordIdxIter] = &(m_aGlobalWordList_3ch[nGlobalWordIdxIter]);
        const size_t nPxRGBIter = nPxIter * 3;
        const size_t nDescRGBIter = nPxRGBIter * 2;
        for (size_t c = 0; c < 3; ++c) {
          m_oLastColorFrame.data[nPxRGBIter + c] = oInitImg.data[nPxRGBIter + c];
          LBSP_::computeSingleRGBDescriptor(oInitImg, oInitImg.data[nPxRGBIter + c], m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X, m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y, c, m_anLBSPThreshold_8bitLUT[oInitImg.data[nPxRGBIter + c]], ((ushort*)(m_oLastDescFrame.data + nDescRGBIter))[c]);
        }
        ++nModelIter;
      }
    }
  }
  m_bInitialized = true;
  refreshModel(1, 0);
}

void BackgroundSubtractorPAWCS::refreshModel(size_t nBaseOccCount, float fOccDecrFrac, bool bForceFGUpdate) {
  // == refresh
  CV_Assert(m_bInitialized);
  CV_Assert(fOccDecrFrac >= 0.0f && fOccDecrFrac <= 1.0f);
  if (m_nImgChannels == 1) {
    for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
      const size_t nPxIter = m_aPxIdxLUT[nModelIter];
      if (bForceFGUpdate || !m_oLastFGMask_dilated.data[nPxIter]) {
        const size_t nLocalDictIdx = nModelIter*m_nCurrLocalWords;
        const size_t nFloatIter = nPxIter * 4;
        uchar& bCurrRegionIsUnstable = m_oUnstableRegionMask.data[nPxIter];
        const float fCurrDistThresholdFactor = *(float*)(m_oDistThresholdFrame.data + nFloatIter);
        const size_t nCurrColorDistThreshold = (size_t)(sqrt(fCurrDistThresholdFactor)*m_nMinColorDistThreshold) / 2;
        const size_t nCurrDescDistThreshold = ((size_t)1 << ((size_t)floor(fCurrDistThresholdFactor + 0.5f))) + m_nDescDistThresholdOffset + (bCurrRegionIsUnstable*UNSTAB_DESC_DIST_OFFSET);
        // == refresh: local decr
        if (fOccDecrFrac > 0.0f) {
          for (size_t nLocalWordIdx = 0; nLocalWordIdx < m_nCurrLocalWords; ++nLocalWordIdx) {
            LocalWord_1ch* pCurrLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
            if (pCurrLocalWord)
              pCurrLocalWord->nOccurrences -= (size_t)(fOccDecrFrac*pCurrLocalWord->nOccurrences);
          }
        }
        const size_t nCurrWordOccIncr = DEFAULT_LWORD_OCC_INCR;
        const size_t nTotLocalSamplingIterCount = (s_nSamplesInitPatternWidth*s_nSamplesInitPatternHeight) * 2;
        for (size_t nLocalSamplingIter = 0; nLocalSamplingIter < nTotLocalSamplingIterCount; ++nLocalSamplingIter) {
          // == refresh: local resampling
          int nSampleImgCoord_Y, nSampleImgCoord_X;
          getRandSamplePosition(nSampleImgCoord_X, nSampleImgCoord_Y, m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X, m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y, LBSP_::PATCH_SIZE / 2, m_oImgSize);
          const size_t nSamplePxIdx = m_oImgSize.width*nSampleImgCoord_Y + nSampleImgCoord_X;
          if (bForceFGUpdate || !m_oLastFGMask_dilated.data[nSamplePxIdx]) {
            const uchar nSampleColor = m_oLastColorFrame.data[nSamplePxIdx];
            const size_t nSampleDescIdx = nSamplePxIdx * 2;
            const ushort nSampleIntraDesc = *((ushort*)(m_oLastDescFrame.data + nSampleDescIdx));
            bool bFoundUninitd = false;
            size_t nLocalWordIdx;
            for (nLocalWordIdx = 0; nLocalWordIdx < m_nCurrLocalWords; ++nLocalWordIdx) {
              LocalWord_1ch* pCurrLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
              if (pCurrLocalWord
                && L1dist(nSampleColor, pCurrLocalWord->oFeature.anColor[0]) <= nCurrColorDistThreshold
                && hdist(nSampleIntraDesc, pCurrLocalWord->oFeature.anDesc[0]) <= nCurrDescDistThreshold) {
                pCurrLocalWord->nOccurrences += nCurrWordOccIncr;
                pCurrLocalWord->nLastOcc = m_nFrameIndex;
                break;
              }
              else if (!pCurrLocalWord)
                bFoundUninitd = true;
            }
            if (nLocalWordIdx == m_nCurrLocalWords) {
              nLocalWordIdx = m_nCurrLocalWords - 1;
              LocalWord_1ch* pCurrLocalWord = bFoundUninitd ? m_pLocalWordListIter_1ch++ : (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
              pCurrLocalWord->oFeature.anColor[0] = nSampleColor;
              pCurrLocalWord->oFeature.anDesc[0] = nSampleIntraDesc;
              pCurrLocalWord->nOccurrences = nBaseOccCount;
              pCurrLocalWord->nFirstOcc = m_nFrameIndex;
              pCurrLocalWord->nLastOcc = m_nFrameIndex;
              m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx] = pCurrLocalWord;
            }
            while (nLocalWordIdx > 0 && (!m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1] || GetLocalWordWeight(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_nFrameIndex, m_nLocalWordWeightOffset) > GetLocalWordWeight(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1], m_nFrameIndex, m_nLocalWordWeightOffset))) {
              std::swap(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1]);
              --nLocalWordIdx;
            }
          }
        }
        CV_Assert(m_apLocalWordDict[nLocalDictIdx]);
        for (size_t nLocalWordIdx = 1; nLocalWordIdx < m_nCurrLocalWords; ++nLocalWordIdx) {
          // == refresh: local random resampling
          LocalWord_1ch* pCurrLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
          if (!pCurrLocalWord) {
            const size_t nRandLocalWordIdx = (rand() % nLocalWordIdx);
            const LocalWord_1ch* pRefLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx + nRandLocalWordIdx];
            const int nRandColorOffset = (rand() % (nCurrColorDistThreshold + 1)) - (int)nCurrColorDistThreshold / 2;
            pCurrLocalWord = m_pLocalWordListIter_1ch++;
            pCurrLocalWord->oFeature.anColor[0] = cv::saturate_cast<uchar>((int)pRefLocalWord->oFeature.anColor[0] + nRandColorOffset);
            pCurrLocalWord->oFeature.anDesc[0] = pRefLocalWord->oFeature.anDesc[0];
            pCurrLocalWord->nOccurrences = std::max((size_t)(pRefLocalWord->nOccurrences*((float)(m_nCurrLocalWords - nLocalWordIdx) / m_nCurrLocalWords)), (size_t)1);
            pCurrLocalWord->nFirstOcc = m_nFrameIndex;
            pCurrLocalWord->nLastOcc = m_nFrameIndex;
            m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx] = pCurrLocalWord;
          }
        }
      }
    }
    CV_Assert(m_aLocalWordList_1ch == (m_pLocalWordListIter_1ch - m_nTotRelevantPxCount*m_nCurrLocalWords));
    cv::Mat oGlobalDictPresenceLookupMap(m_oImgSize, CV_8UC1, cv::Scalar_<uchar>(0));
    size_t nPxIterIncr = std::max(m_nTotPxCount / m_nCurrGlobalWords, (size_t)1);
    for (size_t nSamplingPasses = 0; nSamplingPasses < GWORD_DEFAULT_NB_INIT_SAMPL_PASSES; ++nSamplingPasses) {
      for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
        // == refresh: global resampling
        const size_t nPxIter = m_aPxIdxLUT[nModelIter];
        if ((nPxIter%nPxIterIncr) == 0) { // <=(m_nCurrGlobalWords) gwords from (m_nCurrGlobalWords) equally spaced pixels
          if (bForceFGUpdate || !m_oLastFGMask_dilated.data[nPxIter]) {
            const size_t nLocalDictIdx = nModelIter*m_nCurrLocalWords;
            const size_t nGlobalWordMapLookupIdx = m_aPxInfoLUT_PAWCS[nPxIter].nGlobalWordMapLookupIdx;
            const size_t nFloatIter = nPxIter * 4;
            uchar& bCurrRegionIsUnstable = m_oUnstableRegionMask.data[nPxIter];
            const float fCurrDistThresholdFactor = *(float*)(m_oDistThresholdFrame.data + nFloatIter);
            const size_t nCurrColorDistThreshold = (size_t)(sqrt(fCurrDistThresholdFactor)*m_nMinColorDistThreshold) / 2;
            const size_t nCurrDescDistThreshold = ((size_t)1 << ((size_t)floor(fCurrDistThresholdFactor + 0.5f))) + m_nDescDistThresholdOffset + (bCurrRegionIsUnstable*UNSTAB_DESC_DIST_OFFSET);
            CV_Assert(m_apLocalWordDict[nLocalDictIdx]);
            const LocalWord_1ch* pRefBestLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx];
            const float fRefBestLocalWordWeight = GetLocalWordWeight(pRefBestLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
            const uchar nRefBestLocalWordDescBITS = (uchar)popcount(pRefBestLocalWord->oFeature.anDesc[0]);
            bool bFoundUninitd = false;
            size_t nGlobalWordIdx;
            for (nGlobalWordIdx = 0; nGlobalWordIdx < m_nCurrGlobalWords; ++nGlobalWordIdx) {
              GlobalWord_1ch* pCurrGlobalWord = (GlobalWord_1ch*)m_apGlobalWordDict[nGlobalWordIdx];
              if (pCurrGlobalWord
                && L1dist(pCurrGlobalWord->oFeature.anColor[0], pRefBestLocalWord->oFeature.anColor[0]) <= nCurrColorDistThreshold
                && L1dist(nRefBestLocalWordDescBITS, pCurrGlobalWord->nDescBITS) <= nCurrDescDistThreshold / GWORD_DESC_THRES_BITS_MATCH_FACTOR)
                break;
              else if (!pCurrGlobalWord)
                bFoundUninitd = true;
            }
            if (nGlobalWordIdx == m_nCurrGlobalWords) {
              nGlobalWordIdx = m_nCurrGlobalWords - 1;
              GlobalWord_1ch* pCurrGlobalWord = bFoundUninitd ? m_pGlobalWordListIter_1ch++ : (GlobalWord_1ch*)m_apGlobalWordDict[nGlobalWordIdx];
              pCurrGlobalWord->oFeature.anColor[0] = pRefBestLocalWord->oFeature.anColor[0];
              pCurrGlobalWord->oFeature.anDesc[0] = pRefBestLocalWord->oFeature.anDesc[0];
              pCurrGlobalWord->nDescBITS = nRefBestLocalWordDescBITS;
              pCurrGlobalWord->oSpatioOccMap.create(m_oDownSampledFrameSize_GlobalWordLookup, CV_32FC1);
              pCurrGlobalWord->oSpatioOccMap = cv::Scalar(0.0f);
              pCurrGlobalWord->fLatestWeight = 0.0f;
              m_apGlobalWordDict[nGlobalWordIdx] = pCurrGlobalWord;
            }
            float& fCurrGlobalWordLocalWeight = *(float*)(m_apGlobalWordDict[nGlobalWordIdx]->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
            if (fCurrGlobalWordLocalWeight < fRefBestLocalWordWeight) {
              m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight += fRefBestLocalWordWeight;
              fCurrGlobalWordLocalWeight += fRefBestLocalWordWeight;
            }
            oGlobalDictPresenceLookupMap.data[nPxIter] = UCHAR_MAX;
            while (nGlobalWordIdx > 0 && (!m_apGlobalWordDict[nGlobalWordIdx - 1] || m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight > m_apGlobalWordDict[nGlobalWordIdx - 1]->fLatestWeight)) {
              std::swap(m_apGlobalWordDict[nGlobalWordIdx], m_apGlobalWordDict[nGlobalWordIdx - 1]);
              --nGlobalWordIdx;
            }
          }
        }
      }
      nPxIterIncr = std::max(nPxIterIncr / 3, (size_t)1);
    }
    for (size_t nGlobalWordIdx = 0; nGlobalWordIdx < m_nCurrGlobalWords; ++nGlobalWordIdx) {
      GlobalWord_1ch* pCurrGlobalWord = (GlobalWord_1ch*)m_apGlobalWordDict[nGlobalWordIdx];
      if (!pCurrGlobalWord) {
        pCurrGlobalWord = m_pGlobalWordListIter_1ch++;
        pCurrGlobalWord->oFeature.anColor[0] = 0;
        pCurrGlobalWord->oFeature.anDesc[0] = 0;
        pCurrGlobalWord->nDescBITS = 0;
        pCurrGlobalWord->oSpatioOccMap.create(m_oDownSampledFrameSize_GlobalWordLookup, CV_32FC1);
        pCurrGlobalWord->oSpatioOccMap = cv::Scalar(0.0f);
        pCurrGlobalWord->fLatestWeight = 0.0f;
        m_apGlobalWordDict[nGlobalWordIdx] = pCurrGlobalWord;
      }
    }
    CV_Assert((size_t)(m_pGlobalWordListIter_1ch - m_aGlobalWordList_1ch) == m_nCurrGlobalWords && m_aGlobalWordList_1ch == (m_pGlobalWordListIter_1ch - m_nCurrGlobalWords));
  }
  else { //m_nImgChannels==3
    for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
      const size_t nPxIter = m_aPxIdxLUT[nModelIter];
      if (bForceFGUpdate || !m_oLastFGMask_dilated.data[nPxIter]) {
        const size_t nLocalDictIdx = nModelIter*m_nCurrLocalWords;
        const size_t nFloatIter = nPxIter * 4;
        uchar& bCurrRegionIsUnstable = m_oUnstableRegionMask.data[nPxIter];
        const float fCurrDistThresholdFactor = *(float*)(m_oDistThresholdFrame.data + nFloatIter);
        const size_t nCurrTotColorDistThreshold = (size_t)(sqrt(fCurrDistThresholdFactor)*m_nMinColorDistThreshold) * 3;
        const size_t nCurrTotDescDistThreshold = (((size_t)1 << ((size_t)floor(fCurrDistThresholdFactor + 0.5f))) + m_nDescDistThresholdOffset + (bCurrRegionIsUnstable*UNSTAB_DESC_DIST_OFFSET)) * 3;
        // == refresh: local decr
        if (fOccDecrFrac > 0.0f) {
          for (size_t nLocalWordIdx = 0; nLocalWordIdx < m_nCurrLocalWords; ++nLocalWordIdx) {
            LocalWord_3ch* pCurrLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
            if (pCurrLocalWord)
              pCurrLocalWord->nOccurrences -= (size_t)(fOccDecrFrac*pCurrLocalWord->nOccurrences);
          }
        }
        const size_t nCurrWordOccIncr = DEFAULT_LWORD_OCC_INCR;
        const size_t nTotLocalSamplingIterCount = (s_nSamplesInitPatternWidth*s_nSamplesInitPatternHeight) * 2;
        for (size_t nLocalSamplingIter = 0; nLocalSamplingIter < nTotLocalSamplingIterCount; ++nLocalSamplingIter) {
          // == refresh: local resampling
          int nSampleImgCoord_Y, nSampleImgCoord_X;
          getRandSamplePosition(nSampleImgCoord_X, nSampleImgCoord_Y, m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X, m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y, LBSP_::PATCH_SIZE / 2, m_oImgSize);
          const size_t nSamplePxIdx = m_oImgSize.width*nSampleImgCoord_Y + nSampleImgCoord_X;
          if (bForceFGUpdate || !m_oLastFGMask_dilated.data[nSamplePxIdx]) {
            const size_t nSamplePxRGBIdx = nSamplePxIdx * 3;
            const size_t nSampleDescRGBIdx = nSamplePxRGBIdx * 2;
            const uchar* const anSampleColor = m_oLastColorFrame.data + nSamplePxRGBIdx;
            const ushort* const anSampleIntraDesc = ((ushort*)(m_oLastDescFrame.data + nSampleDescRGBIdx));
            bool bFoundUninitd = false;
            size_t nLocalWordIdx;
            for (nLocalWordIdx = 0; nLocalWordIdx < m_nCurrLocalWords; ++nLocalWordIdx) {
              LocalWord_3ch* pCurrLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
              if (pCurrLocalWord
                && cmixdist<3>(anSampleColor, pCurrLocalWord->oFeature.anColor) <= nCurrTotColorDistThreshold
                && hdist<3>(anSampleIntraDesc, pCurrLocalWord->oFeature.anDesc) <= nCurrTotDescDistThreshold) {
                pCurrLocalWord->nOccurrences += nCurrWordOccIncr;
                pCurrLocalWord->nLastOcc = m_nFrameIndex;
                break;
              }
              else if (!pCurrLocalWord)
                bFoundUninitd = true;
            }
            if (nLocalWordIdx == m_nCurrLocalWords) {
              nLocalWordIdx = m_nCurrLocalWords - 1;
              LocalWord_3ch* pCurrLocalWord = bFoundUninitd ? m_pLocalWordListIter_3ch++ : (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
              for (size_t c = 0; c < 3; ++c) {
                pCurrLocalWord->oFeature.anColor[c] = anSampleColor[c];
                pCurrLocalWord->oFeature.anDesc[c] = anSampleIntraDesc[c];
              }
              pCurrLocalWord->nOccurrences = nBaseOccCount;
              pCurrLocalWord->nFirstOcc = m_nFrameIndex;
              pCurrLocalWord->nLastOcc = m_nFrameIndex;
              m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx] = pCurrLocalWord;
            }
            while (nLocalWordIdx > 0 && (!m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1] || GetLocalWordWeight(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_nFrameIndex, m_nLocalWordWeightOffset) > GetLocalWordWeight(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1], m_nFrameIndex, m_nLocalWordWeightOffset))) {
              std::swap(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1]);
              --nLocalWordIdx;
            }
          }
        }
        CV_Assert(m_apLocalWordDict[nLocalDictIdx]);
        for (size_t nLocalWordIdx = 1; nLocalWordIdx < m_nCurrLocalWords; ++nLocalWordIdx) {
          // == refresh: local random resampling
          LocalWord_3ch* pCurrLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
          if (!pCurrLocalWord) {
            const size_t nRandLocalWordIdx = (rand() % nLocalWordIdx);
            const LocalWord_3ch* pRefLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx + nRandLocalWordIdx];
            const int nRandColorOffset = (rand() % (nCurrTotColorDistThreshold / 3 + 1)) - (int)(nCurrTotColorDistThreshold / 6);
            pCurrLocalWord = m_pLocalWordListIter_3ch++;
            for (size_t c = 0; c < 3; ++c) {
              pCurrLocalWord->oFeature.anColor[c] = cv::saturate_cast<uchar>((int)pRefLocalWord->oFeature.anColor[c] + nRandColorOffset);
              pCurrLocalWord->oFeature.anDesc[c] = pRefLocalWord->oFeature.anDesc[c];
            }
            pCurrLocalWord->nOccurrences = std::max((size_t)(pRefLocalWord->nOccurrences*((float)(m_nCurrLocalWords - nLocalWordIdx) / m_nCurrLocalWords)), (size_t)1);
            pCurrLocalWord->nFirstOcc = m_nFrameIndex;
            pCurrLocalWord->nLastOcc = m_nFrameIndex;
            m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx] = pCurrLocalWord;
          }
        }
      }
    }
    CV_Assert(m_aLocalWordList_3ch == (m_pLocalWordListIter_3ch - m_nTotRelevantPxCount*m_nCurrLocalWords));
    cv::Mat oGlobalDictPresenceLookupMap(m_oImgSize, CV_8UC1, cv::Scalar_<uchar>(0));
    size_t nPxIterIncr = std::max(m_nTotPxCount / m_nCurrGlobalWords, (size_t)1);
    for (size_t nSamplingPasses = 0; nSamplingPasses < GWORD_DEFAULT_NB_INIT_SAMPL_PASSES; ++nSamplingPasses) {
      for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
        // == refresh: global resampling
        const size_t nPxIter = m_aPxIdxLUT[nModelIter];
        if ((nPxIter%nPxIterIncr) == 0) { // <=(m_nCurrGlobalWords) gwords from (m_nCurrGlobalWords) equally spaced pixels
          if (bForceFGUpdate || !m_oLastFGMask_dilated.data[nPxIter]) {
            const size_t nLocalDictIdx = nModelIter*m_nCurrLocalWords;
            const size_t nGlobalWordMapLookupIdx = m_aPxInfoLUT_PAWCS[nPxIter].nGlobalWordMapLookupIdx;
            const size_t nFloatIter = nPxIter * 4;
            uchar& bCurrRegionIsUnstable = m_oUnstableRegionMask.data[nPxIter];
            const float fCurrDistThresholdFactor = *(float*)(m_oDistThresholdFrame.data + nFloatIter);
            const size_t nCurrTotColorDistThreshold = (size_t)(sqrt(fCurrDistThresholdFactor)*m_nMinColorDistThreshold) * 3;
            const size_t nCurrTotDescDistThreshold = (((size_t)1 << ((size_t)floor(fCurrDistThresholdFactor + 0.5f))) + m_nDescDistThresholdOffset + (bCurrRegionIsUnstable*UNSTAB_DESC_DIST_OFFSET)) * 3;
            CV_Assert(m_apLocalWordDict[nLocalDictIdx]);
            const LocalWord_3ch* pRefBestLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx];
            const float fRefBestLocalWordWeight = GetLocalWordWeight(pRefBestLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
            const uchar nRefBestLocalWordDescBITS = (uchar)popcount<3>(pRefBestLocalWord->oFeature.anDesc);
            bool bFoundUninitd = false;
            size_t nGlobalWordIdx;
            for (nGlobalWordIdx = 0; nGlobalWordIdx < m_nCurrGlobalWords; ++nGlobalWordIdx) {
              GlobalWord_3ch* pCurrGlobalWord = (GlobalWord_3ch*)m_apGlobalWordDict[nGlobalWordIdx];
              if (pCurrGlobalWord
                && L1dist(nRefBestLocalWordDescBITS, pCurrGlobalWord->nDescBITS) <= nCurrTotDescDistThreshold / GWORD_DESC_THRES_BITS_MATCH_FACTOR
                && cmixdist<3>(pRefBestLocalWord->oFeature.anColor, pCurrGlobalWord->oFeature.anColor) <= nCurrTotColorDistThreshold)
                break;
              else if (!pCurrGlobalWord)
                bFoundUninitd = true;
            }
            if (nGlobalWordIdx == m_nCurrGlobalWords) {
              nGlobalWordIdx = m_nCurrGlobalWords - 1;
              GlobalWord_3ch* pCurrGlobalWord = bFoundUninitd ? m_pGlobalWordListIter_3ch++ : (GlobalWord_3ch*)m_apGlobalWordDict[nGlobalWordIdx];
              for (size_t c = 0; c < 3; ++c) {
                pCurrGlobalWord->oFeature.anColor[c] = pRefBestLocalWord->oFeature.anColor[c];
                pCurrGlobalWord->oFeature.anDesc[c] = pRefBestLocalWord->oFeature.anDesc[c];
              }
              pCurrGlobalWord->nDescBITS = nRefBestLocalWordDescBITS;
              pCurrGlobalWord->oSpatioOccMap.create(m_oDownSampledFrameSize_GlobalWordLookup, CV_32FC1);
              pCurrGlobalWord->oSpatioOccMap = cv::Scalar(0.0f);
              pCurrGlobalWord->fLatestWeight = 0.0f;
              m_apGlobalWordDict[nGlobalWordIdx] = pCurrGlobalWord;
            }
            float& fCurrGlobalWordLocalWeight = *(float*)(m_apGlobalWordDict[nGlobalWordIdx]->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
            if (fCurrGlobalWordLocalWeight < fRefBestLocalWordWeight) {
              m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight += fRefBestLocalWordWeight;
              fCurrGlobalWordLocalWeight += fRefBestLocalWordWeight;
            }
            oGlobalDictPresenceLookupMap.data[nPxIter] = UCHAR_MAX;
            while (nGlobalWordIdx > 0 && (!m_apGlobalWordDict[nGlobalWordIdx - 1] || m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight > m_apGlobalWordDict[nGlobalWordIdx - 1]->fLatestWeight)) {
              std::swap(m_apGlobalWordDict[nGlobalWordIdx], m_apGlobalWordDict[nGlobalWordIdx - 1]);
              --nGlobalWordIdx;
            }
          }
        }
      }
      nPxIterIncr = std::max(nPxIterIncr / 3, (size_t)1);
    }
    for (size_t nGlobalWordIdx = 0; nGlobalWordIdx < m_nCurrGlobalWords; ++nGlobalWordIdx) {
      GlobalWord_3ch* pCurrGlobalWord = (GlobalWord_3ch*)m_apGlobalWordDict[nGlobalWordIdx];
      if (!pCurrGlobalWord) {
        pCurrGlobalWord = m_pGlobalWordListIter_3ch++;
        for (size_t c = 0; c < 3; ++c) {
          pCurrGlobalWord->oFeature.anColor[c] = 0;
          pCurrGlobalWord->oFeature.anDesc[c] = 0;
        }
        pCurrGlobalWord->nDescBITS = 0;
        pCurrGlobalWord->oSpatioOccMap.create(m_oDownSampledFrameSize_GlobalWordLookup, CV_32FC1);
        pCurrGlobalWord->oSpatioOccMap = cv::Scalar(0.0f);
        pCurrGlobalWord->fLatestWeight = 0.0f;
        m_apGlobalWordDict[nGlobalWordIdx] = pCurrGlobalWord;
      }
    }
    CV_Assert((size_t)(m_pGlobalWordListIter_3ch - m_aGlobalWordList_3ch) == m_nCurrGlobalWords && m_aGlobalWordList_3ch == (m_pGlobalWordListIter_3ch - m_nCurrGlobalWords));
  }
  for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
    // == refresh: per-px global word sort
    const size_t nPxIter = m_aPxIdxLUT[nModelIter];
    const size_t nGlobalWordMapLookupIdx = m_aPxInfoLUT_PAWCS[nPxIter].nGlobalWordMapLookupIdx;
    float fLastGlobalWordLocalWeight = *(float*)(m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[0]->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
    for (size_t nGlobalWordLUTIdx = 1; nGlobalWordLUTIdx < m_nCurrGlobalWords; ++nGlobalWordLUTIdx) {
      const float fCurrGlobalWordLocalWeight = *(float*)(m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx]->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
      if (fCurrGlobalWordLocalWeight > fLastGlobalWordLocalWeight)
        std::swap(m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx], m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx - 1]);
      else
        fLastGlobalWordLocalWeight = fCurrGlobalWordLocalWeight;
    }
  }
}

void BackgroundSubtractorPAWCS::apply(cv::InputArray _image, cv::OutputArray _fgmask, double learningRateOverride) {
  // == process
  CV_Assert(m_bInitialized);
  cv::Mat oInputImg = _image.getMat();
  CV_Assert(oInputImg.type() == m_nImgType && oInputImg.size() == m_oImgSize);
  CV_Assert(oInputImg.isContinuous());
  _fgmask.create(m_oImgSize, CV_8UC1);
  cv::Mat oCurrFGMask = _fgmask.getMat();
  memset(oCurrFGMask.data, 0, oCurrFGMask.cols*oCurrFGMask.rows);
  const bool bBootstrapping = ++m_nFrameIndex <= DEFAULT_BOOTSTRAP_WIN_SIZE;
  const size_t nCurrSamplesForMovingAvg_LT = bBootstrapping ? m_nSamplesForMovingAvgs / 2 : m_nSamplesForMovingAvgs;
  const size_t nCurrSamplesForMovingAvg_ST = nCurrSamplesForMovingAvg_LT / 4;
  const float fRollAvgFactor_LT = 1.0f / std::min(m_nFrameIndex, nCurrSamplesForMovingAvg_LT);
  const float fRollAvgFactor_ST = 1.0f / std::min(m_nFrameIndex, nCurrSamplesForMovingAvg_ST);
  const size_t nCurrGlobalWordUpdateRate = bBootstrapping ? DEFAULT_RESAMPLING_RATE / 2 : DEFAULT_RESAMPLING_RATE;
  size_t nFlatRegionCount = 0;
  if (m_nImgChannels == 1) {
    for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
      const size_t nPxIter = m_aPxIdxLUT[nModelIter];
      const size_t nDescIter = nPxIter * 2;
      const size_t nFloatIter = nPxIter * 4;
      const size_t nLocalDictIdx = nModelIter*m_nCurrLocalWords;
      const size_t nGlobalWordMapLookupIdx = m_aPxInfoLUT_PAWCS[nPxIter].nGlobalWordMapLookupIdx;
      const uchar nCurrColor = oInputImg.data[nPxIter];
      uchar& nLastColor = m_oLastColorFrame.data[nPxIter];
      ushort& nLastIntraDesc = *((ushort*)(m_oLastDescFrame.data + nDescIter));
      size_t nMinColorDist = s_nColorMaxDataRange_1ch;
      size_t nMinDescDist = s_nDescMaxDataRange_1ch;
      float& fCurrMeanRawSegmRes_LT = *(float*)(m_oMeanRawSegmResFrame_LT.data + nFloatIter);
      float& fCurrMeanRawSegmRes_ST = *(float*)(m_oMeanRawSegmResFrame_ST.data + nFloatIter);
      float& fCurrMeanFinalSegmRes_LT = *(float*)(m_oMeanFinalSegmResFrame_LT.data + nFloatIter);
      float& fCurrMeanFinalSegmRes_ST = *(float*)(m_oMeanFinalSegmResFrame_ST.data + nFloatIter);
      float& fCurrDistThresholdFactor = *(float*)(m_oDistThresholdFrame.data + nFloatIter);
      float& fCurrDistThresholdVariationFactor = *(float*)(m_oDistThresholdVariationFrame.data + nFloatIter);
      float& fCurrLearningRate = *(float*)(m_oUpdateRateFrame.data + nFloatIter);
      float& fCurrMeanMinDist_LT = *(float*)(m_oMeanMinDistFrame_LT.data + nFloatIter);
      float& fCurrMeanMinDist_ST = *(float*)(m_oMeanMinDistFrame_ST.data + nFloatIter);
      const float fBestLocalWordWeight = GetLocalWordWeight(m_apLocalWordDict[nLocalDictIdx], m_nFrameIndex, m_nLocalWordWeightOffset);
      const float fLocalWordsWeightSumThreshold = fBestLocalWordWeight / (fCurrDistThresholdFactor * 2);
      uchar& bCurrRegionIsUnstable = m_oUnstableRegionMask.data[nPxIter];
      uchar& nCurrRegionIllumUpdtVal = m_oIllumUpdtRegionMask.data[nPxIter];
      uchar& nCurrRegionSegmVal = oCurrFGMask.data[nPxIter];
      const bool bCurrRegionIsROIBorder = m_oROI.data[nPxIter] < UCHAR_MAX;
      const int nCurrImgCoord_X = m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X;
      const int nCurrImgCoord_Y = m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y;
      ushort nCurrInterDesc, nCurrIntraDesc;
      LBSP_::computeGrayscaleDescriptor(oInputImg, nCurrColor, nCurrImgCoord_X, nCurrImgCoord_Y, m_anLBSPThreshold_8bitLUT[nCurrColor], nCurrIntraDesc);
      const uchar nCurrIntraDescBITS = (uchar)popcount(nCurrIntraDesc);
      const bool bCurrRegionIsFlat = nCurrIntraDescBITS < FLAT_REGION_BIT_COUNT;
      if (bCurrRegionIsFlat)
        ++nFlatRegionCount;
      const size_t nCurrWordOccIncr = (DEFAULT_LWORD_OCC_INCR + m_nModelResetCooldown) << int(bCurrRegionIsFlat || bBootstrapping);
      const size_t nCurrLocalWordUpdateRate = learningRateOverride > 0 ? (size_t)ceil(learningRateOverride) : bCurrRegionIsFlat ? (size_t)ceil(fCurrLearningRate + FEEDBACK_T_LOWER) / 2 : (size_t)ceil(fCurrLearningRate);
      const size_t nCurrColorDistThreshold = (size_t)(sqrt(fCurrDistThresholdFactor)*m_nMinColorDistThreshold) / 2;
      const size_t nCurrDescDistThreshold = ((size_t)1 << ((size_t)floor(fCurrDistThresholdFactor + 0.5f))) + m_nDescDistThresholdOffset + (bCurrRegionIsUnstable*UNSTAB_DESC_DIST_OFFSET);
      size_t nLocalWordIdx = 0;
      float fPotentialLocalWordsWeightSum = 0.0f;
      float fLastLocalWordWeight = FLT_MAX;
      while (nLocalWordIdx < m_nCurrLocalWords && fPotentialLocalWordsWeightSum < fLocalWordsWeightSumThreshold) {
        LocalWord_1ch* pCurrLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
        const float fCurrLocalWordWeight = GetLocalWordWeight(pCurrLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
        {
          const size_t nColorDist = L1dist(nCurrColor, pCurrLocalWord->oFeature.anColor[0]);
          const size_t nIntraDescDist = hdist(nCurrIntraDesc, pCurrLocalWord->oFeature.anDesc[0]);
          LBSP_::computeGrayscaleDescriptor(oInputImg, pCurrLocalWord->oFeature.anColor[0], nCurrImgCoord_X, nCurrImgCoord_Y, m_anLBSPThreshold_8bitLUT[pCurrLocalWord->oFeature.anColor[0]], nCurrInterDesc);
          const size_t nInterDescDist = hdist(nCurrInterDesc, pCurrLocalWord->oFeature.anDesc[0]);
          const size_t nDescDist = (nIntraDescDist + nInterDescDist) / 2;
          if ((!bCurrRegionIsUnstable || bCurrRegionIsFlat || bCurrRegionIsROIBorder)
            && nColorDist <= nCurrColorDistThreshold
            && nColorDist >= nCurrColorDistThreshold / 2
            && nIntraDescDist <= nCurrDescDistThreshold / 2
            && (rand() % (nCurrRegionIllumUpdtVal ? (nCurrLocalWordUpdateRate / 2 + 1) : nCurrLocalWordUpdateRate)) == 0) {
            // == illum updt
            pCurrLocalWord->oFeature.anColor[0] = nCurrColor;
            pCurrLocalWord->oFeature.anDesc[0] = nCurrIntraDesc;
            m_oIllumUpdtRegionMask.data[nPxIter - 1] = 1 & m_oROI.data[nPxIter - 1];
            m_oIllumUpdtRegionMask.data[nPxIter + 1] = 1 & m_oROI.data[nPxIter + 1];
            m_oIllumUpdtRegionMask.data[nPxIter] = 2;
          }
          if (nDescDist <= nCurrDescDistThreshold && nColorDist <= nCurrColorDistThreshold) {
            fPotentialLocalWordsWeightSum += fCurrLocalWordWeight;
            pCurrLocalWord->nLastOcc = m_nFrameIndex;
            if ((!m_oLastFGMask.data[nPxIter] || m_bUsingMovingCamera) && fCurrLocalWordWeight < DEFAULT_LWORD_MAX_WEIGHT)
              pCurrLocalWord->nOccurrences += nCurrWordOccIncr;
            nMinColorDist = std::min(nMinColorDist, nColorDist);
            nMinDescDist = std::min(nMinDescDist, nDescDist);
          }
        }
        if (fCurrLocalWordWeight > fLastLocalWordWeight) {
          std::swap(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1]);
        }
        else
          fLastLocalWordWeight = fCurrLocalWordWeight;
        ++nLocalWordIdx;
      }
      while (nLocalWordIdx < m_nCurrLocalWords) {
        const float fCurrLocalWordWeight = GetLocalWordWeight(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_nFrameIndex, m_nLocalWordWeightOffset);
        if (fCurrLocalWordWeight > fLastLocalWordWeight) {
          std::swap(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1]);
        }
        else
          fLastLocalWordWeight = fCurrLocalWordWeight;
        ++nLocalWordIdx;
      }
      if (fPotentialLocalWordsWeightSum >= fLocalWordsWeightSumThreshold || bCurrRegionIsROIBorder) {
        // == background
        const float fNormalizedMinDist = std::max((float)nMinColorDist / s_nColorMaxDataRange_1ch, (float)nMinDescDist / s_nDescMaxDataRange_1ch);
        fCurrMeanMinDist_LT = fCurrMeanMinDist_LT*(1.0f - fRollAvgFactor_LT) + fNormalizedMinDist*fRollAvgFactor_LT;
        fCurrMeanMinDist_ST = fCurrMeanMinDist_ST*(1.0f - fRollAvgFactor_ST) + fNormalizedMinDist*fRollAvgFactor_ST;
        fCurrMeanRawSegmRes_LT = fCurrMeanRawSegmRes_LT*(1.0f - fRollAvgFactor_LT);
        fCurrMeanRawSegmRes_ST = fCurrMeanRawSegmRes_ST*(1.0f - fRollAvgFactor_ST);
        if ((rand() % nCurrLocalWordUpdateRate) == 0) {
          size_t nGlobalWordLUTIdx;
          GlobalWord_1ch* pCurrGlobalWord = nullptr;
          for (nGlobalWordLUTIdx = 0; nGlobalWordLUTIdx < m_nCurrGlobalWords; ++nGlobalWordLUTIdx) {
            pCurrGlobalWord = (GlobalWord_1ch*)m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx];
            if (L1dist(pCurrGlobalWord->oFeature.anColor[0], nCurrColor) <= nCurrColorDistThreshold
              && L1dist(nCurrIntraDescBITS, pCurrGlobalWord->nDescBITS) <= nCurrDescDistThreshold / GWORD_DESC_THRES_BITS_MATCH_FACTOR)
              break;
          }
          if (nGlobalWordLUTIdx != m_nCurrGlobalWords || (rand() % (nCurrLocalWordUpdateRate * 2)) == 0) {
            if (nGlobalWordLUTIdx == m_nCurrGlobalWords) {
              pCurrGlobalWord = (GlobalWord_1ch*)m_apGlobalWordDict[m_nCurrGlobalWords - 1];
              pCurrGlobalWord->oFeature.anColor[0] = nCurrColor;
              pCurrGlobalWord->oFeature.anDesc[0] = nCurrIntraDesc;
              pCurrGlobalWord->nDescBITS = nCurrIntraDescBITS;
              pCurrGlobalWord->oSpatioOccMap = cv::Scalar(0.0f);
              pCurrGlobalWord->fLatestWeight = 0.0f;
            }
            float& fCurrGlobalWordLocalWeight = *(float*)(pCurrGlobalWord->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
            if (fCurrGlobalWordLocalWeight < fPotentialLocalWordsWeightSum) {
              pCurrGlobalWord->fLatestWeight += fPotentialLocalWordsWeightSum;
              fCurrGlobalWordLocalWeight += fPotentialLocalWordsWeightSum;
            }
          }
        }
      }
      else {
        // == foreground
        const float fNormalizedMinDist = std::max(std::max((float)nMinColorDist / s_nColorMaxDataRange_1ch, (float)nMinDescDist / s_nDescMaxDataRange_1ch), (fLocalWordsWeightSumThreshold - fPotentialLocalWordsWeightSum) / fLocalWordsWeightSumThreshold);
        fCurrMeanMinDist_LT = fCurrMeanMinDist_LT*(1.0f - fRollAvgFactor_LT) + fNormalizedMinDist*fRollAvgFactor_LT;
        fCurrMeanMinDist_ST = fCurrMeanMinDist_ST*(1.0f - fRollAvgFactor_ST) + fNormalizedMinDist*fRollAvgFactor_ST;
        fCurrMeanRawSegmRes_LT = fCurrMeanRawSegmRes_LT*(1.0f - fRollAvgFactor_LT) + fRollAvgFactor_LT;
        fCurrMeanRawSegmRes_ST = fCurrMeanRawSegmRes_ST*(1.0f - fRollAvgFactor_ST) + fRollAvgFactor_ST;
        if (bCurrRegionIsFlat || (rand() % nCurrLocalWordUpdateRate) == 0) {
          size_t nGlobalWordLUTIdx;
          GlobalWord_1ch* pCurrGlobalWord;
          for (nGlobalWordLUTIdx = 0; nGlobalWordLUTIdx < m_nCurrGlobalWords; ++nGlobalWordLUTIdx) {
            pCurrGlobalWord = (GlobalWord_1ch*)m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx];
            if (L1dist(pCurrGlobalWord->oFeature.anColor[0], nCurrColor) <= nCurrColorDistThreshold
              && L1dist(nCurrIntraDescBITS, pCurrGlobalWord->nDescBITS) <= nCurrDescDistThreshold / GWORD_DESC_THRES_BITS_MATCH_FACTOR)
              break;
          }
          if (nGlobalWordLUTIdx == m_nCurrGlobalWords)
            nCurrRegionSegmVal = UCHAR_MAX;
          else {
            const float fGlobalWordLocalizedWeight = *(float*)(pCurrGlobalWord->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
            if (fPotentialLocalWordsWeightSum + fGlobalWordLocalizedWeight / (bCurrRegionIsFlat ? 2 : 4) < fLocalWordsWeightSumThreshold)
              nCurrRegionSegmVal = UCHAR_MAX;
          }
        }
        else
          nCurrRegionSegmVal = UCHAR_MAX;
        if (fPotentialLocalWordsWeightSum < DEFAULT_LWORD_INIT_WEIGHT) {
          const size_t nNewLocalWordIdx = m_nCurrLocalWords - 1;
          LocalWord_1ch* pNewLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx + nNewLocalWordIdx];
          pNewLocalWord->oFeature.anColor[0] = nCurrColor;
          pNewLocalWord->oFeature.anDesc[0] = nCurrIntraDesc;
          pNewLocalWord->nOccurrences = nCurrWordOccIncr;
          pNewLocalWord->nFirstOcc = m_nFrameIndex;
          pNewLocalWord->nLastOcc = m_nFrameIndex;
        }
      }
      // == neighb updt
      if ((!nCurrRegionSegmVal && (rand() % nCurrLocalWordUpdateRate) == 0) || bCurrRegionIsROIBorder || m_bUsingMovingCamera) {
        //if((!nCurrRegionSegmVal && (rand()%(nCurrRegionIllumUpdtVal?(nCurrLocalWordUpdateRate/2+1):nCurrLocalWordUpdateRate))==0) || bCurrRegionIsROIBorder) {
        int nSampleImgCoord_Y, nSampleImgCoord_X;
        if (bCurrRegionIsFlat || bCurrRegionIsROIBorder || m_bUsingMovingCamera)
          getRandNeighborPosition_5x5(nSampleImgCoord_X, nSampleImgCoord_Y, nCurrImgCoord_X, nCurrImgCoord_Y, LBSP_::PATCH_SIZE / 2, m_oImgSize);
        else
          getRandNeighborPosition_3x3(nSampleImgCoord_X, nSampleImgCoord_Y, nCurrImgCoord_X, nCurrImgCoord_Y, LBSP_::PATCH_SIZE / 2, m_oImgSize);
        const size_t nSamplePxIdx = m_oImgSize.width*nSampleImgCoord_Y + nSampleImgCoord_X;
        if (m_oROI.data[nSamplePxIdx]) {
          const size_t nNeighborLocalDictIdx = m_aPxInfoLUT_PAWCS[nSamplePxIdx].nModelIdx*m_nCurrLocalWords;
          size_t nNeighborLocalWordIdx = 0;
          float fNeighborPotentialLocalWordsWeightSum = 0.0f;
          while (nNeighborLocalWordIdx < m_nCurrLocalWords && fNeighborPotentialLocalWordsWeightSum < fLocalWordsWeightSumThreshold) {
            LocalWord_1ch* pNeighborLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nNeighborLocalDictIdx + nNeighborLocalWordIdx];
            const size_t nNeighborColorDist = L1dist(nCurrColor, pNeighborLocalWord->oFeature.anColor[0]);
            const size_t nNeighborIntraDescDist = hdist(nCurrIntraDesc, pNeighborLocalWord->oFeature.anDesc[0]);
            const bool bNeighborRegionIsFlat = popcount(pNeighborLocalWord->oFeature.anDesc[0]) < FLAT_REGION_BIT_COUNT;
            const size_t nNeighborWordOccIncr = bNeighborRegionIsFlat ? nCurrWordOccIncr * 2 : nCurrWordOccIncr;
            if (nNeighborColorDist <= nCurrColorDistThreshold && nNeighborIntraDescDist <= nCurrDescDistThreshold) {
              const float fNeighborLocalWordWeight = GetLocalWordWeight(pNeighborLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
              fNeighborPotentialLocalWordsWeightSum += fNeighborLocalWordWeight;
              pNeighborLocalWord->nLastOcc = m_nFrameIndex;
              if (fNeighborLocalWordWeight < DEFAULT_LWORD_MAX_WEIGHT)
                pNeighborLocalWord->nOccurrences += nNeighborWordOccIncr;
            }
            else if (!oCurrFGMask.data[nSamplePxIdx] && bCurrRegionIsFlat && (bBootstrapping || (rand() % nCurrLocalWordUpdateRate) == 0)) {
              const size_t nSampleDescIdx = nSamplePxIdx * 2;
              ushort& nNeighborLastIntraDesc = *((ushort*)(m_oLastDescFrame.data + nSampleDescIdx));
              const size_t nNeighborLastIntraDescDist = hdist(nCurrIntraDesc, nNeighborLastIntraDesc);
              if (nNeighborColorDist <= nCurrColorDistThreshold && nNeighborLastIntraDescDist <= nCurrDescDistThreshold / 2) {
                const float fNeighborLocalWordWeight = GetLocalWordWeight(pNeighborLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
                fNeighborPotentialLocalWordsWeightSum += fNeighborLocalWordWeight;
                pNeighborLocalWord->nLastOcc = m_nFrameIndex;
                if (fNeighborLocalWordWeight < DEFAULT_LWORD_MAX_WEIGHT)
                  pNeighborLocalWord->nOccurrences += nNeighborWordOccIncr;
                pNeighborLocalWord->oFeature.anDesc[0] = nCurrIntraDesc;
              }
            }
            ++nNeighborLocalWordIdx;
          }
          if (fNeighborPotentialLocalWordsWeightSum < DEFAULT_LWORD_INIT_WEIGHT) {
            nNeighborLocalWordIdx = m_nCurrLocalWords - 1;
            LocalWord_1ch* pNeighborLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nNeighborLocalDictIdx + nNeighborLocalWordIdx];
            pNeighborLocalWord->oFeature.anColor[0] = nCurrColor;
            pNeighborLocalWord->oFeature.anDesc[0] = nCurrIntraDesc;
            pNeighborLocalWord->nOccurrences = nCurrWordOccIncr;
            pNeighborLocalWord->nFirstOcc = m_nFrameIndex;
            pNeighborLocalWord->nLastOcc = m_nFrameIndex;
          }
        }
      }
      if (nCurrRegionIllumUpdtVal)
        nCurrRegionIllumUpdtVal -= 1;
      // == feedback adj
      bCurrRegionIsUnstable = fCurrDistThresholdFactor > UNSTABLE_REG_RDIST_MIN || (fCurrMeanRawSegmRes_LT - fCurrMeanFinalSegmRes_LT) > UNSTABLE_REG_RATIO_MIN || (fCurrMeanRawSegmRes_ST - fCurrMeanFinalSegmRes_ST) > UNSTABLE_REG_RATIO_MIN;
      if (m_oLastFGMask.data[nPxIter] || (std::min(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST) < UNSTABLE_REG_RATIO_MIN && nCurrRegionSegmVal))
        fCurrLearningRate = std::min(fCurrLearningRate + FEEDBACK_T_INCR / (std::max(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST)*fCurrDistThresholdVariationFactor), FEEDBACK_T_UPPER);
      else
        fCurrLearningRate = std::max(fCurrLearningRate - FEEDBACK_T_DECR*fCurrDistThresholdVariationFactor / std::max(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST), FEEDBACK_T_LOWER);
      if (std::max(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST) > UNSTABLE_REG_RATIO_MIN && m_oBlinksFrame.data[nPxIter])
        (fCurrDistThresholdVariationFactor) += bBootstrapping ? FEEDBACK_V_INCR * 2 : FEEDBACK_V_INCR;
      else
        fCurrDistThresholdVariationFactor = std::max(fCurrDistThresholdVariationFactor - FEEDBACK_V_DECR*((bBootstrapping || bCurrRegionIsFlat) ? 2 : m_oLastFGMask.data[nPxIter] ? 0.5f : 1), FEEDBACK_V_DECR);
      if (fCurrDistThresholdFactor < std::pow(1.0f + std::min(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST) * 2, 2))
        fCurrDistThresholdFactor += FEEDBACK_R_VAR*(fCurrDistThresholdVariationFactor - FEEDBACK_V_DECR);
      else
        fCurrDistThresholdFactor = std::max(fCurrDistThresholdFactor - FEEDBACK_R_VAR / fCurrDistThresholdVariationFactor, 1.0f);
      nLastIntraDesc = nCurrIntraDesc;
      nLastColor = nCurrColor;
    }
  }
  else { //m_nImgChannels==3
    for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
      const size_t nPxIter = m_aPxIdxLUT[nModelIter];
      const size_t nPxRGBIter = nPxIter * 3;
      const size_t nDescRGBIter = nPxRGBIter * 2;
      const size_t nFloatIter = nPxIter * 4;
      const size_t nLocalDictIdx = nModelIter*m_nCurrLocalWords;
      const size_t nGlobalWordMapLookupIdx = m_aPxInfoLUT_PAWCS[nPxIter].nGlobalWordMapLookupIdx;
      const uchar* const anCurrColor = oInputImg.data + nPxRGBIter;
      uchar* anLastColor = m_oLastColorFrame.data + nPxRGBIter;
      ushort* anLastIntraDesc = ((ushort*)(m_oLastDescFrame.data + nDescRGBIter));
      size_t nMinTotColorDist = s_nColorMaxDataRange_3ch;
      size_t nMinTotDescDist = s_nDescMaxDataRange_3ch;
      float& fCurrMeanRawSegmRes_LT = *(float*)(m_oMeanRawSegmResFrame_LT.data + nFloatIter);
      float& fCurrMeanRawSegmRes_ST = *(float*)(m_oMeanRawSegmResFrame_ST.data + nFloatIter);
      float& fCurrMeanFinalSegmRes_LT = *(float*)(m_oMeanFinalSegmResFrame_LT.data + nFloatIter);
      float& fCurrMeanFinalSegmRes_ST = *(float*)(m_oMeanFinalSegmResFrame_ST.data + nFloatIter);
      float& fCurrDistThresholdFactor = *(float*)(m_oDistThresholdFrame.data + nFloatIter);
      float& fCurrDistThresholdVariationFactor = *(float*)(m_oDistThresholdVariationFrame.data + nFloatIter);
      float& fCurrLearningRate = *(float*)(m_oUpdateRateFrame.data + nFloatIter);
      float& fCurrMeanMinDist_LT = *(float*)(m_oMeanMinDistFrame_LT.data + nFloatIter);
      float& fCurrMeanMinDist_ST = *(float*)(m_oMeanMinDistFrame_ST.data + nFloatIter);
      const float fBestLocalWordWeight = GetLocalWordWeight(m_apLocalWordDict[nLocalDictIdx], m_nFrameIndex, m_nLocalWordWeightOffset);
      const float fLocalWordsWeightSumThreshold = fBestLocalWordWeight / (fCurrDistThresholdFactor * 2);
      uchar& bCurrRegionIsUnstable = m_oUnstableRegionMask.data[nPxIter];
      uchar& nCurrRegionIllumUpdtVal = m_oIllumUpdtRegionMask.data[nPxIter];
      uchar& nCurrRegionSegmVal = oCurrFGMask.data[nPxIter];
      const bool bCurrRegionIsROIBorder = m_oROI.data[nPxIter] < UCHAR_MAX;
      const int nCurrImgCoord_X = m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X;
      const int nCurrImgCoord_Y = m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y;
      ushort anCurrInterDesc[3], anCurrIntraDesc[3];
      const size_t anCurrIntraLBSPThresholds[3] = { m_anLBSPThreshold_8bitLUT[anCurrColor[0]],m_anLBSPThreshold_8bitLUT[anCurrColor[1]],m_anLBSPThreshold_8bitLUT[anCurrColor[2]] };
      LBSP_::computeRGBDescriptor(oInputImg, anCurrColor, nCurrImgCoord_X, nCurrImgCoord_Y, anCurrIntraLBSPThresholds, anCurrIntraDesc);
      const uchar nCurrIntraDescBITS = (uchar)popcount<3>(anCurrIntraDesc);
      const bool bCurrRegionIsFlat = nCurrIntraDescBITS < FLAT_REGION_BIT_COUNT * 2;
      if (bCurrRegionIsFlat)
        ++nFlatRegionCount;
      const size_t nCurrWordOccIncr = (DEFAULT_LWORD_OCC_INCR + m_nModelResetCooldown) << int(bCurrRegionIsFlat || bBootstrapping);
      const size_t nCurrLocalWordUpdateRate = learningRateOverride > 0 ? (size_t)ceil(learningRateOverride) : bCurrRegionIsFlat ? (size_t)ceil(fCurrLearningRate + FEEDBACK_T_LOWER) / 2 : (size_t)ceil(fCurrLearningRate);
      const size_t nCurrTotColorDistThreshold = (size_t)(sqrt(fCurrDistThresholdFactor)*m_nMinColorDistThreshold) * 3;
      const size_t nCurrTotDescDistThreshold = (((size_t)1 << ((size_t)floor(fCurrDistThresholdFactor + 0.5f))) + m_nDescDistThresholdOffset + (bCurrRegionIsUnstable*UNSTAB_DESC_DIST_OFFSET)) * 3;
      size_t nLocalWordIdx = 0;
      float fPotentialLocalWordsWeightSum = 0.0f;
      float fLastLocalWordWeight = FLT_MAX;
      while (nLocalWordIdx < m_nCurrLocalWords && fPotentialLocalWordsWeightSum < fLocalWordsWeightSumThreshold) {
        LocalWord_3ch* pCurrLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
        const float fCurrLocalWordWeight = GetLocalWordWeight(pCurrLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
        {
          const size_t nTotColorL1Dist = L1dist<3>(anCurrColor, pCurrLocalWord->oFeature.anColor);
          const size_t nColorDistortion = cdist<3>(anCurrColor, pCurrLocalWord->oFeature.anColor);
          const size_t nTotColorMixDist = cmixdist(nTotColorL1Dist, nColorDistortion);
          const size_t nTotIntraDescDist = hdist<3>(anCurrIntraDesc, pCurrLocalWord->oFeature.anDesc);
          const size_t anCurrInterLBSPThresholds[3] = { m_anLBSPThreshold_8bitLUT[pCurrLocalWord->oFeature.anColor[0]],m_anLBSPThreshold_8bitLUT[pCurrLocalWord->oFeature.anColor[1]],m_anLBSPThreshold_8bitLUT[pCurrLocalWord->oFeature.anColor[2]] };
          LBSP_::computeRGBDescriptor(oInputImg, pCurrLocalWord->oFeature.anColor, nCurrImgCoord_X, nCurrImgCoord_Y, anCurrInterLBSPThresholds, anCurrInterDesc);
          const size_t nTotInterDescDist = hdist<3>(anCurrInterDesc, pCurrLocalWord->oFeature.anDesc);
          const size_t nTotDescDist = (nTotIntraDescDist + nTotInterDescDist) / 2;
          if ((!bCurrRegionIsUnstable || bCurrRegionIsFlat || bCurrRegionIsROIBorder)
            && nTotColorMixDist <= nCurrTotColorDistThreshold
            && nTotColorL1Dist >= nCurrTotColorDistThreshold / 2
            && nTotIntraDescDist <= nCurrTotDescDistThreshold / 2
            && (rand() % (nCurrRegionIllumUpdtVal ? (nCurrLocalWordUpdateRate / 2 + 1) : nCurrLocalWordUpdateRate)) == 0) {
            // == illum updt
            for (size_t c = 0; c < 3; ++c) {
              pCurrLocalWord->oFeature.anColor[c] = anCurrColor[c];
              pCurrLocalWord->oFeature.anDesc[c] = anCurrIntraDesc[c];
            }
            m_oIllumUpdtRegionMask.data[nPxIter - 1] = 1 & m_oROI.data[nPxIter - 1];
            m_oIllumUpdtRegionMask.data[nPxIter + 1] = 1 & m_oROI.data[nPxIter + 1];
            m_oIllumUpdtRegionMask.data[nPxIter] = 2;
          }
          if (nTotDescDist <= nCurrTotDescDistThreshold && nTotColorMixDist <= nCurrTotColorDistThreshold) {
            fPotentialLocalWordsWeightSum += fCurrLocalWordWeight;
            pCurrLocalWord->nLastOcc = m_nFrameIndex;
            if ((!m_oLastFGMask.data[nPxIter] || m_bUsingMovingCamera) && fCurrLocalWordWeight < DEFAULT_LWORD_MAX_WEIGHT)
              pCurrLocalWord->nOccurrences += nCurrWordOccIncr;
            nMinTotColorDist = std::min(nMinTotColorDist, nTotColorMixDist);
            nMinTotDescDist = std::min(nMinTotDescDist, nTotDescDist);
          }
        }
        if (fCurrLocalWordWeight > fLastLocalWordWeight) {
          std::swap(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1]);
        }
        else
          fLastLocalWordWeight = fCurrLocalWordWeight;
        ++nLocalWordIdx;
      }
      while (nLocalWordIdx < m_nCurrLocalWords) {
        const float fCurrLocalWordWeight = GetLocalWordWeight(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_nFrameIndex, m_nLocalWordWeightOffset);
        if (fCurrLocalWordWeight > fLastLocalWordWeight) {
          std::swap(m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx], m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx - 1]);
        }
        else
          fLastLocalWordWeight = fCurrLocalWordWeight;
        ++nLocalWordIdx;
      }
      if (fPotentialLocalWordsWeightSum >= fLocalWordsWeightSumThreshold || bCurrRegionIsROIBorder) {
        // == background
        const float fNormalizedMinDist = std::max((float)nMinTotColorDist / s_nColorMaxDataRange_3ch, (float)nMinTotDescDist / s_nDescMaxDataRange_3ch);
        fCurrMeanMinDist_LT = fCurrMeanMinDist_LT*(1.0f - fRollAvgFactor_LT) + fNormalizedMinDist*fRollAvgFactor_LT;
        fCurrMeanMinDist_ST = fCurrMeanMinDist_ST*(1.0f - fRollAvgFactor_ST) + fNormalizedMinDist*fRollAvgFactor_ST;
        fCurrMeanRawSegmRes_LT = fCurrMeanRawSegmRes_LT*(1.0f - fRollAvgFactor_LT);
        fCurrMeanRawSegmRes_ST = fCurrMeanRawSegmRes_ST*(1.0f - fRollAvgFactor_ST);
        if ((rand() % nCurrLocalWordUpdateRate) == 0) {
          size_t nGlobalWordLUTIdx;
          GlobalWord_3ch* pCurrGlobalWord = nullptr;
          for (nGlobalWordLUTIdx = 0; nGlobalWordLUTIdx < m_nCurrGlobalWords; ++nGlobalWordLUTIdx) {
            pCurrGlobalWord = (GlobalWord_3ch*)m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx];
            if (L1dist(nCurrIntraDescBITS, pCurrGlobalWord->nDescBITS) <= nCurrTotDescDistThreshold / GWORD_DESC_THRES_BITS_MATCH_FACTOR
              && cmixdist<3>(anCurrColor, pCurrGlobalWord->oFeature.anColor) <= nCurrTotColorDistThreshold)
              break;
          }
          if (nGlobalWordLUTIdx != m_nCurrGlobalWords || (rand() % (nCurrLocalWordUpdateRate * 2)) == 0) {
            if (nGlobalWordLUTIdx == m_nCurrGlobalWords) {
              pCurrGlobalWord = (GlobalWord_3ch*)m_apGlobalWordDict[m_nCurrGlobalWords - 1];
              for (size_t c = 0; c < 3; ++c) {
                pCurrGlobalWord->oFeature.anColor[c] = anCurrColor[c];
                pCurrGlobalWord->oFeature.anDesc[c] = anCurrIntraDesc[c];
              }
              pCurrGlobalWord->nDescBITS = nCurrIntraDescBITS;
              pCurrGlobalWord->oSpatioOccMap = cv::Scalar(0.0f);
              pCurrGlobalWord->fLatestWeight = 0.0f;
            }
            float& fCurrGlobalWordLocalWeight = *(float*)(pCurrGlobalWord->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
            if (fCurrGlobalWordLocalWeight < fPotentialLocalWordsWeightSum) {
              pCurrGlobalWord->fLatestWeight += fPotentialLocalWordsWeightSum;
              fCurrGlobalWordLocalWeight += fPotentialLocalWordsWeightSum;
            }
          }
        }
      }
      else {
        // == foreground
        const float fNormalizedMinDist = std::max(std::max((float)nMinTotColorDist / s_nColorMaxDataRange_3ch, (float)nMinTotDescDist / s_nDescMaxDataRange_3ch), (fLocalWordsWeightSumThreshold - fPotentialLocalWordsWeightSum) / fLocalWordsWeightSumThreshold);
        fCurrMeanMinDist_LT = fCurrMeanMinDist_LT*(1.0f - fRollAvgFactor_LT) + fNormalizedMinDist*fRollAvgFactor_LT;
        fCurrMeanMinDist_ST = fCurrMeanMinDist_ST*(1.0f - fRollAvgFactor_ST) + fNormalizedMinDist*fRollAvgFactor_ST;
        fCurrMeanRawSegmRes_LT = fCurrMeanRawSegmRes_LT*(1.0f - fRollAvgFactor_LT) + fRollAvgFactor_LT;
        fCurrMeanRawSegmRes_ST = fCurrMeanRawSegmRes_ST*(1.0f - fRollAvgFactor_ST) + fRollAvgFactor_ST;
        if (bCurrRegionIsFlat || (rand() % nCurrLocalWordUpdateRate) == 0) {
          size_t nGlobalWordLUTIdx;
          GlobalWord_3ch* pCurrGlobalWord;
          for (nGlobalWordLUTIdx = 0; nGlobalWordLUTIdx < m_nCurrGlobalWords; ++nGlobalWordLUTIdx) {
            pCurrGlobalWord = (GlobalWord_3ch*)m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx];
            if (L1dist(nCurrIntraDescBITS, pCurrGlobalWord->nDescBITS) <= nCurrTotDescDistThreshold / GWORD_DESC_THRES_BITS_MATCH_FACTOR
              && cmixdist<3>(anCurrColor, pCurrGlobalWord->oFeature.anColor) <= nCurrTotColorDistThreshold)
              break;
          }
          if (nGlobalWordLUTIdx == m_nCurrGlobalWords)
            nCurrRegionSegmVal = UCHAR_MAX;
          else {
            const float fGlobalWordLocalizedWeight = *(float*)(pCurrGlobalWord->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
            if (fPotentialLocalWordsWeightSum + fGlobalWordLocalizedWeight / (bCurrRegionIsFlat ? 2 : 4) < fLocalWordsWeightSumThreshold)
              nCurrRegionSegmVal = UCHAR_MAX;
          }
        }
        else
          nCurrRegionSegmVal = UCHAR_MAX;
        if (fPotentialLocalWordsWeightSum < DEFAULT_LWORD_INIT_WEIGHT) {
          const size_t nNewLocalWordIdx = m_nCurrLocalWords - 1;
          LocalWord_3ch* pNewLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx + nNewLocalWordIdx];
          for (size_t c = 0; c < 3; ++c) {
            pNewLocalWord->oFeature.anColor[c] = anCurrColor[c];
            pNewLocalWord->oFeature.anDesc[c] = anCurrIntraDesc[c];
          }
          pNewLocalWord->nOccurrences = nCurrWordOccIncr;
          pNewLocalWord->nFirstOcc = m_nFrameIndex;
          pNewLocalWord->nLastOcc = m_nFrameIndex;
        }
      }
      // == neighb updt
      if ((!nCurrRegionSegmVal && (rand() % nCurrLocalWordUpdateRate) == 0) || bCurrRegionIsROIBorder || m_bUsingMovingCamera) {
        //if((!nCurrRegionSegmVal && (rand()%(nCurrRegionIllumUpdtVal?(nCurrLocalWordUpdateRate/2+1):nCurrLocalWordUpdateRate))==0) || bCurrRegionIsROIBorder) {
        int nSampleImgCoord_Y, nSampleImgCoord_X;
        if (bCurrRegionIsFlat || bCurrRegionIsROIBorder || m_bUsingMovingCamera)
          getRandNeighborPosition_5x5(nSampleImgCoord_X, nSampleImgCoord_Y, nCurrImgCoord_X, nCurrImgCoord_Y, LBSP_::PATCH_SIZE / 2, m_oImgSize);
        else
          getRandNeighborPosition_3x3(nSampleImgCoord_X, nSampleImgCoord_Y, nCurrImgCoord_X, nCurrImgCoord_Y, LBSP_::PATCH_SIZE / 2, m_oImgSize);
        const size_t nSamplePxIdx = m_oImgSize.width*nSampleImgCoord_Y + nSampleImgCoord_X;
        if (m_oROI.data[nSamplePxIdx]) {
          const size_t nNeighborLocalDictIdx = m_aPxInfoLUT_PAWCS[nSamplePxIdx].nModelIdx*m_nCurrLocalWords;
          size_t nNeighborLocalWordIdx = 0;
          float fNeighborPotentialLocalWordsWeightSum = 0.0f;
          while (nNeighborLocalWordIdx < m_nCurrLocalWords && fNeighborPotentialLocalWordsWeightSum < fLocalWordsWeightSumThreshold) {
            LocalWord_3ch* pNeighborLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nNeighborLocalDictIdx + nNeighborLocalWordIdx];
            const size_t nNeighborTotColorL1Dist = L1dist<3>(anCurrColor, pNeighborLocalWord->oFeature.anColor);
            const size_t nNeighborColorDistortion = cdist<3>(anCurrColor, pNeighborLocalWord->oFeature.anColor);
            const size_t nNeighborTotColorMixDist = cmixdist(nNeighborTotColorL1Dist, nNeighborColorDistortion);
            const size_t nNeighborTotIntraDescDist = hdist<3>(anCurrIntraDesc, pNeighborLocalWord->oFeature.anDesc);
            const bool bNeighborRegionIsFlat = popcount<3>(pNeighborLocalWord->oFeature.anDesc) < FLAT_REGION_BIT_COUNT * 2;
            const size_t nNeighborWordOccIncr = bNeighborRegionIsFlat ? nCurrWordOccIncr * 2 : nCurrWordOccIncr;
            if (nNeighborTotColorMixDist <= nCurrTotColorDistThreshold && nNeighborTotIntraDescDist <= nCurrTotDescDistThreshold) {
              const float fNeighborLocalWordWeight = GetLocalWordWeight(pNeighborLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
              fNeighborPotentialLocalWordsWeightSum += fNeighborLocalWordWeight;
              pNeighborLocalWord->nLastOcc = m_nFrameIndex;
              if (fNeighborLocalWordWeight < DEFAULT_LWORD_MAX_WEIGHT)
                pNeighborLocalWord->nOccurrences += nNeighborWordOccIncr;
            }
            else if (!oCurrFGMask.data[nSamplePxIdx] && bCurrRegionIsFlat && (bBootstrapping || (rand() % nCurrLocalWordUpdateRate) == 0)) {
              const size_t nSamplePxRGBIdx = nSamplePxIdx * 3;
              const size_t nSampleDescRGBIdx = nSamplePxRGBIdx * 2;
              ushort* anNeighborLastIntraDesc = ((ushort*)(m_oLastDescFrame.data + nSampleDescRGBIdx));
              const size_t nNeighborTotLastIntraDescDist = hdist<3>(anCurrIntraDesc, anNeighborLastIntraDesc);
              if (nNeighborTotColorMixDist <= nCurrTotColorDistThreshold && nNeighborTotLastIntraDescDist <= nCurrTotDescDistThreshold / 2) {
                const float fNeighborLocalWordWeight = GetLocalWordWeight(pNeighborLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
                fNeighborPotentialLocalWordsWeightSum += fNeighborLocalWordWeight;
                pNeighborLocalWord->nLastOcc = m_nFrameIndex;
                if (fNeighborLocalWordWeight < DEFAULT_LWORD_MAX_WEIGHT)
                  pNeighborLocalWord->nOccurrences += nNeighborWordOccIncr;
                for (size_t c = 0; c < 3; ++c)
                  pNeighborLocalWord->oFeature.anDesc[c] = anCurrIntraDesc[c];
              }
              else {
                const bool bNeighborLastRegionIsFlat = popcount<3>(anNeighborLastIntraDesc) < FLAT_REGION_BIT_COUNT * 2;
                if (bNeighborLastRegionIsFlat && bCurrRegionIsFlat &&
                  nNeighborTotLastIntraDescDist + nNeighborTotIntraDescDist <= nCurrTotDescDistThreshold &&
                  nNeighborColorDistortion <= nCurrTotColorDistThreshold / 4) {
                  const float fNeighborLocalWordWeight = GetLocalWordWeight(pNeighborLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
                  fNeighborPotentialLocalWordsWeightSum += fNeighborLocalWordWeight;
                  pNeighborLocalWord->nLastOcc = m_nFrameIndex;
                  if (fNeighborLocalWordWeight < DEFAULT_LWORD_MAX_WEIGHT)
                    pNeighborLocalWord->nOccurrences += nNeighborWordOccIncr;
                  for (size_t c = 0; c < 3; ++c)
                    pNeighborLocalWord->oFeature.anColor[c] = anCurrColor[c];
                }
              }
            }
            ++nNeighborLocalWordIdx;
          }
          if (fNeighborPotentialLocalWordsWeightSum < DEFAULT_LWORD_INIT_WEIGHT) {
            nNeighborLocalWordIdx = m_nCurrLocalWords - 1;
            LocalWord_3ch* pNeighborLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nNeighborLocalDictIdx + nNeighborLocalWordIdx];
            for (size_t c = 0; c < 3; ++c) {
              pNeighborLocalWord->oFeature.anColor[c] = anCurrColor[c];
              pNeighborLocalWord->oFeature.anDesc[c] = anCurrIntraDesc[c];
            }
            pNeighborLocalWord->nOccurrences = nCurrWordOccIncr;
            pNeighborLocalWord->nFirstOcc = m_nFrameIndex;
            pNeighborLocalWord->nLastOcc = m_nFrameIndex;
          }
        }
      }
      if (nCurrRegionIllumUpdtVal)
        nCurrRegionIllumUpdtVal -= 1;
      // == feedback adj
      bCurrRegionIsUnstable = fCurrDistThresholdFactor > UNSTABLE_REG_RDIST_MIN || (fCurrMeanRawSegmRes_LT - fCurrMeanFinalSegmRes_LT) > UNSTABLE_REG_RATIO_MIN || (fCurrMeanRawSegmRes_ST - fCurrMeanFinalSegmRes_ST) > UNSTABLE_REG_RATIO_MIN;
      if (m_oLastFGMask.data[nPxIter] || (std::min(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST) < UNSTABLE_REG_RATIO_MIN && nCurrRegionSegmVal))
        fCurrLearningRate = std::min(fCurrLearningRate + FEEDBACK_T_INCR / (std::max(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST)*fCurrDistThresholdVariationFactor), FEEDBACK_T_UPPER);
      else
        fCurrLearningRate = std::max(fCurrLearningRate - FEEDBACK_T_DECR*fCurrDistThresholdVariationFactor / std::max(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST), FEEDBACK_T_LOWER);
      if (std::max(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST) > UNSTABLE_REG_RATIO_MIN && m_oBlinksFrame.data[nPxIter])
        (fCurrDistThresholdVariationFactor) += bBootstrapping ? FEEDBACK_V_INCR * 2 : FEEDBACK_V_INCR;
      else
        fCurrDistThresholdVariationFactor = std::max(fCurrDistThresholdVariationFactor - FEEDBACK_V_DECR*((bBootstrapping || bCurrRegionIsFlat) ? 2 : m_oLastFGMask.data[nPxIter] ? 0.5f : 1), FEEDBACK_V_DECR);
      if (fCurrDistThresholdFactor < std::pow(1.0f + std::min(fCurrMeanMinDist_LT, fCurrMeanMinDist_ST) * 2, 2))
        fCurrDistThresholdFactor += FEEDBACK_R_VAR*(fCurrDistThresholdVariationFactor - FEEDBACK_V_DECR);
      else
        fCurrDistThresholdFactor = std::max(fCurrDistThresholdFactor - FEEDBACK_R_VAR / fCurrDistThresholdVariationFactor, 1.0f);
      for (size_t c = 0; c < 3; ++c) {
        anLastIntraDesc[c] = anCurrIntraDesc[c];
        anLastColor[c] = anCurrColor[c];
      }
    }
  }
  const bool bRecalcGlobalWords = !(m_nFrameIndex % (nCurrGlobalWordUpdateRate << 5));
  const bool bUpdateGlobalWords = !(m_nFrameIndex % (nCurrGlobalWordUpdateRate));
  cv::Mat oLastFGMask_dilated_inverted_downscaled;
  if (bUpdateGlobalWords)
    cv::resize(m_oLastFGMask_dilated_inverted, oLastFGMask_dilated_inverted_downscaled, m_oDownSampledFrameSize_GlobalWordLookup, 0, 0, cv::INTER_NEAREST);
  for (size_t nGlobalWordIdx = 0; nGlobalWordIdx < m_nCurrGlobalWords; ++nGlobalWordIdx) {
    if (bRecalcGlobalWords && m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight > 0.0f) {
      m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight = GetGlobalWordWeight(m_apGlobalWordDict[nGlobalWordIdx]);
      if (m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight < 1.0f) {
        m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight = 0.0f;
        m_apGlobalWordDict[nGlobalWordIdx]->oSpatioOccMap = cv::Scalar(0.0f);
      }
    }
    if (bUpdateGlobalWords && m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight > 0.0f) {
      cv::accumulateProduct(m_apGlobalWordDict[nGlobalWordIdx]->oSpatioOccMap, m_oTempGlobalWordWeightDiffFactor, m_apGlobalWordDict[nGlobalWordIdx]->oSpatioOccMap, oLastFGMask_dilated_inverted_downscaled);
      m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight *= 0.9f;
      cv::blur(m_apGlobalWordDict[nGlobalWordIdx]->oSpatioOccMap, m_apGlobalWordDict[nGlobalWordIdx]->oSpatioOccMap, cv::Size(3, 3), cv::Point(-1, -1), cv::BORDER_REPLICATE);
    }
    if (nGlobalWordIdx > 0 && m_apGlobalWordDict[nGlobalWordIdx]->fLatestWeight > m_apGlobalWordDict[nGlobalWordIdx - 1]->fLatestWeight)
      std::swap(m_apGlobalWordDict[nGlobalWordIdx], m_apGlobalWordDict[nGlobalWordIdx - 1]);
  }
  if (bUpdateGlobalWords) {
    for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
      const size_t nPxIter = m_aPxIdxLUT[nModelIter];
      const size_t nGlobalWordMapLookupIdx = m_aPxInfoLUT_PAWCS[nPxIter].nGlobalWordMapLookupIdx;
      float fLastGlobalWordLocalWeight = *(float*)(m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[0]->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
      for (size_t nGlobalWordLUTIdx = 1; nGlobalWordLUTIdx < m_nCurrGlobalWords; ++nGlobalWordLUTIdx) {
        const float fCurrGlobalWordLocalWeight = *(float*)(m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx]->oSpatioOccMap.data + nGlobalWordMapLookupIdx);
        if (fCurrGlobalWordLocalWeight > fLastGlobalWordLocalWeight)
          std::swap(m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx], m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT[nGlobalWordLUTIdx - 1]);
        else
          fLastGlobalWordLocalWeight = fCurrGlobalWordLocalWeight;
      }
    }
  }
  cv::bitwise_xor(oCurrFGMask, m_oLastRawFGMask, m_oCurrRawFGBlinkMask);
  cv::bitwise_or(m_oCurrRawFGBlinkMask, m_oLastRawFGBlinkMask, m_oBlinksFrame);
  m_oCurrRawFGBlinkMask.copyTo(m_oLastRawFGBlinkMask);
  oCurrFGMask.copyTo(m_oLastRawFGMask);
  cv::morphologyEx(oCurrFGMask, m_oFGMask_PreFlood, cv::MORPH_CLOSE, m_oMorphExStructElement);
  m_oFGMask_PreFlood.copyTo(m_oFGMask_FloodedHoles);
  cv::floodFill(m_oFGMask_FloodedHoles, cv::Point(0, 0), UCHAR_MAX);
  cv::bitwise_not(m_oFGMask_FloodedHoles, m_oFGMask_FloodedHoles);
  cv::erode(m_oFGMask_PreFlood, m_oFGMask_PreFlood, cv::Mat(), cv::Point(-1, -1), 3);
  cv::bitwise_or(oCurrFGMask, m_oFGMask_FloodedHoles, oCurrFGMask);
  cv::bitwise_or(oCurrFGMask, m_oFGMask_PreFlood, oCurrFGMask);
  cv::medianBlur(oCurrFGMask, m_oLastFGMask, m_nMedianBlurKernelSize);
  cv::dilate(m_oLastFGMask, m_oLastFGMask_dilated, cv::Mat(), cv::Point(-1, -1), 3);
  cv::bitwise_and(m_oBlinksFrame, m_oLastFGMask_dilated_inverted, m_oBlinksFrame);
  cv::bitwise_not(m_oLastFGMask_dilated, m_oLastFGMask_dilated_inverted);
  cv::bitwise_and(m_oBlinksFrame, m_oLastFGMask_dilated_inverted, m_oBlinksFrame);
  m_oLastFGMask.copyTo(oCurrFGMask);
  cv::addWeighted(m_oMeanFinalSegmResFrame_LT, (1.0f - fRollAvgFactor_LT), m_oLastFGMask, (1.0 / UCHAR_MAX)*fRollAvgFactor_LT, 0, m_oMeanFinalSegmResFrame_LT, CV_32F);
  cv::addWeighted(m_oMeanFinalSegmResFrame_ST, (1.0f - fRollAvgFactor_ST), m_oLastFGMask, (1.0 / UCHAR_MAX)*fRollAvgFactor_ST, 0, m_oMeanFinalSegmResFrame_ST, CV_32F);
  const float fCurrNonFlatRegionRatio = (float)(m_nTotRelevantPxCount - nFlatRegionCount) / m_nTotRelevantPxCount;
  if (fCurrNonFlatRegionRatio < LBSPDESC_RATIO_MIN && m_fLastNonFlatRegionRatio < LBSPDESC_RATIO_MIN) {
    for (size_t t = 0; t <= UCHAR_MAX; ++t)
      if (m_anLBSPThreshold_8bitLUT[t] > cv::saturate_cast<uchar>((m_nLBSPThresholdOffset + t*m_fRelLBSPThreshold) / 4))
        --m_anLBSPThreshold_8bitLUT[t];
  }
  else if (fCurrNonFlatRegionRatio > LBSPDESC_RATIO_MAX && m_fLastNonFlatRegionRatio > LBSPDESC_RATIO_MAX) {
    for (size_t t = 0; t <= UCHAR_MAX; ++t)
      if (m_anLBSPThreshold_8bitLUT[t] < cv::saturate_cast<uchar>(m_nLBSPThresholdOffset + UCHAR_MAX*m_fRelLBSPThreshold))
        ++m_anLBSPThreshold_8bitLUT[t];
  }
  m_fLastNonFlatRegionRatio = fCurrNonFlatRegionRatio;
  cv::resize(oInputImg, m_oDownSampledFrame_MotionAnalysis, m_oDownSampledFrameSize_MotionAnalysis, 0, 0, cv::INTER_AREA);
  cv::accumulateWeighted(m_oDownSampledFrame_MotionAnalysis, m_oMeanDownSampledLastDistFrame_LT, fRollAvgFactor_LT);
  cv::accumulateWeighted(m_oDownSampledFrame_MotionAnalysis, m_oMeanDownSampledLastDistFrame_ST, fRollAvgFactor_ST);
  const float fCurrMeanL1DistRatio = L1dist((float*)m_oMeanDownSampledLastDistFrame_LT.data, (float*)m_oMeanDownSampledLastDistFrame_ST.data, m_oMeanDownSampledLastDistFrame_LT.total(), m_nImgChannels, m_oDownSampledROI_MotionAnalysis.data) / m_nDownSampledROIPxCount;
  if (!m_bAutoModelResetEnabled && fCurrMeanL1DistRatio >= FRAMELEVEL_MIN_L1DIST_THRES * 2)
    m_bAutoModelResetEnabled = true;
  if (m_bAutoModelResetEnabled || m_bUsingMovingCamera) {
    if ((m_nFrameIndex%DEFAULT_BOOTSTRAP_WIN_SIZE) == 0) {
      cv::Mat oCurrBackgroundImg, oDownSampledBackgroundImg;
      getBackgroundImage(oCurrBackgroundImg);
      cv::resize(oCurrBackgroundImg, oDownSampledBackgroundImg, m_oDownSampledFrameSize_MotionAnalysis, 0, 0, cv::INTER_AREA);
      cv::Mat oDownSampledBackgroundImg_32F; oDownSampledBackgroundImg.convertTo(oDownSampledBackgroundImg_32F, CV_32F);
      const float fCurrModelL1DistRatio = L1dist((float*)m_oMeanDownSampledLastDistFrame_LT.data, (float*)oDownSampledBackgroundImg_32F.data, m_oMeanDownSampledLastDistFrame_LT.total(), m_nImgChannels, cv::Mat(m_oDownSampledROI_MotionAnalysis == UCHAR_MAX).data) / m_nDownSampledROIPxCount;
      const float fCurrModelCDistRatio = cdist((float*)m_oMeanDownSampledLastDistFrame_LT.data, (float*)oDownSampledBackgroundImg_32F.data, m_oMeanDownSampledLastDistFrame_LT.total(), m_nImgChannels, cv::Mat(m_oDownSampledROI_MotionAnalysis == UCHAR_MAX).data) / m_nDownSampledROIPxCount;
      if (m_bUsingMovingCamera && fCurrModelL1DistRatio < FRAMELEVEL_MIN_L1DIST_THRES / 4 && fCurrModelCDistRatio < FRAMELEVEL_MIN_CDIST_THRES / 4) {
        m_nLocalWordWeightOffset = DEFAULT_LWORD_WEIGHT_OFFSET;
        m_bUsingMovingCamera = false;
        refreshModel(1, 1, true);
      }
      else if (bBootstrapping && !m_bUsingMovingCamera && (fCurrModelL1DistRatio >= FRAMELEVEL_MIN_L1DIST_THRES || fCurrModelCDistRatio >= FRAMELEVEL_MIN_CDIST_THRES)) {
        m_nLocalWordWeightOffset = 5;
        m_bUsingMovingCamera = true;
        refreshModel(1, 1, true);
      }
    }
    if (m_nFramesSinceLastReset > DEFAULT_BOOTSTRAP_WIN_SIZE * 2)
      m_bAutoModelResetEnabled = false;
    else if (fCurrMeanL1DistRatio >= FRAMELEVEL_MIN_L1DIST_THRES && m_nModelResetCooldown == 0) {
      m_nFramesSinceLastReset = 0;
      refreshModel(m_nLocalWordWeightOffset / 8, 0, true);
      m_nModelResetCooldown = nCurrSamplesForMovingAvg_ST;
      m_oUpdateRateFrame = cv::Scalar(1.0f);
    }
    else if (!bBootstrapping)
      ++m_nFramesSinceLastReset;
  }
  if (m_nModelResetCooldown > 0)
    --m_nModelResetCooldown;
}

void BackgroundSubtractorPAWCS::getBackgroundImage(cv::OutputArray backgroundImage) const { // @@@ add option to reconstruct from gwords?
  CV_Assert(m_bInitialized);
  cv::Mat oAvgBGImg = cv::Mat::zeros(m_oImgSize, CV_32FC((int)m_nImgChannels));
  for (size_t nModelIter = 0; nModelIter < m_nTotRelevantPxCount; ++nModelIter) {
    const size_t nPxIter = m_aPxIdxLUT[nModelIter];
    const size_t nLocalDictIdx = nModelIter*m_nCurrLocalWords;
    const int nCurrImgCoord_X = m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_X;
    const int nCurrImgCoord_Y = m_aPxInfoLUT_PAWCS[nPxIter].nImgCoord_Y;
    if (m_nImgChannels == 1) {
      float fTotWeight = 0.0f;
      float fTotColor = 0.0f;
      for (size_t nLocalWordIdx = 0; nLocalWordIdx < m_nCurrLocalWords; ++nLocalWordIdx) {
        LocalWord_1ch* pCurrLocalWord = (LocalWord_1ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
        float fCurrWeight = GetLocalWordWeight(pCurrLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
        fTotColor += (float)pCurrLocalWord->oFeature.anColor[0] * fCurrWeight;
        fTotWeight += fCurrWeight;
      }
      oAvgBGImg.at<float>(nCurrImgCoord_Y, nCurrImgCoord_X) = fTotColor / fTotWeight;
    }
    else { //m_nImgChannels==3
      float fTotWeight = 0.0f;
      float fTotColor[3] = { 0.0f,0.0f,0.0f };
      for (size_t nLocalWordIdx = 0; nLocalWordIdx < m_nCurrLocalWords; ++nLocalWordIdx) {
        LocalWord_3ch* pCurrLocalWord = (LocalWord_3ch*)m_apLocalWordDict[nLocalDictIdx + nLocalWordIdx];
        float fCurrWeight = GetLocalWordWeight(pCurrLocalWord, m_nFrameIndex, m_nLocalWordWeightOffset);
        for (size_t c = 0; c < 3; ++c)
          fTotColor[c] += (float)pCurrLocalWord->oFeature.anColor[c] * fCurrWeight;
        fTotWeight += fCurrWeight;
      }
      oAvgBGImg.at<cv::Vec3f>(nCurrImgCoord_Y, nCurrImgCoord_X) = cv::Vec3f(fTotColor[0] / fTotWeight, fTotColor[1] / fTotWeight, fTotColor[2] / fTotWeight);
    }
  }
  oAvgBGImg.convertTo(backgroundImage, CV_8U);
}

void BackgroundSubtractorPAWCS::getBackgroundDescriptorsImage(cv::OutputArray backgroundDescImage) const {
  CV_Assert(LBSP_::DESC_SIZE == 2);
  CV_Assert(m_bInitialized);
  cv::Mat oAvgBGDesc = cv::Mat::zeros(m_oImgSize, CV_32FC((int)m_nImgChannels));
  // @@@@@@ TO BE REWRITTEN FOR WORD-BASED RECONSTRUCTION
  /*for(size_t n=0; n<m_voBGDescSamples.size(); ++n) {
        for(int y=0; y<m_oImgSize.height; ++y) {
            for(int x=0; x<m_oImgSize.width; ++x) {
                const size_t nDescIter = m_voBGDescSamples[n].step.p[0]*y + m_voBGDescSamples[n].step.p[1]*x;
                const size_t nFloatIter = nDescIter*2;
                float* oAvgBgDescPtr = (float*)(oAvgBGDesc.data+nFloatIter);
                const ushort* const oBGDescPtr = (ushort*)(m_voBGDescSamples[n].data+nDescIter);
                for(size_t c=0; c<m_nImgChannels; ++c)
                    oAvgBgDescPtr[c] += ((float)oBGDescPtr[c])/m_voBGDescSamples.size();
            }
        }
    }*/
  oAvgBGDesc.convertTo(backgroundDescImage, CV_16U);
}

void BackgroundSubtractorPAWCS::CleanupDictionaries() {
  if (m_aLocalWordList_1ch) {
    delete[] m_aLocalWordList_1ch;
    m_aLocalWordList_1ch = nullptr;
    m_pLocalWordListIter_1ch = nullptr;
  }
  else if (m_aLocalWordList_3ch) {
    delete[] m_aLocalWordList_3ch;
    m_aLocalWordList_3ch = nullptr;
    m_pLocalWordListIter_3ch = nullptr;
  }
  if (m_apLocalWordDict) {
    delete[] m_apLocalWordDict;
    m_apLocalWordDict = nullptr;
  }
  if (m_aGlobalWordList_1ch) {
    delete[] m_aGlobalWordList_1ch;
    m_aGlobalWordList_1ch = nullptr;
    m_pGlobalWordListIter_1ch = nullptr;
  }
  else if (m_aGlobalWordList_3ch) {
    delete[] m_aGlobalWordList_3ch;
    m_aGlobalWordList_3ch = nullptr;
    m_pGlobalWordListIter_3ch = nullptr;
  }
  if (m_apGlobalWordDict) {
    delete[] m_apGlobalWordDict;
    m_apGlobalWordDict = nullptr;
  }
  if (m_aPxInfoLUT_PAWCS) {
    for (size_t nPxIter = 0; nPxIter < m_nTotPxCount; ++nPxIter)
      delete[] m_aPxInfoLUT_PAWCS[nPxIter].apGlobalDictSortLUT;
    delete[] m_aPxInfoLUT_PAWCS;
    m_aPxInfoLUT = nullptr;
    m_aPxInfoLUT_PAWCS = nullptr;
  }
  if (m_aPxIdxLUT) {
    delete[] m_aPxIdxLUT;
    m_aPxIdxLUT = nullptr;
  }
}

float BackgroundSubtractorPAWCS::GetLocalWordWeight(const LocalWordBase* w, size_t nCurrFrame, size_t nOffset) {
  return (float)(w->nOccurrences) / ((w->nLastOcc - w->nFirstOcc) + (nCurrFrame - w->nLastOcc) * 2 + nOffset);
}

float BackgroundSubtractorPAWCS::GetGlobalWordWeight(const GlobalWordBase* w) {
  return (float)cv::sum(w->oSpatioOccMap).val[0];
}
