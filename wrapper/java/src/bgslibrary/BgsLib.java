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
