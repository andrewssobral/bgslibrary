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
package br.com.bgslibrary.entity;

public interface Configuration
{
  final String FrameProcessor = "FrameProcessor.xml";
  final String VideoCapture = "VideoCapture.xml";
  final String PreProcessor = "PreProcessor.xml";
  final String ForegroundMaskAnalysis = "ForegroundMaskAnalysis.xml";
  
  final String FrameDifferenceBGS = "FrameDifferenceBGS.xml";
  final String StaticFrameDifferenceBGS = "StaticFrameDifferenceBGS.xml";
  final String WeightedMovingMeanBGS = "WeightedMovingMeanBGS.xml";
  final String WeightedMovingVarianceBGS = "WeightedMovingVarianceBGS.xml";
  final String AdaptiveBackgroundLearning = "AdaptiveBackgroundLearning.xml";
  final String MixtureOfGaussianV1BGS = "MixtureOfGaussianV1BGS.xml";
  final String MixtureOfGaussianV2BGS = "MixtureOfGaussianV2BGS.xml";
  final String GMG = "GMG.xml";
  
  final String DPAdaptiveMedianBGS = "DPAdaptiveMedianBGS.xml";
  final String DPEigenbackgroundBGS = "DPEigenbackgroundBGS.xml";
  final String DPGrimsonGMMBGS = "DPGrimsonGMMBGS.xml";
  final String DPMeanBGS = "DPMeanBGS.xml";
  final String DPPratiMediodBGS = "DPPratiMediodBGS.xml";
  final String DPWrenGABGS = "DPWrenGABGS.xml";
  final String DPZivkovicAGMMBGS = "DPZivkovicAGMMBGS.xml";
  
  final String LBAdaptiveSOM = "LBAdaptiveSOM.xml";
  final String LBFuzzyAdaptiveSOM = "LBFuzzyAdaptiveSOM.xml";
  final String LBFuzzyGaussian = "LBFuzzyGaussian.xml";
  final String LBMixtureOfGaussians = "LBMixtureOfGaussians.xml";
  final String LBSimpleGaussian = "LBSimpleGaussian.xml";
  
  final String MultiLayerBGS = "MultiLayerBGS.xml";
  
  //final String PBAS = "PixelBasedAdaptiveSegmenter.xml";
  
  final String VuMeter = "VuMeter.xml";
  
  final String T2FGMM_UM = "T2FGMM_UM.xml";
  final String T2FGMM_UV = "T2FGMM_UV.xml";
  final String T2FMRF_UM = "T2FMRF_UM.xml";
  final String T2FMRF_UV = "T2FMRF_UV.xml";
  final String FuzzySugenoIntegral = "FuzzySugenoIntegral.xml";
  final String FuzzyChoquetIntegral = "FuzzyChoquetIntegral.xml";
}
