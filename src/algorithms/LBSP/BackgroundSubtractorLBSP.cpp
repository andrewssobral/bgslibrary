#include <iostream>
#include <iomanip>
#include <exception>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "BackgroundSubtractorLBSP.h"
#include "DistanceUtils.h"
#include "RandUtils.h"

//using namespace bgslibrary::algorithms::lbsp;

#ifndef SIZE_MAX
# if __WORDSIZE == 64
#  define SIZE_MAX		(18446744073709551615UL)
# else
#  define SIZE_MAX		(4294967295U)
# endif
#endif

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lbsp
    {
      // local define used to determine the default median blur kernel size
      const int DEFAULT_MEDIAN_BLUR_KERNEL_SIZE = 9;

      BackgroundSubtractorLBSP::BackgroundSubtractorLBSP(float fRelLBSPThreshold, size_t nLBSPThresholdOffset)
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
        , nDebugCoordX(0), nDebugCoordY(0) {
        CV_Assert(m_fRelLBSPThreshold >= 0);
      }

      BackgroundSubtractorLBSP::~BackgroundSubtractorLBSP() {}

      void BackgroundSubtractorLBSP::initialize(const cv::Mat& oInitImg) {
        this->initialize(oInitImg, cv::Mat());
      }

      /*cv::AlgorithmInfo* BackgroundSubtractorLBSP::info() const {
        return nullptr;
      }*/

      cv::Mat BackgroundSubtractorLBSP::getROICopy() const {
        return m_oROI.clone();
      }

      void BackgroundSubtractorLBSP::setROI(cv::Mat& oROI) {
        LBSP::validateROI(oROI);
        CV_Assert(cv::countNonZero(oROI) > 0);
        if (m_bInitialized) {
          cv::Mat oLatestBackgroundImage;
          getBackgroundImage(oLatestBackgroundImage);
          initialize(oLatestBackgroundImage, oROI);
        }
        else
          m_oROI = oROI.clone();
      }

      void BackgroundSubtractorLBSP::setAutomaticModelReset(bool bVal) {
        m_bAutoModelResetEnabled = bVal;
      }
    }
  }
}
