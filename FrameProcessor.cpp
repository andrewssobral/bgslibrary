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
#include "FrameProcessor.h"
#include <iomanip>

namespace bgslibrary
{
  FrameProcessor::FrameProcessor() : firstTime(true), frameNumber(0), duration(0), tictoc(""), frameToStop(0)
  {
    std::cout << "FrameProcessor()" << std::endl;

    loadConfig();
    saveConfig();
  }

  FrameProcessor::~FrameProcessor()
  {
    std::cout << "~FrameProcessor()" << std::endl;
  }

  void FrameProcessor::init()
  {
    if (enablePreProcessor)
      preProcessor = new PreProcessor;

    if (enableFrameDifference)
      frameDifference = new FrameDifference;

    if (enableStaticFrameDifference)
      staticFrameDifference = new StaticFrameDifference;

    if (enableWeightedMovingMean)
      weightedMovingMean = new WeightedMovingMean;

    if (enableWeightedMovingVariance)
      weightedMovingVariance = new WeightedMovingVariance;

#if CV_MAJOR_VERSION == 2
    if (enableMixtureOfGaussianV1)
      mixtureOfGaussianV1 = new MixtureOfGaussianV1;
#endif

    if (enableMixtureOfGaussianV2)
      mixtureOfGaussianV2 = new MixtureOfGaussianV2;

    if (enableAdaptiveBackgroundLearning)
      adaptiveBackgroundLearning = new AdaptiveBackgroundLearning;

#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    if (enableGMG)
      gmg = new GMG;
#endif

#if CV_MAJOR_VERSION == 3
    if (enableKNN)
      knn = new KNN;
#endif

    if (enableDPAdaptiveMedian)
      dpAdaptiveMedian = new DPAdaptiveMedian;

    if (enableDPGrimsonGMM)
      dpGrimsonGMM = new DPGrimsonGMM;

    if (enableDPZivkovicAGMM)
      dpZivkovicAGMM = new DPZivkovicAGMM;

    if (enableDPMean)
      dpTemporalMean = new DPMean;

    if (enableDPWrenGA)
      dpWrenGA = new DPWrenGA;

    if (enableDPPratiMediod)
      dpPratiMediod = new DPPratiMediod;

    if (enableDPEigenbackground)
      dpEigenBackground = new DPEigenbackground;

    if (enableDPTexture)
      dpTexture = new DPTexture;

    if (enableT2FGMM_UM)
      type2FuzzyGMM_UM = new T2FGMM_UM;

    if (enableT2FGMM_UV)
      type2FuzzyGMM_UV = new T2FGMM_UV;

    if (enableT2FMRF_UM)
      type2FuzzyMRF_UM = new T2FMRF_UM;

    if (enableT2FMRF_UV)
      type2FuzzyMRF_UV = new T2FMRF_UV;

    if (enableFuzzySugenoIntegral)
      fuzzySugenoIntegral = new FuzzySugenoIntegral;

    if (enableFuzzyChoquetIntegral)
      fuzzyChoquetIntegral = new FuzzyChoquetIntegral;

    if (enableLBSimpleGaussian)
      lbSimpleGaussian = new LBSimpleGaussian;

    if (enableLBFuzzyGaussian)
      lbFuzzyGaussian = new LBFuzzyGaussian;

    if (enableLBMixtureOfGaussians)
      lbMixtureOfGaussians = new LBMixtureOfGaussians;

    if (enableLBAdaptiveSOM)
      lbAdaptiveSOM = new LBAdaptiveSOM;

    if (enableLBFuzzyAdaptiveSOM)
      lbFuzzyAdaptiveSOM = new LBFuzzyAdaptiveSOM;

    if (enableLbpMrf)
      lbpMrf = new LBP_MRF;

    if (enableMultiLayer)
      multiLayer = new MultiLayer;

    if (enablePBAS)
      pixelBasedAdaptiveSegmenter = new PixelBasedAdaptiveSegmenter;

    if (enableVuMeter)
      vuMeter = new VuMeter;

    if (enableKDE)
      kde = new KDE;

    if (enableIMBS)
      imbs = new IndependentMultimodal;

    if (enableMultiCue)
      multiCue = new MultiCue;

    if (enableSigmaDelta)
      sigmaDelta = new SigmaDelta;

    if (enableSuBSENSE)
      subSENSE = new SuBSENSE;

    if (enableLOBSTER)
      lobster = new LOBSTER;

    if (enablePAWCS)
      pawcs = new PAWCS;

    if (enableTwoPoints)
      twoPoints = new TwoPoints;

    if (enableViBe)
      vibe = new ViBe;

    if (enableCodeBook)
      codeBook = new CodeBook;

    if (enableForegroundMaskAnalysis)
      foregroundMaskAnalysis = new ForegroundMaskAnalysis;
  }

  void FrameProcessor::process(std::string name, IBGS *bgs, const cv::Mat &img_input, cv::Mat &img_bgs)
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

    if (enableFrameDifference)
      process("FrameDifference", frameDifference, img_preProcessor, img_frameDifference);

    if (enableStaticFrameDifference)
      process("StaticFrameDifference", staticFrameDifference, img_preProcessor, img_staticFrameDifference);

    if (enableWeightedMovingMean)
      process("WeightedMovingMean", weightedMovingMean, img_preProcessor, img_weightedMovingMean);

    if (enableWeightedMovingVariance)
      process("WeightedMovingVariance", weightedMovingVariance, img_preProcessor, img_weightedMovingVariance);

#if CV_MAJOR_VERSION == 2
    if (enableMixtureOfGaussianV1)
      process("MixtureOfGaussianV1", mixtureOfGaussianV1, img_preProcessor, img_mixtureOfGaussianV1);
#endif

    if (enableMixtureOfGaussianV2)
      process("MixtureOfGaussianV2", mixtureOfGaussianV2, img_preProcessor, img_mixtureOfGaussianV2);

    if (enableAdaptiveBackgroundLearning)
      process("AdaptiveBackgroundLearning", adaptiveBackgroundLearning, img_preProcessor, img_adaptiveBackgroundLearning);

#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    if (enableGMG)
      process("GMG", gmg, img_preProcessor, img_gmg);
#endif

#if CV_MAJOR_VERSION == 3
    if (enableKNN)
      process("KNN", knn, img_preProcessor, img_knn);
#endif

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

    if (enableLbpMrf)
      process("LbpMrf", lbpMrf, img_preProcessor, img_lbpMrf);

    if (enableMultiLayer)
    {
      multiLayer->setStatus(MultiLayer::MLBGS_LEARN);
      //multiLayer->setStatus(MultiLayer::MLBGS_DETECT);
      process("MultiLayer", multiLayer, img_preProcessor, img_multiLayer);
    }

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
#if CV_MAJOR_VERSION == 2
      foregroundMaskAnalysis->process(frameNumber, "MixtureOfGaussianV1", img_mixtureOfGaussianV1);
#endif
      foregroundMaskAnalysis->process(frameNumber, "MixtureOfGaussianV2", img_mixtureOfGaussianV2);
      foregroundMaskAnalysis->process(frameNumber, "AdaptiveBackgroundLearning", img_adaptiveBackgroundLearning);
#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
      foregroundMaskAnalysis->process(frameNumber, "GMG", img_gmg);
#endif
#if CV_MAJOR_VERSION == 3
      foregroundMaskAnalysis->process(frameNumber, "KNN", img_knn);
#endif
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
      foregroundMaskAnalysis->process(frameNumber, "LbpMrf", img_lbpMrf);
      foregroundMaskAnalysis->process(frameNumber, "MultiLayer", img_multiLayer);
      foregroundMaskAnalysis->process(frameNumber, "PBAS", img_pixelBasedAdaptiveSegmenter);
      foregroundMaskAnalysis->process(frameNumber, "VuMeter", img_vumeter);
      foregroundMaskAnalysis->process(frameNumber, "KDE", img_kde);
      foregroundMaskAnalysis->process(frameNumber, "IMBS", img_imbs);
      foregroundMaskAnalysis->process(frameNumber, "MultiCue", img_multiCue);
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

  void FrameProcessor::finish(void)
  {
    /*if(enableMultiLayer)
    multiLayer->finish();

    if(enableLBSimpleGaussian)
    lbSimpleGaussian->finish();

    if(enableLBFuzzyGaussian)
    lbFuzzyGaussian->finish();

    if(enableLBMixtureOfGaussians)
    lbMixtureOfGaussians->finish();

    if(enableLBAdaptiveSOM)
    lbAdaptiveSOM->finish();

    if(enableLBFuzzyAdaptiveSOM)
    lbFuzzyAdaptiveSOM->finish();*/

    if (enableForegroundMaskAnalysis)
      delete foregroundMaskAnalysis;

    if (enableCodeBook)
      delete codeBook;

    if (enableViBe)
      delete vibe;

    if (enableTwoPoints)
      delete twoPoints;

    if (enablePAWCS)
      delete pawcs;

    if (enableLOBSTER)
      delete lobster;

    if (enableSuBSENSE)
      delete subSENSE;

    if (enableSigmaDelta)
      delete sigmaDelta;

    if (enableMultiCue)
      delete multiCue;

    if (enableIMBS)
      delete imbs;

    if (enableKDE)
      delete kde;

    if (enableVuMeter)
      delete vuMeter;

    if (enablePBAS)
      delete pixelBasedAdaptiveSegmenter;

    if (enableMultiLayer)
      delete multiLayer;

    if (enableLBFuzzyAdaptiveSOM)
      delete lbFuzzyAdaptiveSOM;

    if (enableLBAdaptiveSOM)
      delete lbAdaptiveSOM;

    if (enableLBMixtureOfGaussians)
      delete lbMixtureOfGaussians;

    if (enableLBFuzzyGaussian)
      delete lbFuzzyGaussian;

    if (enableLBSimpleGaussian)
      delete lbSimpleGaussian;

    if (enableLbpMrf)
      delete lbpMrf;

    if (enableFuzzyChoquetIntegral)
      delete fuzzyChoquetIntegral;

    if (enableFuzzySugenoIntegral)
      delete fuzzySugenoIntegral;

    if (enableT2FMRF_UV)
      delete type2FuzzyMRF_UV;

    if (enableT2FMRF_UM)
      delete type2FuzzyMRF_UM;

    if (enableT2FGMM_UV)
      delete type2FuzzyGMM_UV;

    if (enableT2FGMM_UM)
      delete type2FuzzyGMM_UM;

    if (enableDPTexture)
      delete dpTexture;

    if (enableDPEigenbackground)
      delete dpEigenBackground;

    if (enableDPPratiMediod)
      delete dpPratiMediod;

    if (enableDPWrenGA)
      delete dpWrenGA;

    if (enableDPMean)
      delete dpTemporalMean;

    if (enableDPZivkovicAGMM)
      delete dpZivkovicAGMM;

    if (enableDPGrimsonGMM)
      delete dpGrimsonGMM;

    if (enableDPAdaptiveMedian)
      delete dpAdaptiveMedian;

#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    if (enableGMG)
      delete gmg;
#endif

#if CV_MAJOR_VERSION == 3
    if (enableKNN)
      delete knn;
#endif

    if (enableAdaptiveBackgroundLearning)
      delete adaptiveBackgroundLearning;

    if (enableMixtureOfGaussianV2)
      delete mixtureOfGaussianV2;

#if CV_MAJOR_VERSION == 2
    if (enableMixtureOfGaussianV1)
      delete mixtureOfGaussianV1;
#endif

    if (enableWeightedMovingVariance)
      delete weightedMovingVariance;

    if (enableWeightedMovingMean)
      delete weightedMovingMean;

    if (enableStaticFrameDifference)
      delete staticFrameDifference;

    if (enableFrameDifference)
      delete frameDifference;

    if (enablePreProcessor)
      delete preProcessor;
  }

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

  void FrameProcessor::saveConfig()
  {
    CvFileStorage* fs = cvOpenFileStorage("./config/FrameProcessor.xml", 0, CV_STORAGE_WRITE);

    cvWriteString(fs, "tictoc", tictoc.c_str());

    cvWriteInt(fs, "enablePreProcessor", enablePreProcessor);

    cvWriteInt(fs, "enableForegroundMaskAnalysis", enableForegroundMaskAnalysis);

    cvWriteInt(fs, "enableFrameDifference", enableFrameDifference);
    cvWriteInt(fs, "enableStaticFrameDifference", enableStaticFrameDifference);
    cvWriteInt(fs, "enableWeightedMovingMean", enableWeightedMovingMean);
    cvWriteInt(fs, "enableWeightedMovingVariance", enableWeightedMovingVariance);
#if CV_MAJOR_VERSION == 2
    cvWriteInt(fs, "enableMixtureOfGaussianV1", enableMixtureOfGaussianV1);
#endif
    cvWriteInt(fs, "enableMixtureOfGaussianV2", enableMixtureOfGaussianV2);
    cvWriteInt(fs, "enableAdaptiveBackgroundLearning", enableAdaptiveBackgroundLearning);
#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    cvWriteInt(fs, "enableGMG", enableGMG);
#endif
#if CV_MAJOR_VERSION == 3
    cvWriteInt(fs, "enableKNN", enableKNN);
#endif

    cvWriteInt(fs, "enableDPAdaptiveMedian", enableDPAdaptiveMedian);
    cvWriteInt(fs, "enableDPGrimsonGMM", enableDPGrimsonGMM);
    cvWriteInt(fs, "enableDPZivkovicAGMM", enableDPZivkovicAGMM);
    cvWriteInt(fs, "enableDPMean", enableDPMean);
    cvWriteInt(fs, "enableDPWrenGA", enableDPWrenGA);
    cvWriteInt(fs, "enableDPPratiMediod", enableDPPratiMediod);
    cvWriteInt(fs, "enableDPEigenbackground", enableDPEigenbackground);
    cvWriteInt(fs, "enableDPTexture", enableDPTexture);

    cvWriteInt(fs, "enableT2FGMM_UM", enableT2FGMM_UM);
    cvWriteInt(fs, "enableT2FGMM_UV", enableT2FGMM_UV);
    cvWriteInt(fs, "enableT2FMRF_UM", enableT2FMRF_UM);
    cvWriteInt(fs, "enableT2FMRF_UV", enableT2FMRF_UV);
    cvWriteInt(fs, "enableFuzzySugenoIntegral", enableFuzzySugenoIntegral);
    cvWriteInt(fs, "enableFuzzyChoquetIntegral", enableFuzzyChoquetIntegral);

    cvWriteInt(fs, "enableLBSimpleGaussian", enableLBSimpleGaussian);
    cvWriteInt(fs, "enableLBFuzzyGaussian", enableLBFuzzyGaussian);
    cvWriteInt(fs, "enableLBMixtureOfGaussians", enableLBMixtureOfGaussians);
    cvWriteInt(fs, "enableLBAdaptiveSOM", enableLBAdaptiveSOM);
    cvWriteInt(fs, "enableLBFuzzyAdaptiveSOM", enableLBFuzzyAdaptiveSOM);

    cvWriteInt(fs, "enableLbpMrf", enableLbpMrf);
    cvWriteInt(fs, "enableMultiLayer", enableMultiLayer);
    cvWriteInt(fs, "enablePBAS", enablePBAS);
    cvWriteInt(fs, "enableVuMeter", enableVuMeter);
    cvWriteInt(fs, "enableKDE", enableKDE);
    cvWriteInt(fs, "enableIMBS", enableIMBS);
    cvWriteInt(fs, "enableMultiCue", enableMultiCue);
    cvWriteInt(fs, "enableSigmaDelta", enableSigmaDelta);
    cvWriteInt(fs, "enableSuBSENSE", enableSuBSENSE);
    cvWriteInt(fs, "enableLOBSTER", enableLOBSTER);
    cvWriteInt(fs, "enablePAWCS", enablePAWCS);
    cvWriteInt(fs, "enableTwoPoints", enableTwoPoints);
    cvWriteInt(fs, "enableViBe", enableViBe);
    cvWriteInt(fs, "enableCodeBook", enableCodeBook);

    cvReleaseFileStorage(&fs);
  }

  void FrameProcessor::loadConfig()
  {
    CvFileStorage* fs = cvOpenFileStorage("./config/FrameProcessor.xml", 0, CV_STORAGE_READ);

    tictoc = cvReadStringByName(fs, 0, "tictoc", "");

    enablePreProcessor = cvReadIntByName(fs, 0, "enablePreProcessor", true);

    enableForegroundMaskAnalysis = cvReadIntByName(fs, 0, "enableForegroundMaskAnalysis", false);

    enableFrameDifference = cvReadIntByName(fs, 0, "enableFrameDifference", false);
    enableStaticFrameDifference = cvReadIntByName(fs, 0, "enableStaticFrameDifference", false);
    enableWeightedMovingMean = cvReadIntByName(fs, 0, "enableWeightedMovingMean", false);
    enableWeightedMovingVariance = cvReadIntByName(fs, 0, "enableWeightedMovingVariance", false);
#if CV_MAJOR_VERSION == 2
    enableMixtureOfGaussianV1 = cvReadIntByName(fs, 0, "enableMixtureOfGaussianV1", false);
#endif
    enableMixtureOfGaussianV2 = cvReadIntByName(fs, 0, "enableMixtureOfGaussianV2", false);
    enableAdaptiveBackgroundLearning = cvReadIntByName(fs, 0, "enableAdaptiveBackgroundLearning", false);
#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    enableGMG = cvReadIntByName(fs, 0, "enableGMG", false);
#endif
#if CV_MAJOR_VERSION == 3
    enableKNN = cvReadIntByName(fs, 0, "enableKNN", false);
#endif

    enableDPAdaptiveMedian = cvReadIntByName(fs, 0, "enableDPAdaptiveMedian", false);
    enableDPGrimsonGMM = cvReadIntByName(fs, 0, "enableDPGrimsonGMM", false);
    enableDPZivkovicAGMM = cvReadIntByName(fs, 0, "enableDPZivkovicAGMM", false);
    enableDPMean = cvReadIntByName(fs, 0, "enableDPMean", false);
    enableDPWrenGA = cvReadIntByName(fs, 0, "enableDPWrenGA", false);
    enableDPPratiMediod = cvReadIntByName(fs, 0, "enableDPPratiMediod", false);
    enableDPEigenbackground = cvReadIntByName(fs, 0, "enableDPEigenbackground", false);
    enableDPTexture = cvReadIntByName(fs, 0, "enableDPTexture", false);

    enableT2FGMM_UM = cvReadIntByName(fs, 0, "enableT2FGMM_UM", false);
    enableT2FGMM_UV = cvReadIntByName(fs, 0, "enableT2FGMM_UV", false);
    enableT2FMRF_UM = cvReadIntByName(fs, 0, "enableT2FMRF_UM", false);
    enableT2FMRF_UV = cvReadIntByName(fs, 0, "enableT2FMRF_UV", false);
    enableFuzzySugenoIntegral = cvReadIntByName(fs, 0, "enableFuzzySugenoIntegral", false);
    enableFuzzyChoquetIntegral = cvReadIntByName(fs, 0, "enableFuzzyChoquetIntegral", false);

    enableLBSimpleGaussian = cvReadIntByName(fs, 0, "enableLBSimpleGaussian", false);
    enableLBFuzzyGaussian = cvReadIntByName(fs, 0, "enableLBFuzzyGaussian", false);
    enableLBMixtureOfGaussians = cvReadIntByName(fs, 0, "enableLBMixtureOfGaussians", false);
    enableLBAdaptiveSOM = cvReadIntByName(fs, 0, "enableLBAdaptiveSOM", false);
    enableLBFuzzyAdaptiveSOM = cvReadIntByName(fs, 0, "enableLBFuzzyAdaptiveSOM", false);

    enableLbpMrf = cvReadIntByName(fs, 0, "enableLbpMrf", false);
    enableMultiLayer = cvReadIntByName(fs, 0, "enableMultiLayer", false);
    enablePBAS = cvReadIntByName(fs, 0, "enablePBAS", false);
    enableVuMeter = cvReadIntByName(fs, 0, "enableVuMeter", false);
    enableKDE = cvReadIntByName(fs, 0, "enableKDE", false);
    enableIMBS = cvReadIntByName(fs, 0, "enableIMBS", false);
    enableMultiCue = cvReadIntByName(fs, 0, "enableMultiCue", false);
    enableSigmaDelta = cvReadIntByName(fs, 0, "enableSigmaDelta", false);
    enableSuBSENSE = cvReadIntByName(fs, 0, "enableSuBSENSE", false);
    enableLOBSTER = cvReadIntByName(fs, 0, "enableLOBSTER", false);
    enablePAWCS = cvReadIntByName(fs, 0, "enablePAWCS", false);
    enableTwoPoints = cvReadIntByName(fs, 0, "enableTwoPoints", false);
    enableViBe = cvReadIntByName(fs, 0, "enableViBe", false);
    enableCodeBook = cvReadIntByName(fs, 0, "enableCodeBook", false);

    cvReleaseFileStorage(&fs);
  }
}
