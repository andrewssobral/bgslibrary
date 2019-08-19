#include <iostream>
#include <iomanip>
#include <exception>

#include <opencv2/imgproc/imgproc.hpp>
#ifndef MEX_COMPILE_FLAG
#include <opencv2/highgui/highgui.hpp>
#endif

#include "BackgroundSubtractorLBSP_.h"
#include "DistanceUtils.h"
#include "RandUtils.h"

//using namespace bgslibrary::algorithms::lbsp;

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lbsp
    {
      // local define used to determine the default median blur kernel size
      const int DEFAULT_MEDIAN_BLUR_KERNEL_SIZE = 9;

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
    }
  }
}
