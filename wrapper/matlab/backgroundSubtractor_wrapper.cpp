#include "opencvmex.hpp"

#include <memory>

#include <typeinfo>
#include "mxarray.h"

#include "IBGS.h"
#include "FrameDifference.h"
#include "StaticFrameDifference.h"
#include "WeightedMovingMean.h"
#include "WeightedMovingVariance.h"
#include "MixtureOfGaussianV1.h"
#include "MixtureOfGaussianV2.h"
#include "AdaptiveBackgroundLearning.h"
#include "AdaptiveSelectiveBackgroundLearning.h"
#include "GMG.h"
#include "KNN.h"
#include "DPAdaptiveMedian.h"
#include "DPGrimsonGMM.h"
#include "DPZivkovicAGMM.h"
#include "DPMean.h"
#include "DPWrenGA.h"
#include "DPPratiMediod.h"
#include "DPEigenbackground.h"
#include "DPTexture.h"
#include "T2FGMM_UM.h"
#include "T2FGMM_UV.h"
#include "T2FMRF_UM.h"
#include "T2FMRF_UV.h"
#include "FuzzySugenoIntegral.h"
#include "FuzzyChoquetIntegral.h"
#include "LBSimpleGaussian.h"
#include "LBFuzzyGaussian.h"
#include "LBMixtureOfGaussians.h"
#include "LBAdaptiveSOM.h"
#include "LBFuzzyAdaptiveSOM.h"
#include "LBP_MRF.h"
#include "MultiLayer.h"
#include "PixelBasedAdaptiveSegmenter.h"
#include "VuMeter.h"
#include "KDE.h"
#include "IndependentMultimodal.h"
#include "MultiCue.h"
#include "SigmaDelta.h"
#include "SuBSENSE.h"
#include "LOBSTER.h"
#include "PAWCS.h"
#include "TwoPoints.h"
#include "ViBe.h"
#include "CodeBook.h"

using namespace bgslibrary::algorithms;

static IBGS *ptrBGS = NULL;

namespace bgslibrary
{
  IBGS* init_alg(std::string alg_name)
  {
    if (alg_name.compare("FrameDifference") == 0)
      return (IBGS *)mxCalloc(1, sizeof(FrameDifference));
    if (alg_name.compare("StaticFrameDifference") == 0)
      return (IBGS *)mxCalloc(1, sizeof(StaticFrameDifference));
    if (alg_name.compare("WeightedMovingMean") == 0)
      return (IBGS *)mxCalloc(1, sizeof(WeightedMovingMean));
    if (alg_name.compare("WeightedMovingVariance") == 0)
      return (IBGS *)mxCalloc(1, sizeof(WeightedMovingVariance));
#if CV_MAJOR_VERSION == 2
    if (alg_name.compare("MixtureOfGaussianV1") == 0)
      return (IBGS *)mxCalloc(1, sizeof(MixtureOfGaussianV1)); // only for OpenCV 2.x
#endif
    if (alg_name.compare("MixtureOfGaussianV2") == 0)
      return (IBGS *)mxCalloc(1, sizeof(MixtureOfGaussianV2));
    if (alg_name.compare("AdaptiveBackgroundLearning") == 0)
      return (IBGS *)mxCalloc(1, sizeof(AdaptiveBackgroundLearning));
    if (alg_name.compare("AdaptiveSelectiveBackgroundLearning") == 0)
      return (IBGS *)mxCalloc(1, sizeof(AdaptiveSelectiveBackgroundLearning));
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    if (alg_name.compare("GMG") == 0)
      return (IBGS *)mxCalloc(1, sizeof(GMG)); // only for OpenCV >= 2.4.3
#endif
#if CV_MAJOR_VERSION >= 3
    if (alg_name.compare("KNN") == 0)
      return (IBGS *)mxCalloc(1, sizeof(KNN)); // only for OpenCV 3.x
#endif
    if (alg_name.compare("DPAdaptiveMedian") == 0)
      return (IBGS *)mxCalloc(1, sizeof(DPAdaptiveMedian));
    if (alg_name.compare("DPGrimsonGMM") == 0)
      return (IBGS *)mxCalloc(1, sizeof(DPGrimsonGMM));
    if (alg_name.compare("DPZivkovicAGMM") == 0)
      return (IBGS *)mxCalloc(1, sizeof(DPZivkovicAGMM));
    if (alg_name.compare("DPMean") == 0)
      return (IBGS *)mxCalloc(1, sizeof(DPMean));
    if (alg_name.compare("DPWrenGA") == 0)
      return (IBGS *)mxCalloc(1, sizeof(DPWrenGA));
    if (alg_name.compare("DPPratiMediod") == 0)
      return (IBGS *)mxCalloc(1, sizeof(DPPratiMediod));
    if (alg_name.compare("DPEigenbackground") == 0)
      return (IBGS *)mxCalloc(1, sizeof(DPEigenbackground));
    if (alg_name.compare("DPTexture") == 0)
      return (IBGS *)mxCalloc(1, sizeof(DPTexture));
    if (alg_name.compare("T2FGMM_UM") == 0)
      return (IBGS *)mxCalloc(1, sizeof(T2FGMM_UM));
    if (alg_name.compare("T2FGMM_UV") == 0)
      return (IBGS *)mxCalloc(1, sizeof(T2FGMM_UV));
    if (alg_name.compare("T2FMRF_UM") == 0)
      return (IBGS *)mxCalloc(1, sizeof(T2FMRF_UM));
    if (alg_name.compare("T2FMRF_UV") == 0)
      return (IBGS *)mxCalloc(1, sizeof(T2FMRF_UV));
    if (alg_name.compare("FuzzySugenoIntegral") == 0)
      return (IBGS *)mxCalloc(1, sizeof(FuzzySugenoIntegral));
    if (alg_name.compare("FuzzyChoquetIntegral") == 0)
      return (IBGS *)mxCalloc(1, sizeof(FuzzyChoquetIntegral));
    if (alg_name.compare("LBSimpleGaussian") == 0)
      return (IBGS *)mxCalloc(1, sizeof(LBSimpleGaussian));
    if (alg_name.compare("LBFuzzyGaussian") == 0)
      return (IBGS *)mxCalloc(1, sizeof(LBFuzzyGaussian));
    if (alg_name.compare("LBMixtureOfGaussians") == 0)
      return (IBGS *)mxCalloc(1, sizeof(LBMixtureOfGaussians));
    if (alg_name.compare("LBAdaptiveSOM") == 0)
      return (IBGS *)mxCalloc(1, sizeof(LBAdaptiveSOM));
    if (alg_name.compare("LBFuzzyAdaptiveSOM") == 0)
      return (IBGS *)mxCalloc(1, sizeof(LBFuzzyAdaptiveSOM));
    if (alg_name.compare("LBP_MRF") == 0)
      return (IBGS *)mxCalloc(1, sizeof(LBP_MRF));
    if (alg_name.compare("MultiLayer") == 0)
      return (IBGS *)mxCalloc(1, sizeof(MultiLayer));
    if (alg_name.compare("PixelBasedAdaptiveSegmenter") == 0)
      return (IBGS *)mxCalloc(1, sizeof(PixelBasedAdaptiveSegmenter));
    if (alg_name.compare("VuMeter") == 0)
      return (IBGS *)mxCalloc(1, sizeof(VuMeter));
    if (alg_name.compare("KDE") == 0)
      return (IBGS *)mxCalloc(1, sizeof(KDE));
    if (alg_name.compare("IndependentMultimodal") == 0)
      return (IBGS *)mxCalloc(1, sizeof(IndependentMultimodal));
    if (alg_name.compare("MultiCue") == 0)
      return (IBGS *)mxCalloc(1, sizeof(MultiCue));
    if (alg_name.compare("SigmaDelta") == 0)
      return (IBGS *)mxCalloc(1, sizeof(SigmaDelta));
    if (alg_name.compare("SuBSENSE") == 0)
      return (IBGS *)mxCalloc(1, sizeof(SuBSENSE));
    if (alg_name.compare("LOBSTER") == 0)
      return (IBGS *)mxCalloc(1, sizeof(LOBSTER));
    if (alg_name.compare("PAWCS") == 0)
      return (IBGS *)mxCalloc(1, sizeof(PAWCS));
    if (alg_name.compare("TwoPoints") == 0)
      return (IBGS *)mxCalloc(1, sizeof(TwoPoints));
    if (alg_name.compare("ViBe") == 0)
      return (IBGS *)mxCalloc(1, sizeof(ViBe));
    if (alg_name.compare("CodeBook") == 0)
      return (IBGS *)mxCalloc(1, sizeof(CodeBook));
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

//////////////////////////////////////////////////////////////////////////////
// Check inputs
//////////////////////////////////////////////////////////////////////////////
void checkInputs(int nrhs, const mxArray *prhs[])
{
  //mexPrintf("checkInputs()\n");
  if ((nrhs < 1) || (nrhs > 2))
    mexErrMsgTxt("Incorrect number of inputs. Function expects 1 or 2 inputs.");
}

//////////////////////////////////////////////////////////////////////////////
// Get MEX function inputs
//////////////////////////////////////////////////////////////////////////////
void getParams(std::string &algorithm, const mxArray* mxParams)
{
  //mexPrintf("getParams()\n");
  const mxArray* mxfield;
  mxfield = mxGetField(mxParams, 0, "algorithm");
  algorithm = mexplus::MxArray::to<std::string>(mxfield);
  //mexPrintf(algorithm.c_str());
  //mexPrintf("\n");
}

//////////////////////////////////////////////////////////////////////////////
// Exit function for freeing persistent memory
//////////////////////////////////////////////////////////////////////////////
void exitFcn()
{
  //mexPrintf("exitFcn()\n");
  if (ptrBGS != NULL)
  {
    // explicitly call destructor for "placement new"
    ptrBGS->~IBGS();
    mxFree(ptrBGS);
    ptrBGS = NULL;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Construct object
//////////////////////////////////////////////////////////////////////////////
void constructObject(const mxArray *prhs[])
{
  //mexPrintf("constructObject()\n");
  std::string algorithm;

  // second input must be struct
  if (mxIsStruct(prhs[1]))
    getParams(algorithm, prhs[1]);

  if (ptrBGS != NULL) exitFcn();

  // Allocate memory for background subtractor model
  //ptrBGS = (IBGS *)mxCalloc(1, sizeof(IBGS));
  ptrBGS = bgslibrary::init_alg(algorithm);
  if (ptrBGS != NULL)
  {
    // Make memory allocated by MATLAB software persist after MEX-function completes.
    // This lets us use the updated background subtractor model for the next frame.
    mexMakeMemoryPersistent(ptrBGS);

    // Use "placement new" to construct an object on memory that was
    // already allocated using mxCalloc
    ptrBGS = bgslibrary::get_alg(algorithm);
    if (ptrBGS == NULL)
      mexErrMsgTxt("Failed to construct an object on memory. Algorithm not initialized.");
  }
  else
    mexErrMsgTxt("Failed to allocate memory. Algorithm not found?");

  // Register a function that gets called when the MEX-function is cleared.
  // This function is responsible for freeing persistent memory
  mexAtExit(exitFcn);
}

//////////////////////////////////////////////////////////////////////////////
// Compute foreground mask
//////////////////////////////////////////////////////////////////////////////
void computeForegroundMask(mxArray *plhs[], const mxArray *prhs[])
{
  //mexPrintf("computeForegroundMask()\n");
  if (ptrBGS != NULL)
  {
    cv::Ptr<cv::Mat> img = ocvMxArrayToImage_uint8(prhs[1], true);
    cv::Mat fgmask, bgmodel;

    ptrBGS->process(*img, fgmask, bgmodel);

    if (fgmask.empty())
      fgmask = cv::Mat::zeros((*img).size(), CV_8UC1);
    if (bgmodel.empty())
      bgmodel = cv::Mat::zeros((*img).size(), CV_8UC3); // (*img).type()

    // https://fr.mathworks.com/help/vision/ref/ocvmxarrayfromimage_datatype.html
    plhs[0] = ocvMxArrayFromImage_uint8(fgmask);
    plhs[1] = ocvMxArrayFromImage_uint8(bgmodel);

    //if(!fgmask.empty())
    fgmask.release();
    //if(!bgmodel.empty())
    bgmodel.release();
  }
  else
    mexErrMsgTxt("Algorithm not initialized.");
}

//////////////////////////////////////////////////////////////////////////////
// The main MEX function entry point
//////////////////////////////////////////////////////////////////////////////
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  //mexPrintf("mexFunction()\n");
  checkInputs(nrhs, prhs);
  const char *str = mxIsChar(prhs[0]) ? mxArrayToString(prhs[0]) : NULL;

  if (str != NULL)
  {
    //mexPrintf("%s\n",str);
    if (strcmp(str, "construct") == 0)
      constructObject(prhs);
    else if (strcmp(str, "compute") == 0)
      computeForegroundMask(plhs, prhs);
    else if (strcmp(str, "destroy") == 0)
      exitFcn();

    // Free memory allocated by mxArrayToString
    mxFree((void *)str);
  }
}
