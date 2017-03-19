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
#pragma once
#pragma warning(disable : 4482)

#include "IFrameProcessor.h"
#include "PreProcessor.h"

#include "package_bgs/IBGS.h"
#include "package_bgs/bgslibrary.h"
#include "package_analysis/ForegroundMaskAnalysis.h"

namespace bgslibrary
{
  class FrameProcessor : public IFrameProcessor
  {
  private:
    bool firstTime;
    long frameNumber;
    std::string processname;
    double duration;
    std::string tictoc;

    cv::Mat img_preProcessor;
    PreProcessor* preProcessor;
    bool enablePreProcessor;

    cv::Mat img_frameDifference;
    FrameDifference* frameDifference;
    bool enableFrameDifference;

    cv::Mat img_staticFrameDifference;
    StaticFrameDifference* staticFrameDifference;
    bool enableStaticFrameDifference;

    cv::Mat img_weightedMovingMean;
    WeightedMovingMean* weightedMovingMean;
    bool enableWeightedMovingMean;

    cv::Mat img_weightedMovingVariance;
    WeightedMovingVariance* weightedMovingVariance;
    bool enableWeightedMovingVariance;

#if CV_MAJOR_VERSION == 2
    cv::Mat img_mixtureOfGaussianV1;
    MixtureOfGaussianV1* mixtureOfGaussianV1;
    bool enableMixtureOfGaussianV1;
#endif

    cv::Mat img_mixtureOfGaussianV2;
    MixtureOfGaussianV2* mixtureOfGaussianV2;
    bool enableMixtureOfGaussianV2;

    cv::Mat img_adaptiveBackgroundLearning;
    AdaptiveBackgroundLearning* adaptiveBackgroundLearning;
    bool enableAdaptiveBackgroundLearning;

#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    cv::Mat img_gmg;
    GMG* gmg;
    bool enableGMG;
#endif

#if CV_MAJOR_VERSION == 3
    cv::Mat img_knn;
    KNN* knn;
    bool enableKNN;
#endif

    cv::Mat img_dpAdaptiveMedian;
    DPAdaptiveMedian* dpAdaptiveMedian;
    bool enableDPAdaptiveMedian;

    cv::Mat img_dpGrimsonGMM;
    DPGrimsonGMM* dpGrimsonGMM;
    bool enableDPGrimsonGMM;

    cv::Mat img_dpZivkovicAGMM;
    DPZivkovicAGMM* dpZivkovicAGMM;
    bool enableDPZivkovicAGMM;

    cv::Mat img_dpTemporalMean;
    DPMean* dpTemporalMean;
    bool enableDPMean;

    cv::Mat img_dpWrenGA;
    DPWrenGA* dpWrenGA;
    bool enableDPWrenGA;

    cv::Mat img_dpPratiMediod;
    DPPratiMediod* dpPratiMediod;
    bool enableDPPratiMediod;

    cv::Mat img_dpEigenBackground;
    DPEigenbackground* dpEigenBackground;
    bool enableDPEigenbackground;

    cv::Mat img_dpTexture;
    DPTexture* dpTexture;
    bool enableDPTexture;

    cv::Mat img_type2FuzzyGMM_UM;
    T2FGMM_UM* type2FuzzyGMM_UM;
    bool enableT2FGMM_UM;

    cv::Mat img_type2FuzzyGMM_UV;
    T2FGMM_UV* type2FuzzyGMM_UV;
    bool enableT2FGMM_UV;

    cv::Mat img_type2FuzzyMRF_UM;
    T2FMRF_UM* type2FuzzyMRF_UM;
    bool enableT2FMRF_UM;

    cv::Mat img_type2FuzzyMRF_UV;
    T2FMRF_UV* type2FuzzyMRF_UV;
    bool enableT2FMRF_UV;

    cv::Mat img_fuzzySugenoIntegral;
    FuzzySugenoIntegral* fuzzySugenoIntegral;
    bool enableFuzzySugenoIntegral;

    cv::Mat img_fuzzyChoquetIntegral;
    FuzzyChoquetIntegral* fuzzyChoquetIntegral;
    bool enableFuzzyChoquetIntegral;

    cv::Mat img_lbSimpleGaussian;
    LBSimpleGaussian* lbSimpleGaussian;
    bool enableLBSimpleGaussian;

    cv::Mat img_lbFuzzyGaussian;
    LBFuzzyGaussian* lbFuzzyGaussian;
    bool enableLBFuzzyGaussian;

    cv::Mat img_lbMixtureOfGaussians;
    LBMixtureOfGaussians* lbMixtureOfGaussians;
    bool enableLBMixtureOfGaussians;

    cv::Mat img_lbAdaptiveSOM;
    LBAdaptiveSOM* lbAdaptiveSOM;
    bool enableLBAdaptiveSOM;

    cv::Mat img_lbFuzzyAdaptiveSOM;
    LBFuzzyAdaptiveSOM* lbFuzzyAdaptiveSOM;
    bool enableLBFuzzyAdaptiveSOM;

    cv::Mat img_lbpMrf;
    LBP_MRF* lbpMrf;
    bool enableLbpMrf;

    cv::Mat img_multiLayer;
    MultiLayer* multiLayer;
    bool enableMultiLayer;

    cv::Mat img_pixelBasedAdaptiveSegmenter;
    PixelBasedAdaptiveSegmenter* pixelBasedAdaptiveSegmenter;
    bool enablePBAS;

    cv::Mat img_vumeter;
    VuMeter* vuMeter;
    bool enableVuMeter;

    cv::Mat img_kde;
    KDE* kde;
    bool enableKDE;

    cv::Mat img_imbs;
    IndependentMultimodal* imbs;
    bool enableIMBS;

    cv::Mat img_multiCue;
    MultiCue* multiCue;
    bool enableMultiCue;

    cv::Mat img_sigmaDelta;
    SigmaDelta* sigmaDelta;
    bool enableSigmaDelta;

    cv::Mat img_subSENSE;
    SuBSENSE* subSENSE;
    bool enableSuBSENSE;

    cv::Mat img_lobster;
    LOBSTER* lobster;
    bool enableLOBSTER;

    cv::Mat img_pawcs;
    PAWCS* pawcs;
    bool enablePAWCS;

    cv::Mat img_twoPoints;
    TwoPoints* twoPoints;
    bool enableTwoPoints;

    cv::Mat img_vibe;
    ViBe* vibe;
    bool enableViBe;

    cv::Mat img_codeBook;
    CodeBook* codeBook;
    bool enableCodeBook;

    ForegroundMaskAnalysis* foregroundMaskAnalysis;
    bool enableForegroundMaskAnalysis;

  public:
    FrameProcessor();
    ~FrameProcessor();

    long frameToStop;
    std::string imgref;

    void init();
    void process(const cv::Mat &img_input);
    void finish(void);

  private:
    void process(std::string name, IBGS *bgs, const cv::Mat &img_input, cv::Mat &img_bgs);
    void tic(std::string value);
    void toc();

    void saveConfig();
    void loadConfig();
  };
}
