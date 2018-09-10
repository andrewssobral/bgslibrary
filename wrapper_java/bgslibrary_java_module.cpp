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
#include <iostream>
#include <opencv2/opencv.hpp>

#include "bgslibrary_java_module.hpp"

static IBGS *ptrBGS = nullptr;
static std::string algorithm = "";

namespace bgslibrary
{
  IBGS* init_alg(std::string alg_name)
  {
    if (alg_name.compare("FrameDifference") == 0)
      return (IBGS *)malloc(sizeof(FrameDifference));
    if (alg_name.compare("StaticFrameDifference") == 0)
      return (IBGS *)malloc(sizeof(StaticFrameDifference));
    if (alg_name.compare("WeightedMovingMean") == 0)
      return (IBGS *)malloc(sizeof(WeightedMovingMean));
    if (alg_name.compare("WeightedMovingVariance") == 0)
      return (IBGS *)malloc(sizeof(WeightedMovingVariance));
#if CV_MAJOR_VERSION == 2
    if (alg_name.compare("MixtureOfGaussianV1") == 0)
      return (IBGS *)malloc(sizeof(MixtureOfGaussianV1)); // only for OpenCV 2.x
#endif
    if (alg_name.compare("MixtureOfGaussianV2") == 0)
      return (IBGS *)malloc(sizeof(MixtureOfGaussianV2));
    if (alg_name.compare("AdaptiveBackgroundLearning") == 0)
      return (IBGS *)malloc(sizeof(AdaptiveBackgroundLearning));
    if (alg_name.compare("AdaptiveSelectiveBackgroundLearning") == 0)
      return (IBGS *)malloc(sizeof(AdaptiveSelectiveBackgroundLearning));
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    if (alg_name.compare("GMG") == 0)
      return (IBGS *)malloc(sizeof(GMG)); // only for OpenCV >= 2.4.3
#endif
#if CV_MAJOR_VERSION >= 3
    if (alg_name.compare("KNN") == 0)
      return (IBGS *)malloc(sizeof(KNN)); // only for OpenCV 3.x
#endif
    if (alg_name.compare("DPAdaptiveMedian") == 0)
      return (IBGS *)malloc(sizeof(DPAdaptiveMedian));
    if (alg_name.compare("DPGrimsonGMM") == 0)
      return (IBGS *)malloc(sizeof(DPGrimsonGMM));
    if (alg_name.compare("DPZivkovicAGMM") == 0)
      return (IBGS *)malloc(sizeof(DPZivkovicAGMM));
    if (alg_name.compare("DPMean") == 0)
      return (IBGS *)malloc(sizeof(DPMean));
    if (alg_name.compare("DPWrenGA") == 0)
      return (IBGS *)malloc(sizeof(DPWrenGA));
    if (alg_name.compare("DPPratiMediod") == 0)
      return (IBGS *)malloc(sizeof(DPPratiMediod));
    if (alg_name.compare("DPEigenbackground") == 0)
      return (IBGS *)malloc(sizeof(DPEigenbackground));
    if (alg_name.compare("DPTexture") == 0)
      return (IBGS *)malloc(sizeof(DPTexture));
    if (alg_name.compare("T2FGMM_UM") == 0)
      return (IBGS *)malloc(sizeof(T2FGMM_UM));
    if (alg_name.compare("T2FGMM_UV") == 0)
      return (IBGS *)malloc(sizeof(T2FGMM_UV));
    if (alg_name.compare("T2FMRF_UM") == 0)
      return (IBGS *)malloc(sizeof(T2FMRF_UM));
    if (alg_name.compare("T2FMRF_UV") == 0)
      return (IBGS *)malloc(sizeof(T2FMRF_UV));
    if (alg_name.compare("FuzzySugenoIntegral") == 0)
      return (IBGS *)malloc(sizeof(FuzzySugenoIntegral));
    if (alg_name.compare("FuzzyChoquetIntegral") == 0)
      return (IBGS *)malloc(sizeof(FuzzyChoquetIntegral));
    if (alg_name.compare("LBSimpleGaussian") == 0)
      return (IBGS *)malloc(sizeof(LBSimpleGaussian));
    if (alg_name.compare("LBFuzzyGaussian") == 0)
      return (IBGS *)malloc(sizeof(LBFuzzyGaussian));
    if (alg_name.compare("LBMixtureOfGaussians") == 0)
      return (IBGS *)malloc(sizeof(LBMixtureOfGaussians));
    if (alg_name.compare("LBAdaptiveSOM") == 0)
      return (IBGS *)malloc(sizeof(LBAdaptiveSOM));
    if (alg_name.compare("LBFuzzyAdaptiveSOM") == 0)
      return (IBGS *)malloc(sizeof(LBFuzzyAdaptiveSOM));
    if (alg_name.compare("LBP_MRF") == 0)
      return (IBGS *)malloc(sizeof(LBP_MRF));
    if (alg_name.compare("MultiLayer") == 0)
      return (IBGS *)malloc(sizeof(MultiLayer));
    if (alg_name.compare("PixelBasedAdaptiveSegmenter") == 0)
      return (IBGS *)malloc(sizeof(PixelBasedAdaptiveSegmenter));
    if (alg_name.compare("VuMeter") == 0)
      return (IBGS *)malloc(sizeof(VuMeter));
    if (alg_name.compare("KDE") == 0)
      return (IBGS *)malloc(sizeof(KDE));
    if (alg_name.compare("IndependentMultimodal") == 0)
      return (IBGS *)malloc(sizeof(IndependentMultimodal));
    if (alg_name.compare("MultiCue") == 0)
      return (IBGS *)malloc(sizeof(MultiCue));
    if (alg_name.compare("SigmaDelta") == 0)
      return (IBGS *)malloc(sizeof(SigmaDelta));
    if (alg_name.compare("SuBSENSE") == 0)
      return (IBGS *)malloc(sizeof(SuBSENSE));
    if (alg_name.compare("LOBSTER") == 0)
      return (IBGS *)malloc(sizeof(LOBSTER));
    if (alg_name.compare("PAWCS") == 0)
      return (IBGS *)malloc(sizeof(PAWCS));
    if (alg_name.compare("TwoPoints") == 0)
      return (IBGS *)malloc(sizeof(TwoPoints));
    if (alg_name.compare("ViBe") == 0)
      return (IBGS *)malloc(sizeof(ViBe));
    if (alg_name.compare("CodeBook") == 0)
      return (IBGS *)malloc(sizeof(CodeBook));
    return NULL;
  }

  IBGS* get_alg(std::string alg_name)
  {
    if (alg_name.compare("FrameDifference") == 0)
      return new (ptrBGS) FrameDifference();
    if (alg_name.compare("StaticFrameDifference") == 0)
      return new (ptrBGS) StaticFrameDifference();
    if (alg_name.compare("WeightedMovingMean") == 0)
      return new (ptrBGS) WeightedMovingMean();
    if (alg_name.compare("WeightedMovingVariance") == 0)
      return new (ptrBGS) WeightedMovingVariance();
#if CV_MAJOR_VERSION == 2
    if (alg_name.compare("MixtureOfGaussianV1") == 0)
      return new (ptrBGS) MixtureOfGaussianV1(); // only for OpenCV 2.x
#endif
    if (alg_name.compare("MixtureOfGaussianV2") == 0)
      return new (ptrBGS) MixtureOfGaussianV2();
    if (alg_name.compare("AdaptiveBackgroundLearning") == 0)
      return new (ptrBGS) AdaptiveBackgroundLearning();
    if (alg_name.compare("AdaptiveSelectiveBackgroundLearning") == 0)
      return new (ptrBGS) AdaptiveSelectiveBackgroundLearning();
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    if (alg_name.compare("GMG") == 0)
      return new (ptrBGS) GMG(); // only for OpenCV >= 2.4.3
#endif
#if CV_MAJOR_VERSION >= 3
    if (alg_name.compare("KNN") == 0)
      return new (ptrBGS) KNN(); // only on OpenCV 3.x
#endif
    if (alg_name.compare("DPAdaptiveMedian") == 0)
      return new (ptrBGS) DPAdaptiveMedian();
    if (alg_name.compare("DPGrimsonGMM") == 0)
      return new (ptrBGS) DPGrimsonGMM();
    if (alg_name.compare("DPZivkovicAGMM") == 0)
      return new (ptrBGS) DPZivkovicAGMM();
    if (alg_name.compare("DPMean") == 0)
      return new (ptrBGS) DPMean();
    if (alg_name.compare("DPWrenGA") == 0)
      return new (ptrBGS) DPWrenGA();
    if (alg_name.compare("DPPratiMediod") == 0)
      return new (ptrBGS) DPPratiMediod();
    if (alg_name.compare("DPEigenbackground") == 0)
      return new (ptrBGS) DPEigenbackground();
    if (alg_name.compare("DPTexture") == 0)
      return new (ptrBGS) DPTexture();
    if (alg_name.compare("T2FGMM_UM") == 0)
      return new (ptrBGS) T2FGMM_UM();
    if (alg_name.compare("T2FGMM_UV") == 0)
      return new (ptrBGS) T2FGMM_UV();
    if (alg_name.compare("T2FMRF_UM") == 0)
      return new (ptrBGS) T2FMRF_UM();
    if (alg_name.compare("T2FMRF_UV") == 0)
      return new (ptrBGS) T2FMRF_UV();
    if (alg_name.compare("FuzzySugenoIntegral") == 0)
      return new (ptrBGS) FuzzySugenoIntegral();
    if (alg_name.compare("FuzzyChoquetIntegral") == 0)
      return new (ptrBGS) FuzzyChoquetIntegral();
    if (alg_name.compare("LBSimpleGaussian") == 0)
      return new (ptrBGS) LBSimpleGaussian();
    if (alg_name.compare("LBFuzzyGaussian") == 0)
      return new (ptrBGS) LBFuzzyGaussian();
    if (alg_name.compare("LBMixtureOfGaussians") == 0)
      return new (ptrBGS) LBMixtureOfGaussians();
    if (alg_name.compare("LBAdaptiveSOM") == 0)
      return new (ptrBGS) LBAdaptiveSOM();
    if (alg_name.compare("LBFuzzyAdaptiveSOM") == 0)
      return new (ptrBGS) LBFuzzyAdaptiveSOM();
    if (alg_name.compare("LBP_MRF") == 0)
      return new (ptrBGS) LBP_MRF();
    if (alg_name.compare("MultiLayer") == 0)
      return new (ptrBGS) MultiLayer();
    if (alg_name.compare("PixelBasedAdaptiveSegmenter") == 0)
      return new (ptrBGS) PixelBasedAdaptiveSegmenter();
    if (alg_name.compare("VuMeter") == 0)
      return new (ptrBGS) VuMeter();
    if (alg_name.compare("KDE") == 0)
      return new (ptrBGS) KDE();
    if (alg_name.compare("IndependentMultimodal") == 0)
      return new (ptrBGS) IndependentMultimodal();
    if (alg_name.compare("MultiCue") == 0)
      return new (ptrBGS) MultiCue();
    if (alg_name.compare("SigmaDelta") == 0)
      return new (ptrBGS) SigmaDelta();
    if (alg_name.compare("SuBSENSE") == 0)
      return new (ptrBGS) SuBSENSE();
    if (alg_name.compare("LOBSTER") == 0)
      return new (ptrBGS) LOBSTER();
    if (alg_name.compare("PAWCS") == 0)
      return new (ptrBGS) PAWCS();
    if (alg_name.compare("TwoPoints") == 0)
      return new (ptrBGS) TwoPoints();
    if (alg_name.compare("ViBe") == 0)
      return new (ptrBGS) ViBe();
    if (alg_name.compare("CodeBook") == 0)
      return new (ptrBGS) CodeBook();
    return NULL;
  }
}

bool constructObject(std::string algorithm)
{
  if (ptrBGS != nullptr) destroyObject();

  ptrBGS = bgslibrary::init_alg(algorithm);
  if (ptrBGS != nullptr)
  {
    ptrBGS = bgslibrary::get_alg(algorithm);
    if (ptrBGS == nullptr)
    {
      std::cout << "Failed to construct an object on memory. Algorithm not initialized." << std::endl;
      return false;
    }
    else
    {
      ptrBGS->setShowOutput(false);
      return true;
    }
  }
  else
  {
    std::cout << "Failed to allocate memory. Algorithm not found?" << std::endl;
    return false;
  }
}

void computeForegroundMask(const cv::Mat &img_input, cv::Mat &img_output)
{
  if (ptrBGS != nullptr)
  {
    cv::Mat fgmask, bgmodel;

    ptrBGS->process(img_input, fgmask, bgmodel);

    if (fgmask.empty())
      fgmask = cv::Mat::zeros(img_input.size(), CV_8UC1);
    if (bgmodel.empty())
      bgmodel = cv::Mat::zeros(img_input.size(), CV_8UC3);

    fgmask.copyTo(img_output);

    fgmask.release();
    bgmodel.release();
  }
  else
    std::cout << "Algorithm not initialized." << std::endl;
}

void destroyObject()
{
  if (ptrBGS != nullptr)
  {
    // explicitly call destructor for "placement new"
    ptrBGS->~IBGS();
    free(ptrBGS);
    ptrBGS = nullptr;
  }
}

JNIEXPORT void JNICALL Java_bgslibrary_BgsLib_constructObject(JNIEnv *env, jclass, jstring jstr)
{
  GetJStringContent(env, jstr, algorithm);
  constructObject(algorithm);
}

JNIEXPORT void JNICALL Java_bgslibrary_BgsLib_computeForegroundMask
(JNIEnv *, jclass, jlong input_matPtr, jlong output_matPtr)
{
  cv::Mat& in_mat = *reinterpret_cast<cv::Mat*>(input_matPtr);
  cv::Mat& fg_mat = *reinterpret_cast<cv::Mat*>(output_matPtr);
  computeForegroundMask(in_mat, fg_mat);
}

JNIEXPORT void JNICALL Java_bgslibrary_BgsLib_destroyObject(JNIEnv *, jclass)
{
  destroyObject();
}
