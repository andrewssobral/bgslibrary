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

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "DistanceUtils.h"

/*!
    Local Binary Similarity Pattern (LBSP) feature extractor

    Note 1: both grayscale and RGB/BGR images may be used with this extractor.
    Note 2: using LBSP_::compute2(...) is logically equivalent to using LBSP_::compute(...) followed by LBSP_::reshapeDesc(...).

    For more details on the different parameters, see G.-A. Bilodeau et al, "Change Detection in Feature Space Using Local
    Binary Similarity Patterns", in CRV 2013.

    This algorithm is currently NOT thread-safe.
 */
class LBSP_ : public cv::Feature2D {
public:
  //! constructor 1, threshold = absolute intensity 'similarity' threshold used when computing comparisons
  LBSP_(size_t nThreshold);
  //! constructor 2, threshold = relative intensity 'similarity' threshold used when computing comparisons
  LBSP_(float fRelThreshold, size_t nThresholdOffset = 0);
  //! default destructor
  virtual ~LBSP_();
  //! loads extractor params from the specified file node @@@@ not impl
  virtual void read(const cv::FileNode&);
  //! writes extractor params to the specified file storage @@@@ not impl
  virtual void write(cv::FileStorage&) const;
  //! sets the 'reference' image to be used for inter-frame comparisons (note: if no image is set or if the image is empty, the algorithm will default back to intra-frame comparisons)
  virtual void setReference(const cv::Mat&);
  //! returns the current descriptor size, in bytes
  virtual int descriptorSize() const;
  //! returns the current descriptor data type
  virtual int descriptorType() const;
  //! returns whether this extractor is using a relative threshold or not
  virtual bool isUsingRelThreshold() const;
  //! returns the current relative threshold used for comparisons (-1 = invalid/not used)
  virtual float getRelThreshold() const;
  //! returns the current absolute threshold used for comparisons (-1 = invalid/not used)
  virtual size_t getAbsThreshold() const;

  //! similar to DescriptorExtractor::compute(const cv::Mat& image, ...), but in this case, the descriptors matrix has the same shape as the input matrix (possibly slower, but the result can be displayed)
  void compute2(const cv::Mat& oImage, std::vector<cv::KeyPoint>& voKeypoints, cv::Mat& oDescriptors) const;
  //! batch version of LBSP_::compute2(const cv::Mat& image, ...), also similar to DescriptorExtractor::compute(const std::vector<cv::Mat>& imageCollection, ...)
  void compute2(const std::vector<cv::Mat>& voImageCollection, std::vector<std::vector<cv::KeyPoint> >& vvoPointCollection, std::vector<cv::Mat>& voDescCollection) const;

  //! utility function, shortcut/lightweight/direct single-point LBSP computation function for extra flexibility (1-channel version)
  inline static void computeGrayscaleDescriptor(const cv::Mat& oInputImg, const uchar _ref, const int _x, const int _y, const size_t _t, ushort& _res) {
    CV_DbgAssert(!oInputImg.empty());
    CV_DbgAssert(oInputImg.type() == CV_8UC1);
    CV_DbgAssert(LBSP_::DESC_SIZE == 2); // @@@ also relies on a constant desc size
    CV_DbgAssert(_x >= (int)LBSP_::PATCH_SIZE / 2 && _y >= (int)LBSP_::PATCH_SIZE / 2);
    CV_DbgAssert(_x < oInputImg.cols - (int)LBSP_::PATCH_SIZE / 2 && _y < oInputImg.rows - (int)LBSP_::PATCH_SIZE / 2);
    const size_t _step_row = oInputImg.step.p[0];
    const uchar* const _data = oInputImg.data;
#include "LBSP_16bits_dbcross_1ch.i"
  }

  //! utility function, shortcut/lightweight/direct single-point LBSP computation function for extra flexibility (3-channels version)
  inline static void computeRGBDescriptor(const cv::Mat& oInputImg, const uchar* const _ref, const int _x, const int _y, const size_t* const _t, ushort* _res) {
    CV_DbgAssert(!oInputImg.empty());
    CV_DbgAssert(oInputImg.type() == CV_8UC3);
    CV_DbgAssert(LBSP_::DESC_SIZE == 2); // @@@ also relies on a constant desc size
    CV_DbgAssert(_x >= (int)LBSP_::PATCH_SIZE / 2 && _y >= (int)LBSP_::PATCH_SIZE / 2);
    CV_DbgAssert(_x < oInputImg.cols - (int)LBSP_::PATCH_SIZE / 2 && _y < oInputImg.rows - (int)LBSP_::PATCH_SIZE / 2);
    const size_t _step_row = oInputImg.step.p[0];
    const uchar* const _data = oInputImg.data;
#include "LBSP_16bits_dbcross_3ch3t.i"
  }

  //! utility function, shortcut/lightweight/direct single-point LBSP computation function for extra flexibility (3-channels version)
  inline static void computeRGBDescriptor(const cv::Mat& oInputImg, const uchar* const _ref, const int _x, const int _y, const size_t _t, ushort* _res) {
    CV_DbgAssert(!oInputImg.empty());
    CV_DbgAssert(oInputImg.type() == CV_8UC3);
    CV_DbgAssert(LBSP_::DESC_SIZE == 2); // @@@ also relies on a constant desc size
    CV_DbgAssert(_x >= (int)LBSP_::PATCH_SIZE / 2 && _y >= (int)LBSP_::PATCH_SIZE / 2);
    CV_DbgAssert(_x < oInputImg.cols - (int)LBSP_::PATCH_SIZE / 2 && _y < oInputImg.rows - (int)LBSP_::PATCH_SIZE / 2);
    const size_t _step_row = oInputImg.step.p[0];
    const uchar* const _data = oInputImg.data;
#include "LBSP_16bits_dbcross_3ch1t.i"
  }

  //! utility function, shortcut/lightweight/direct single-point LBSP computation function for extra flexibility (1-channel-RGB version)
  inline static void computeSingleRGBDescriptor(const cv::Mat& oInputImg, const uchar _ref, const int _x, const int _y, const size_t _c, const size_t _t, ushort& _res) {
    CV_DbgAssert(!oInputImg.empty());
    CV_DbgAssert(oInputImg.type() == CV_8UC3 && _c < 3);
    CV_DbgAssert(LBSP_::DESC_SIZE == 2); // @@@ also relies on a constant desc size
    CV_DbgAssert(_x >= (int)LBSP_::PATCH_SIZE / 2 && _y >= (int)LBSP_::PATCH_SIZE / 2);
    CV_DbgAssert(_x < oInputImg.cols - (int)LBSP_::PATCH_SIZE / 2 && _y < oInputImg.rows - (int)LBSP_::PATCH_SIZE / 2);
    const size_t _step_row = oInputImg.step.p[0];
    const uchar* const _data = oInputImg.data;
#include "LBSP_16bits_dbcross_s3ch.i"
  }

  //! utility function, used to reshape a descriptors matrix to its input image size via their keypoint locations
  static void reshapeDesc(cv::Size oSize, const std::vector<cv::KeyPoint>& voKeypoints, const cv::Mat& oDescriptors, cv::Mat& oOutput);
  //! utility function, used to illustrate the difference between two descriptor images
  static void calcDescImgDiff(const cv::Mat& oDesc1, const cv::Mat& oDesc2, cv::Mat& oOutput, bool bForceMergeChannels = false);
  //! utility function, used to filter out bad keypoints that would trigger out of bounds error because they're too close to the image border
  static void validateKeyPoints(std::vector<cv::KeyPoint>& voKeypoints, cv::Size oImgSize);
  //! utility function, used to filter out bad pixels in a ROI that would trigger out of bounds error because they're too close to the image border
  static void validateROI(cv::Mat& oROI);
  //! utility, specifies the pixel size of the pattern used (width and height)
  static const size_t PATCH_SIZE = 5;
  //! utility, specifies the number of bytes per descriptor (should be the same as calling 'descriptorSize()')
  static const size_t DESC_SIZE = 2;

protected:
  //! classic 'compute' implementation, based on the regular DescriptorExtractor::computeImpl arguments & expected output
  virtual void computeImpl(const cv::Mat& oImage, std::vector<cv::KeyPoint>& voKeypoints, cv::Mat& oDescriptors) const;

  const bool m_bOnlyUsingAbsThreshold;
  const float m_fRelThreshold;
  const size_t m_nThreshold;
  cv::Mat m_oRefImage;
};
