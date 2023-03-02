#pragma once
#pragma warning(disable : 4482)

#include "IFrameProcessor.h"
#include "PreProcessor.h"

#include "algorithms/algorithms.h"
#include "tools/ForegroundMaskAnalysis.h"

namespace bgslibrary
{
  class FrameProcessor : public IFrameProcessor, public ILoadSaveConfig
  {
  private:
    bool firstTime;
    long frameNumber;
    std::string processname;
    double duration;
    std::string tictoc;

    cv::Mat img_preProcessor;
    std::unique_ptr<PreProcessor> preProcessor;
    bool enablePreProcessor = false;

    cv::Mat img_frameDifference;
    std::shared_ptr<FrameDifference> frameDifference;
    bool enableFrameDifference = false;

    cv::Mat img_staticFrameDifference;
    std::shared_ptr<StaticFrameDifference> staticFrameDifference;
    bool enableStaticFrameDifference = false;

    cv::Mat img_weightedMovingMean;
    std::shared_ptr<WeightedMovingMean> weightedMovingMean;
    bool enableWeightedMovingMean = false;

    cv::Mat img_weightedMovingVariance;
    std::shared_ptr<WeightedMovingVariance> weightedMovingVariance;
    bool enableWeightedMovingVariance = false;

    cv::Mat img_adaptiveBackgroundLearning;
    std::shared_ptr<AdaptiveBackgroundLearning> adaptiveBackgroundLearning;
    bool enableAdaptiveBackgroundLearning = false;

    cv::Mat img_adaptiveSelectiveBackgroundLearning;
    std::shared_ptr<AdaptiveSelectiveBackgroundLearning> adaptiveSelectiveBackgroundLearning;
    bool enableAdaptiveSelectiveBackgroundLearning = false;

    cv::Mat img_mixtureOfGaussianV2;
    std::shared_ptr<MixtureOfGaussianV2> mixtureOfGaussianV2;
    bool enableMixtureOfGaussianV2 = false;

#if CV_MAJOR_VERSION == 2
    cv::Mat img_mixtureOfGaussianV1;
    std::shared_ptr<MixtureOfGaussianV1> mixtureOfGaussianV1;
    bool enableMixtureOfGaussianV1 = false;
#endif

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    cv::Mat img_gmg;
    std::shared_ptr<GMG> gmg;
    bool enableGMG = false;
#endif

#if CV_MAJOR_VERSION >= 3
    cv::Mat img_knn;
    std::shared_ptr<KNN> knn;
    bool enableKNN = false;
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3
    cv::Mat img_dpAdaptiveMedian;
    std::shared_ptr<DPAdaptiveMedian> dpAdaptiveMedian;
    bool enableDPAdaptiveMedian = false;

    cv::Mat img_dpGrimsonGMM;
    std::shared_ptr<DPGrimsonGMM> dpGrimsonGMM;
    bool enableDPGrimsonGMM = false;

    cv::Mat img_dpZivkovicAGMM;
    std::shared_ptr<DPZivkovicAGMM> dpZivkovicAGMM;
    bool enableDPZivkovicAGMM = false;

    cv::Mat img_dpTemporalMean;
    std::shared_ptr<DPMean> dpTemporalMean;
    bool enableDPMean = false;

    cv::Mat img_dpWrenGA;
    std::shared_ptr<DPWrenGA> dpWrenGA;
    bool enableDPWrenGA = false;

    cv::Mat img_dpPratiMediod;
    std::shared_ptr<DPPratiMediod> dpPratiMediod;
    bool enableDPPratiMediod = false;

    cv::Mat img_dpEigenBackground;
    std::shared_ptr<DPEigenbackground> dpEigenBackground;
    bool enableDPEigenbackground = false;

    cv::Mat img_dpTexture;
    std::shared_ptr<DPTexture> dpTexture;
    bool enableDPTexture = false;

    cv::Mat img_type2FuzzyGMM_UM;
    std::shared_ptr<T2FGMM_UM> type2FuzzyGMM_UM;
    bool enableT2FGMM_UM = false;

    cv::Mat img_type2FuzzyGMM_UV;
    std::shared_ptr<T2FGMM_UV> type2FuzzyGMM_UV;
    bool enableT2FGMM_UV = false;

    cv::Mat img_type2FuzzyMRF_UM;
    std::shared_ptr<T2FMRF_UM> type2FuzzyMRF_UM;
    bool enableT2FMRF_UM = false;

    cv::Mat img_type2FuzzyMRF_UV;
    std::shared_ptr<T2FMRF_UV> type2FuzzyMRF_UV;
    bool enableT2FMRF_UV = false;

    cv::Mat img_fuzzySugenoIntegral;
    std::shared_ptr<FuzzySugenoIntegral> fuzzySugenoIntegral;
    bool enableFuzzySugenoIntegral = false;

    cv::Mat img_fuzzyChoquetIntegral;
    std::shared_ptr<FuzzyChoquetIntegral> fuzzyChoquetIntegral;
    bool enableFuzzyChoquetIntegral = false;

    cv::Mat img_lbSimpleGaussian;
    std::shared_ptr<LBSimpleGaussian> lbSimpleGaussian;
    bool enableLBSimpleGaussian = false;

    cv::Mat img_lbFuzzyGaussian;
    std::shared_ptr<LBFuzzyGaussian> lbFuzzyGaussian;
    bool enableLBFuzzyGaussian = false;

    cv::Mat img_lbMixtureOfGaussians;
    std::shared_ptr<LBMixtureOfGaussians> lbMixtureOfGaussians;
    bool enableLBMixtureOfGaussians = false;

    cv::Mat img_lbAdaptiveSOM;
    std::shared_ptr<LBAdaptiveSOM> lbAdaptiveSOM;
    bool enableLBAdaptiveSOM = false;

    cv::Mat img_lbFuzzyAdaptiveSOM;
    std::shared_ptr<LBFuzzyAdaptiveSOM> lbFuzzyAdaptiveSOM;
    bool enableLBFuzzyAdaptiveSOM = false;

    cv::Mat img_pixelBasedAdaptiveSegmenter;
    std::shared_ptr<PixelBasedAdaptiveSegmenter> pixelBasedAdaptiveSegmenter;
    bool enablePBAS = false;

    cv::Mat img_vumeter;
    std::shared_ptr<VuMeter> vuMeter;
    bool enableVuMeter = false;

    cv::Mat img_kde;
    std::shared_ptr<KDE> kde;
    bool enableKDE = false;

    cv::Mat img_imbs;
    std::shared_ptr<IndependentMultimodal> imbs;
    bool enableIMBS = false;

    cv::Mat img_multiCue;
    std::shared_ptr<MultiCue> multiCue;
    bool enableMultiCue = false;
#endif

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7
    cv::Mat img_lbpMrf;
    std::shared_ptr<LBP_MRF> lbpMrf;
    bool enableLbpMrf = false;

    cv::Mat img_multiLayer;
    std::shared_ptr<MultiLayer> multiLayer;
    bool enableMultiLayer = false;
#endif

    cv::Mat img_sigmaDelta;
    std::shared_ptr<SigmaDelta> sigmaDelta;
    bool enableSigmaDelta = false;

    cv::Mat img_subSENSE;
    std::shared_ptr<SuBSENSE> subSENSE;
    bool enableSuBSENSE = false;

    cv::Mat img_lobster;
    std::shared_ptr<LOBSTER> lobster;
    bool enableLOBSTER = false;

    cv::Mat img_pawcs;
    std::shared_ptr<PAWCS> pawcs;
    bool enablePAWCS = false;

    cv::Mat img_twoPoints;
    std::shared_ptr<TwoPoints> twoPoints;
    bool enableTwoPoints = false;

    cv::Mat img_vibe;
    std::shared_ptr<ViBe> vibe;
    bool enableViBe = false;

    cv::Mat img_codeBook;
    std::shared_ptr<CodeBook> codeBook;
    bool enableCodeBook = false;
    
    std::shared_ptr<tools::ForegroundMaskAnalysis> foregroundMaskAnalysis;
    bool enableForegroundMaskAnalysis = false;

  public:
    FrameProcessor();
    ~FrameProcessor();

    long frameToStop;
    std::string imgref;

    void init();
    void process(const cv::Mat &img_input);
    void finish(void);

  private:
    void process(const std::string name, const std::shared_ptr<IBGS> &bgs, const cv::Mat &img_input, cv::Mat &img_bgs);
    void tic(std::string value);
    void toc();
    
    void save_config(cv::FileStorage &fs);
    void load_config(cv::FileStorage &fs);
  };
}
