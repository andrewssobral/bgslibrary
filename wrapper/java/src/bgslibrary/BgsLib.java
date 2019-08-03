package bgslibrary;

import org.opencv.core.Core;

public class BgsLib
{
  static
  {
    System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
    System.loadLibrary("libbgs_java_module");
  }
  
  private static final String[] algorithms =
    {
      "FrameDifference", "StaticFrameDifference", "WeightedMovingMean", 
      "WeightedMovingVariance", "MixtureOfGaussianV1", "MixtureOfGaussianV2", 
      "AdaptiveBackgroundLearning", "AdaptiveSelectiveBackgroundLearning", 
      "GMG", "KNN", "DPAdaptiveMedian", "DPGrimsonGMM", "DPZivkovicAGMM", 
      "DPMean", "DPWrenGA", "DPPratiMediod", "DPEigenbackground", "DPTexture", 
      "T2FGMM_UM", "T2FGMM_UV", "T2FMRF_UM", "T2FMRF_UV", "FuzzySugenoIntegral", 
      "FuzzyChoquetIntegral", "LBSimpleGaussian", "LBFuzzyGaussian", 
      "LBMixtureOfGaussians", "LBAdaptiveSOM", "LBFuzzyAdaptiveSOM", "LBP_MRF", 
      "MultiLayer", "PixelBasedAdaptiveSegmenter", "VuMeter", "KDE", 
      "IndependentMultimodal", "MultiCue", "SigmaDelta", "SuBSENSE", "LOBSTER", 
      "PAWCS", "TwoPoints", "ViBe", "CodeBook"
    };
  
  public static final String[] getListAlgorithms()
  {
    return algorithms;
  }

  public static native void constructObject(String str);

  public static native void computeForegroundMask(long input_matPtr, long output_matPtr);

  public static native void destroyObject();
}
