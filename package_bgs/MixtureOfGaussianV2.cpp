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
#include "MixtureOfGaussianV2.h"

using namespace bgslibrary::algorithms;

MixtureOfGaussianV2::MixtureOfGaussianV2() :
  alpha(0.05), enableThreshold(true), threshold(15)
{
  std::cout << "MixtureOfGaussianV2()" << std::endl;
  setup("./config/MixtureOfGaussianV2.xml");
}

MixtureOfGaussianV2::~MixtureOfGaussianV2()
{
  std::cout << "~MixtureOfGaussianV2()" << std::endl;
}

void MixtureOfGaussianV2::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
#if CV_MAJOR_VERSION >= 3
    mog = cv::createBackgroundSubtractorMOG2();
#endif
  }

  //------------------------------------------------------------------
  // BackgroundSubtractorMOG2
  // http://opencv.itseez.com/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractormog2
  //
  // Gaussian Mixture-based Backbround/Foreground Segmentation Algorithm.
  //
  // The class implements the Gaussian mixture model background subtraction described in:
  //  (1) Z.Zivkovic, Improved adaptive Gausian mixture model for background subtraction, International Conference Pattern Recognition, UK, August, 2004,
  //  The code is very fast and performs also shadow detection. Number of Gausssian components is adapted per pixel.
  //
  //  (2) Z.Zivkovic, F. van der Heijden, Efficient Adaptive Density Estimation per Image Pixel for the Task of Background Subtraction,
  //  Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006.
  //  The algorithm similar to the standard Stauffer&Grimson algorithm with additional selection of the number of the Gaussian components based on:
  //    Z.Zivkovic, F.van der Heijden, Recursive unsupervised learning of finite mixture models, IEEE Trans. on Pattern Analysis and Machine Intelligence,
  //    vol.26, no.5, pages 651-656, 2004.
  //------------------------------------------------------------------

#if CV_MAJOR_VERSION == 2
  mog(img_input, img_foreground, alpha);
  mog.getBackgroundImage(img_background);
#elif CV_MAJOR_VERSION >= 3
  mog->apply(img_input, img_foreground, alpha);
  mog->getBackgroundImage(img_background);
#endif

  if (enableThreshold)
    cv::threshold(img_foreground, img_foreground, threshold, 255, cv::THRESH_BINARY);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("GMM FG (Zivkovic&Heijden)", img_foreground);
    cv::imshow("GMM BG (Zivkovic&Heijden)", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void MixtureOfGaussianV2::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteReal(fs, "alpha", alpha);
  cvWriteInt(fs, "enableThreshold", enableThreshold);
  cvWriteInt(fs, "threshold", threshold);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void MixtureOfGaussianV2::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  alpha = cvReadRealByName(fs, nullptr, "alpha", 0.05);
  enableThreshold = cvReadIntByName(fs, nullptr, "enableThreshold", true);
  threshold = cvReadIntByName(fs, nullptr, "threshold", 15);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
