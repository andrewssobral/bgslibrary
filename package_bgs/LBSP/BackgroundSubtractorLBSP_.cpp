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
#include "BackgroundSubtractorLBSP_.h"
#include "DistanceUtils.h"
#include "RandUtils.h"
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iomanip>
#include <exception>

// local define used to determine the default median blur kernel size
#define DEFAULT_MEDIAN_BLUR_KERNEL_SIZE (9)

BackgroundSubtractorLBSP_::BackgroundSubtractorLBSP_(float fRelLBSPThreshold, size_t nLBSPThresholdOffset)
  : m_nImgChannels(0)
  , m_nImgType(0)
  , m_nLBSPThresholdOffset(nLBSPThresholdOffset)
  , m_fRelLBSPThreshold(fRelLBSPThreshold)
  , m_nTotPxCount(0)
  , m_nTotRelevantPxCount(0)
  , m_nFrameIndex(SIZE_MAX)
  , m_nFramesSinceLastReset(0)
  , m_nModelResetCooldown(0)
  , m_aPxIdxLUT(nullptr)
  , m_aPxInfoLUT(nullptr)
  , m_nDefaultMedianBlurKernelSize(DEFAULT_MEDIAN_BLUR_KERNEL_SIZE)
  , m_bInitialized(false)
  , m_bAutoModelResetEnabled(true)
  , m_bUsingMovingCamera(false)
  , m_nDebugCoordX(0)
  , m_nDebugCoordY(0)
  , m_pDebugFS(nullptr) {
  CV_Assert(m_fRelLBSPThreshold >= 0);
}

BackgroundSubtractorLBSP_::~BackgroundSubtractorLBSP_() {}

void BackgroundSubtractorLBSP_::initialize(const cv::Mat& oInitImg) {
  this->initialize(oInitImg, cv::Mat());
}
/*
cv::AlgorithmInfo* BackgroundSubtractorLBSP_::info() const {
    return nullptr;
}
*/
cv::Mat BackgroundSubtractorLBSP_::getROICopy() const {
  return m_oROI.clone();
}

void BackgroundSubtractorLBSP_::setROI(cv::Mat& oROI) {
  LBSP_::validateROI(oROI);
  CV_Assert(cv::countNonZero(oROI) > 0);
  if (m_bInitialized) {
    cv::Mat oLatestBackgroundImage;
    getBackgroundImage(oLatestBackgroundImage);
    initialize(oLatestBackgroundImage, oROI);
  }
  else
    m_oROI = oROI.clone();
}

void BackgroundSubtractorLBSP_::setAutomaticModelReset(bool bVal) {
  m_bAutoModelResetEnabled = bVal;
}
