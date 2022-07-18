#include <iomanip>

#include "FrameProcessor.h"

namespace bgslibrary
{
  FrameProcessor::FrameProcessor() : 
    firstTime(true), frameNumber(0), duration(0), 
    tictoc(""), frameToStop(0)
  {
    debug_construction(FrameProcessor);
    initLoadSaveConfig(quote(FrameProcessor));
  }

  FrameProcessor::~FrameProcessor() {
    debug_destruction(FrameProcessor);
  }

  void FrameProcessor::init()
  {
    if (enablePreProcessor)
      preProcessor = std::make_unique<PreProcessor>();

    if (enableFrameDifference)
      frameDifference = std::make_shared<FrameDifference>();

    if (enableStaticFrameDifference)
      staticFrameDifference = std::make_shared<StaticFrameDifference>();

    if (enableWeightedMovingMean)
      weightedMovingMean = std::make_shared<WeightedMovingMean>();

    if (enableWeightedMovingVariance)
      weightedMovingVariance = std::make_shared<WeightedMovingVariance>();

    if (enableAdaptiveBackgroundLearning)
      adaptiveBackgroundLearning = std::make_shared<AdaptiveBackgroundLearning>();

    if (enableAdaptiveSelectiveBackgroundLearning)
      adaptiveSelectiveBackgroundLearning = std::make_shared<AdaptiveSelectiveBackgroundLearning>();

    if (enableMixtureOfGaussianV2)
      mixtureOfGaussianV2 = std::make_shared<MixtureOfGaussianV2>();

#if CV_MAJOR_VERSION == 2
    if (enableMixtureOfGaussianV1)
      mixtureOfGaussianV1 = std::make_shared<MixtureOfGaussianV1>();
#endif

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    if (enableGMG)
      gmg = std::make_shared<GMG>();
#endif

#if CV_MAJOR_VERSION >= 3
    if (enableKNN)
      knn = std::make_shared<KNN>();
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3
    if (enableDPAdaptiveMedian)
      dpAdaptiveMedian = std::make_shared<DPAdaptiveMedian>();

    if (enableDPGrimsonGMM)
      dpGrimsonGMM = std::make_shared<DPGrimsonGMM>();

    if (enableDPZivkovicAGMM)
      dpZivkovicAGMM = std::make_shared<DPZivkovicAGMM>();

    if (enableDPMean)
      dpTemporalMean = std::make_shared<DPMean>();

    if (enableDPWrenGA)
      dpWrenGA = std::make_shared<DPWrenGA>();

    if (enableDPPratiMediod)
      dpPratiMediod = std::make_shared<DPPratiMediod>();

    if (enableDPEigenbackground)
      dpEigenBackground = std::make_shared<DPEigenbackground>();

    if (enableDPTexture)
      dpTexture = std::make_shared<DPTexture>();

    if (enableT2FGMM_UM)
      type2FuzzyGMM_UM = std::make_shared<T2FGMM_UM>();

    if (enableT2FGMM_UV)
      type2FuzzyGMM_UV = std::make_shared<T2FGMM_UV>();

    if (enableT2FMRF_UM)
      type2FuzzyMRF_UM = std::make_shared<T2FMRF_UM>();

    if (enableT2FMRF_UV)
      type2FuzzyMRF_UV = std::make_shared<T2FMRF_UV>();

    if (enableFuzzySugenoIntegral)
      fuzzySugenoIntegral = std::make_shared<FuzzySugenoIntegral>();

    if (enableFuzzyChoquetIntegral)
      fuzzyChoquetIntegral = std::make_shared<FuzzyChoquetIntegral>();

    if (enableLBSimpleGaussian)
      lbSimpleGaussian = std::make_shared<LBSimpleGaussian>();

    if (enableLBFuzzyGaussian)
      lbFuzzyGaussian = std::make_shared<LBFuzzyGaussian>();

    if (enableLBMixtureOfGaussians)
      lbMixtureOfGaussians = std::make_shared<LBMixtureOfGaussians>();

    if (enableLBAdaptiveSOM)
      lbAdaptiveSOM = std::make_shared<LBAdaptiveSOM>();

    if (enableLBFuzzyAdaptiveSOM)
      lbFuzzyAdaptiveSOM = std::make_shared<LBFuzzyAdaptiveSOM>();

    if (enablePBAS)
      pixelBasedAdaptiveSegmenter = std::make_shared<PixelBasedAdaptiveSegmenter>();

    if (enableVuMeter)
      vuMeter = std::make_shared<VuMeter>();

    if (enableKDE)
      kde = std::make_shared<KDE>();

    if (enableIMBS)
      imbs = std::make_shared<IndependentMultimodal>();

    if (enableMultiCue)
      multiCue = std::make_shared<MultiCue>();
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7
    if (enableLbpMrf)
      lbpMrf = std::make_shared<LBP_MRF>();

    if (enableMultiLayer)
      multiLayer = std::make_shared<MultiLayer>();
#endif

    if (enableSigmaDelta)
      sigmaDelta = std::make_shared<SigmaDelta>();

    if (enableSuBSENSE)
      subSENSE = std::make_shared<SuBSENSE>();

    if (enableLOBSTER)
      lobster = std::make_shared<LOBSTER>();

    if (enablePAWCS)
      pawcs = std::make_shared<PAWCS>();

    if (enableTwoPoints)
      twoPoints = std::make_shared<TwoPoints>();

    if (enableViBe)
      vibe = std::make_shared<ViBe>();

    if (enableCodeBook)
      codeBook = std::make_shared<CodeBook>();

    if (enableForegroundMaskAnalysis)
      foregroundMaskAnalysis = std::make_shared<tools::ForegroundMaskAnalysis>();
  }
  
  void FrameProcessor::process(const std::string name, const std::shared_ptr<IBGS> &bgs, const cv::Mat &img_input, cv::Mat &img_bgs)
  {
    if (tictoc == name)
      tic(name);
    
    cv::Mat img_bkgmodel;
    bgs->process(img_input, img_bgs, img_bkgmodel);
    
    if (tictoc == name)
      toc();
  }
  
  void FrameProcessor::process(const cv::Mat &img_input)
  {
    frameNumber++;

    if (enablePreProcessor)
      preProcessor->process(img_input, img_preProcessor);
    else
      img_input.copyTo(img_preProcessor);

    if (enableFrameDifference)
      process("FrameDifference", frameDifference, img_preProcessor, img_frameDifference);

    if (enableStaticFrameDifference)
      process("StaticFrameDifference", staticFrameDifference, img_preProcessor, img_staticFrameDifference);

    if (enableWeightedMovingMean)
      process("WeightedMovingMean", weightedMovingMean, img_preProcessor, img_weightedMovingMean);

    if (enableWeightedMovingVariance)
      process("WeightedMovingVariance", weightedMovingVariance, img_preProcessor, img_weightedMovingVariance);

    if (enableAdaptiveBackgroundLearning)
      process("AdaptiveBackgroundLearning", adaptiveBackgroundLearning, img_preProcessor, img_adaptiveBackgroundLearning);

    if (enableAdaptiveSelectiveBackgroundLearning)
      process("AdaptiveSelectiveBackgroundLearning", adaptiveSelectiveBackgroundLearning, img_preProcessor, img_adaptiveSelectiveBackgroundLearning);

    if (enableMixtureOfGaussianV2)
      process("MixtureOfGaussianV2", mixtureOfGaussianV2, img_preProcessor, img_mixtureOfGaussianV2);

#if CV_MAJOR_VERSION == 2
    if (enableMixtureOfGaussianV1)
      process("MixtureOfGaussianV1", mixtureOfGaussianV1, img_preProcessor, img_mixtureOfGaussianV1);
#endif

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    if (enableGMG)
      process("GMG", gmg, img_preProcessor, img_gmg);
#endif

#if CV_MAJOR_VERSION >= 3
    if (enableKNN)
      process("KNN", knn, img_preProcessor, img_knn);
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3
    if (enableDPAdaptiveMedian)
      process("DPAdaptiveMedian", dpAdaptiveMedian, img_preProcessor, img_dpAdaptiveMedian);

    if (enableDPGrimsonGMM)
      process("DPGrimsonGMM", dpGrimsonGMM, img_preProcessor, img_dpGrimsonGMM);

    if (enableDPZivkovicAGMM)
      process("DPZivkovicAGMM", dpZivkovicAGMM, img_preProcessor, img_dpZivkovicAGMM);

    if (enableDPMean)
      process("DPMean", dpTemporalMean, img_preProcessor, img_dpTemporalMean);

    if (enableDPWrenGA)
      process("DPWrenGA", dpWrenGA, img_preProcessor, img_dpWrenGA);

    if (enableDPPratiMediod)
      process("DPPratiMediod", dpPratiMediod, img_preProcessor, img_dpPratiMediod);

    if (enableDPEigenbackground)
      process("DPEigenbackground", dpEigenBackground, img_preProcessor, img_dpEigenBackground);

    if (enableDPTexture)
      process("DPTexture", dpTexture, img_preProcessor, img_dpTexture);

    if (enableT2FGMM_UM)
      process("T2FGMM_UM", type2FuzzyGMM_UM, img_preProcessor, img_type2FuzzyGMM_UM);

    if (enableT2FGMM_UV)
      process("T2FGMM_UV", type2FuzzyGMM_UV, img_preProcessor, img_type2FuzzyGMM_UV);

    if (enableT2FMRF_UM)
      process("T2FMRF_UM", type2FuzzyMRF_UM, img_preProcessor, img_type2FuzzyMRF_UM);

    if (enableT2FMRF_UV)
      process("T2FMRF_UV", type2FuzzyMRF_UV, img_preProcessor, img_type2FuzzyMRF_UV);

    if (enableFuzzySugenoIntegral)
      process("FuzzySugenoIntegral", fuzzySugenoIntegral, img_preProcessor, img_fuzzySugenoIntegral);

    if (enableFuzzyChoquetIntegral)
      process("FuzzyChoquetIntegral", fuzzyChoquetIntegral, img_preProcessor, img_fuzzyChoquetIntegral);

    if (enableLBSimpleGaussian)
      process("LBSimpleGaussian", lbSimpleGaussian, img_preProcessor, img_lbSimpleGaussian);

    if (enableLBFuzzyGaussian)
      process("LBFuzzyGaussian", lbFuzzyGaussian, img_preProcessor, img_lbFuzzyGaussian);

    if (enableLBMixtureOfGaussians)
      process("LBMixtureOfGaussians", lbMixtureOfGaussians, img_preProcessor, img_lbMixtureOfGaussians);

    if (enableLBAdaptiveSOM)
      process("LBAdaptiveSOM", lbAdaptiveSOM, img_preProcessor, img_lbAdaptiveSOM);

    if (enableLBFuzzyAdaptiveSOM)
      process("LBFuzzyAdaptiveSOM", lbFuzzyAdaptiveSOM, img_preProcessor, img_lbFuzzyAdaptiveSOM);

    if (enablePBAS)
      process("PBAS", pixelBasedAdaptiveSegmenter, img_preProcessor, img_pixelBasedAdaptiveSegmenter);

    if (enableVuMeter)
      process("VuMeter", vuMeter, img_preProcessor, img_vumeter);

    if (enableKDE)
      process("KDE", kde, img_preProcessor, img_kde);

    if (enableIMBS)
      process("IMBS", imbs, img_preProcessor, img_imbs);

    if (enableMultiCue)
      process("MultiCue", multiCue, img_preProcessor, img_multiCue);
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7
    if (enableLbpMrf)
      process("LbpMrf", lbpMrf, img_preProcessor, img_lbpMrf);

    if (enableMultiLayer)
    {
      multiLayer->setStatus(MultiLayer::MLBGS_LEARN);
      //multiLayer->setStatus(MultiLayer::MLBGS_DETECT);
      process("MultiLayer", multiLayer, img_preProcessor, img_multiLayer);
    }
#endif

    if (enableSigmaDelta)
      process("SigmaDelta", sigmaDelta, img_preProcessor, img_sigmaDelta);

    if (enableSuBSENSE)
      process("SuBSENSE", subSENSE, img_preProcessor, img_subSENSE);

    if (enableLOBSTER)
      process("LOBSTER", lobster, img_preProcessor, img_lobster);

    if (enablePAWCS)
      process("PAWCS", pawcs, img_preProcessor, img_pawcs);

    if (enableTwoPoints)
      process("TwoPoints", twoPoints, img_preProcessor, img_twoPoints);

    if (enableViBe)
      process("ViBe", vibe, img_preProcessor, img_vibe);

    if (enableCodeBook)
      process("CodeBook", codeBook, img_preProcessor, img_codeBook);

    if (enableForegroundMaskAnalysis)
    {
      foregroundMaskAnalysis->stopAt = frameToStop;
      foregroundMaskAnalysis->img_ref_path = imgref;

      foregroundMaskAnalysis->process(frameNumber, "FrameDifference", img_frameDifference);
      foregroundMaskAnalysis->process(frameNumber, "StaticFrameDifference", img_staticFrameDifference);
      foregroundMaskAnalysis->process(frameNumber, "WeightedMovingMean", img_weightedMovingMean);
      foregroundMaskAnalysis->process(frameNumber, "WeightedMovingVariance", img_weightedMovingVariance);
      foregroundMaskAnalysis->process(frameNumber, "AdaptiveBackgroundLearning", img_adaptiveBackgroundLearning);
      foregroundMaskAnalysis->process(frameNumber, "AdaptiveSelectiveBackgroundLearning", img_adaptiveSelectiveBackgroundLearning);
      foregroundMaskAnalysis->process(frameNumber, "MixtureOfGaussianV2", img_mixtureOfGaussianV2);

#if CV_MAJOR_VERSION == 2
      foregroundMaskAnalysis->process(frameNumber, "MixtureOfGaussianV1", img_mixtureOfGaussianV1);
#endif

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
      foregroundMaskAnalysis->process(frameNumber, "GMG", img_gmg);
#endif
      
#if CV_MAJOR_VERSION >= 3
      foregroundMaskAnalysis->process(frameNumber, "KNN", img_knn);
#endif
      
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3
      foregroundMaskAnalysis->process(frameNumber, "DPAdaptiveMedian", img_dpAdaptiveMedian);
      foregroundMaskAnalysis->process(frameNumber, "DPGrimsonGMM", img_dpGrimsonGMM);
      foregroundMaskAnalysis->process(frameNumber, "DPZivkovicAGMM", img_dpZivkovicAGMM);
      foregroundMaskAnalysis->process(frameNumber, "DPMean", img_dpTemporalMean);
      foregroundMaskAnalysis->process(frameNumber, "DPWrenGA", img_dpWrenGA);
      foregroundMaskAnalysis->process(frameNumber, "DPPratiMediod", img_dpPratiMediod);
      foregroundMaskAnalysis->process(frameNumber, "DPEigenbackground", img_dpEigenBackground);
      foregroundMaskAnalysis->process(frameNumber, "DPTexture", img_dpTexture);
      foregroundMaskAnalysis->process(frameNumber, "T2FGMM_UM", img_type2FuzzyGMM_UM);
      foregroundMaskAnalysis->process(frameNumber, "T2FGMM_UV", img_type2FuzzyGMM_UV);
      foregroundMaskAnalysis->process(frameNumber, "T2FMRF_UM", img_type2FuzzyMRF_UM);
      foregroundMaskAnalysis->process(frameNumber, "T2FMRF_UV", img_type2FuzzyMRF_UV);
      foregroundMaskAnalysis->process(frameNumber, "FuzzySugenoIntegral", img_fuzzySugenoIntegral);
      foregroundMaskAnalysis->process(frameNumber, "FuzzyChoquetIntegral", img_fuzzyChoquetIntegral);
      foregroundMaskAnalysis->process(frameNumber, "LBSimpleGaussian", img_lbSimpleGaussian);
      foregroundMaskAnalysis->process(frameNumber, "LBFuzzyGaussian", img_lbFuzzyGaussian);
      foregroundMaskAnalysis->process(frameNumber, "LBMixtureOfGaussians", img_lbMixtureOfGaussians);
      foregroundMaskAnalysis->process(frameNumber, "LBAdaptiveSOM", img_lbAdaptiveSOM);
      foregroundMaskAnalysis->process(frameNumber, "LBFuzzyAdaptiveSOM", img_lbFuzzyAdaptiveSOM);
      foregroundMaskAnalysis->process(frameNumber, "PBAS", img_pixelBasedAdaptiveSegmenter);
      foregroundMaskAnalysis->process(frameNumber, "VuMeter", img_vumeter);
      foregroundMaskAnalysis->process(frameNumber, "KDE", img_kde);
      foregroundMaskAnalysis->process(frameNumber, "IMBS", img_imbs);
      foregroundMaskAnalysis->process(frameNumber, "MultiCue", img_multiCue);
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7
      foregroundMaskAnalysis->process(frameNumber, "LbpMrf", img_lbpMrf);
      foregroundMaskAnalysis->process(frameNumber, "MultiLayer", img_multiLayer);
#endif

      foregroundMaskAnalysis->process(frameNumber, "SigmaDelta", img_sigmaDelta);
      foregroundMaskAnalysis->process(frameNumber, "SuBSENSE", img_subSENSE);
      foregroundMaskAnalysis->process(frameNumber, "LOBSTER", img_lobster);
      foregroundMaskAnalysis->process(frameNumber, "PAWCS", img_pawcs);
      foregroundMaskAnalysis->process(frameNumber, "TwoPoints", img_twoPoints);
      foregroundMaskAnalysis->process(frameNumber, "ViBe", img_vibe);
      foregroundMaskAnalysis->process(frameNumber, "CodeBook", img_codeBook);
    }

    firstTime = false;
  }

  void FrameProcessor::finish(void){}

  void FrameProcessor::tic(std::string value)
  {
    processname = value;
    duration = static_cast<double>(cv::getTickCount());
  }

  void FrameProcessor::toc()
  {
    duration = (static_cast<double>(cv::getTickCount()) - duration) / cv::getTickFrequency();
    std::cout << processname << "\ttime(sec):" << std::fixed << std::setprecision(6) << duration << std::endl;
  }

  void FrameProcessor::save_config(cv::FileStorage &fs) {
    fs << "tictoc" << tictoc;
    fs << "enablePreProcessor" << enablePreProcessor;
    fs << "enableForegroundMaskAnalysis" << enableForegroundMaskAnalysis;
    fs << "enableFrameDifference" << enableFrameDifference;
    fs << "enableStaticFrameDifference" << enableStaticFrameDifference;
    fs << "enableWeightedMovingMean" << enableWeightedMovingMean;
    fs << "enableWeightedMovingVariance" << enableWeightedMovingVariance;
    fs << "enableAdaptiveBackgroundLearning" << enableAdaptiveBackgroundLearning;
    fs << "enableAdaptiveSelectiveBackgroundLearning" << enableAdaptiveSelectiveBackgroundLearning;
    fs << "enableMixtureOfGaussianV2" << enableMixtureOfGaussianV2;

#if CV_MAJOR_VERSION == 2
    fs << "enableMixtureOfGaussianV1" << enableMixtureOfGaussianV1;
#endif
    
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    fs << "enableGMG" << enableGMG;
#endif
    
#if CV_MAJOR_VERSION >= 3
    fs << "enableKNN" << enableKNN;
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3
    fs << "enableDPAdaptiveMedian" << enableDPAdaptiveMedian;
    fs << "enableDPGrimsonGMM" << enableDPGrimsonGMM;
    fs << "enableDPZivkovicAGMM" << enableDPZivkovicAGMM;
    fs << "enableDPMean" << enableDPMean;
    fs << "enableDPWrenGA" << enableDPWrenGA;
    fs << "enableDPPratiMediod" << enableDPPratiMediod;
    fs << "enableDPEigenbackground" << enableDPEigenbackground;
    fs << "enableDPTexture" << enableDPTexture;
    fs << "enableT2FGMM_UM" << enableT2FGMM_UM;
    fs << "enableT2FGMM_UV" << enableT2FGMM_UV;
    fs << "enableT2FMRF_UM" << enableT2FMRF_UM;
    fs << "enableT2FMRF_UV" << enableT2FMRF_UV;
    fs << "enableFuzzySugenoIntegral" << enableFuzzySugenoIntegral;
    fs << "enableFuzzyChoquetIntegral" << enableFuzzyChoquetIntegral;
    fs << "enableLBSimpleGaussian" << enableLBSimpleGaussian;
    fs << "enableLBFuzzyGaussian" << enableLBFuzzyGaussian;
    fs << "enableLBMixtureOfGaussians" << enableLBMixtureOfGaussians;
    fs << "enableLBAdaptiveSOM" << enableLBAdaptiveSOM;
    fs << "enableLBFuzzyAdaptiveSOM" << enableLBFuzzyAdaptiveSOM;
    fs << "enablePBAS" << enablePBAS;
    fs << "enableVuMeter" << enableVuMeter;
    fs << "enableKDE" << enableKDE;
    fs << "enableIMBS" << enableIMBS;
    fs << "enableMultiCue" << enableMultiCue;
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7
    fs << "enableLbpMrf" << enableLbpMrf;
    fs << "enableMultiLayer" << enableMultiLayer;
#endif

    fs << "enableSigmaDelta" << enableSigmaDelta;
    fs << "enableSuBSENSE" << enableSuBSENSE;
    fs << "enableLOBSTER" << enableLOBSTER;
    fs << "enablePAWCS" << enablePAWCS;
    fs << "enableTwoPoints" << enableTwoPoints;
    fs << "enableViBe" << enableViBe;
    fs << "enableCodeBook" << enableCodeBook;
  }

  void FrameProcessor::load_config(cv::FileStorage &fs) {
    fs["tictoc"] >> tictoc;
    fs["enablePreProcessor"] >> enablePreProcessor;
    fs["enableForegroundMaskAnalysis"] >> enableForegroundMaskAnalysis;
    fs["enableFrameDifference"] >> enableFrameDifference;
    fs["enableStaticFrameDifference"] >> enableStaticFrameDifference;
    fs["enableWeightedMovingMean"] >> enableWeightedMovingMean;
    fs["enableWeightedMovingVariance"] >> enableWeightedMovingVariance;
    fs["enableAdaptiveBackgroundLearning"] >> enableAdaptiveBackgroundLearning;
    fs["enableAdaptiveSelectiveBackgroundLearning"] >> enableAdaptiveSelectiveBackgroundLearning;
    fs["enableMixtureOfGaussianV2"] >> enableMixtureOfGaussianV2;

#if CV_MAJOR_VERSION == 2
    fs["enableMixtureOfGaussianV1"] >> enableMixtureOfGaussianV1;
#endif
    
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    fs["enableGMG"] >> enableGMG;
#endif
    
#if CV_MAJOR_VERSION >= 3
    fs["enableKNN"] >> enableKNN;
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3
    fs["enableDPAdaptiveMedian"] >> enableDPAdaptiveMedian;
    fs["enableDPGrimsonGMM"] >> enableDPGrimsonGMM;
    fs["enableDPZivkovicAGMM"] >> enableDPZivkovicAGMM;
    fs["enableDPMean"] >> enableDPMean;
    fs["enableDPWrenGA"] >> enableDPWrenGA;
    fs["enableDPPratiMediod"] >> enableDPPratiMediod;
    fs["enableDPEigenbackground"] >> enableDPEigenbackground;
    fs["enableDPTexture"] >> enableDPTexture;
    fs["enableT2FGMM_UM"] >> enableT2FGMM_UM;
    fs["enableT2FGMM_UV"] >> enableT2FGMM_UV;
    fs["enableT2FMRF_UM"] >> enableT2FMRF_UM;
    fs["enableT2FMRF_UV"] >> enableT2FMRF_UV;
    fs["enableFuzzySugenoIntegral"] >> enableFuzzySugenoIntegral;
    fs["enableFuzzyChoquetIntegral"] >> enableFuzzyChoquetIntegral;
    fs["enableLBSimpleGaussian"] >> enableLBSimpleGaussian;
    fs["enableLBFuzzyGaussian"] >> enableLBFuzzyGaussian;
    fs["enableLBMixtureOfGaussians"] >> enableLBMixtureOfGaussians;
    fs["enableLBAdaptiveSOM"] >> enableLBAdaptiveSOM;
    fs["enableLBFuzzyAdaptiveSOM"] >> enableLBFuzzyAdaptiveSOM;
    fs["enablePBAS"] >> enablePBAS;
    fs["enableVuMeter"] >> enableVuMeter;
    fs["enableKDE"] >> enableKDE;
    fs["enableIMBS"] >> enableIMBS;
    fs["enableMultiCue"] >> enableMultiCue;
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7
    fs["enableLbpMrf"] >> enableLbpMrf;
    fs["enableMultiLayer"] >> enableMultiLayer;
#endif

    fs["enableSigmaDelta"] >> enableSigmaDelta;
    fs["enableSuBSENSE"] >> enableSuBSENSE;
    fs["enableLOBSTER"] >> enableLOBSTER;
    fs["enablePAWCS"] >> enablePAWCS;
    fs["enableTwoPoints"] >> enableTwoPoints;
    fs["enableViBe"] >> enableViBe;
    fs["enableCodeBook"] >> enableCodeBook;
  }
}
