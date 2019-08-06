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
  
  final String KDE = "KDE.xml";
  final String IndependentMultimodalBGS = "IndependentMultimodalBGS.xml";
  final String MultiCueBGS = "MultiCueBGS.xml";
  final String SigmaDeltaBGS = "SigmaDeltaBGS.xml";
  final String SuBSENSEBGS = "SuBSENSEBGS.xml";
  final String LOBSTERBGS = "LOBSTERBGS.xml";
}
