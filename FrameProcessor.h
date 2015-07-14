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

#include "package_bgs/FrameDifferenceBGS.h"
#include "package_bgs/StaticFrameDifferenceBGS.h"
#include "package_bgs/WeightedMovingMeanBGS.h"
#include "package_bgs/WeightedMovingVarianceBGS.h"
//#include "package_bgs/MixtureOfGaussianV1BGS.h"
#include "package_bgs/MixtureOfGaussianV2BGS.h"
#include "package_bgs/AdaptiveBackgroundLearning.h"
#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
#include "package_bgs/GMG.h"
#endif

#include "package_bgs/dp/DPAdaptiveMedianBGS.h"
#include "package_bgs/dp/DPGrimsonGMMBGS.h"
#include "package_bgs/dp/DPZivkovicAGMMBGS.h"
#include "package_bgs/dp/DPMeanBGS.h"
#include "package_bgs/dp/DPWrenGABGS.h"
#include "package_bgs/dp/DPPratiMediodBGS.h"
#include "package_bgs/dp/DPEigenbackgroundBGS.h"
#include "package_bgs/dp/DPTextureBGS.h"

#include "package_bgs/tb/T2FGMM_UM.h"
#include "package_bgs/tb/T2FGMM_UV.h"
#include "package_bgs/tb/T2FMRF_UM.h"
#include "package_bgs/tb/T2FMRF_UV.h"
#include "package_bgs/tb/FuzzySugenoIntegral.h"
#include "package_bgs/tb/FuzzyChoquetIntegral.h"

#include "package_bgs/lb/LBSimpleGaussian.h"
#include "package_bgs/lb/LBFuzzyGaussian.h"
#include "package_bgs/lb/LBMixtureOfGaussians.h"
#include "package_bgs/lb/LBAdaptiveSOM.h"
#include "package_bgs/lb/LBFuzzyAdaptiveSOM.h"

#include "package_bgs/ck/LbpMrf.h"

//#include "package_bgs/jmo/MultiLayerBGS.h" // don't works with opencv3
// The PBAS algorithm was removed from BGSLibrary because it is
// based on patented algorithm ViBE
// http://www2.ulg.ac.be/telecom/research/vibe/
//#include "package_bgs/pt/PixelBasedAdaptiveSegmenter.h"
#include "package_bgs/av/VuMeter.h"
#include "package_bgs/ae/KDE.h"
#include "package_bgs/db/IndependentMultimodalBGS.h"
#include "package_bgs/sjn/SJN_MultiCueBGS.h"
#include "package_bgs/bl/SigmaDeltaBGS.h"

#include "package_bgs/pl/SuBSENSE.h"
#include "package_bgs/pl/LOBSTER.h"

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

    cv::Mat img_prep;
    PreProcessor* preProcessor;
    bool enablePreProcessor;

    cv::Mat img_framediff;
    FrameDifferenceBGS* frameDifference;
    bool enableFrameDifferenceBGS;

    cv::Mat img_staticfdiff;
    StaticFrameDifferenceBGS* staticFrameDifference;
    bool enableStaticFrameDifferenceBGS;

    cv::Mat img_wmovmean;
    WeightedMovingMeanBGS* weightedMovingMean;
    bool enableWeightedMovingMeanBGS;

    cv::Mat img_movvar;
    WeightedMovingVarianceBGS* weightedMovingVariance;
    bool enableWeightedMovingVarianceBGS;

//    cv::Mat img_mog1;
//    MixtureOfGaussianV1BGS* mixtureOfGaussianV1BGS;
//    bool enableMixtureOfGaussianV1BGS;

    cv::Mat img_mog2;
    MixtureOfGaussianV2BGS* mixtureOfGaussianV2BGS;
    bool enableMixtureOfGaussianV2BGS;

    cv::Mat img_bkgl_fgmask;
    AdaptiveBackgroundLearning* adaptiveBackgroundLearning;
    bool enableAdaptiveBackgroundLearning;

#if CV_MAJOR_VERSION >= 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    cv::Mat img_gmg;
    GMG* gmg;
    bool enableGMG;
#endif

    cv::Mat img_adpmed;
    DPAdaptiveMedianBGS* adaptiveMedian;
    bool enableDPAdaptiveMedianBGS;

    cv::Mat img_grigmm;
    DPGrimsonGMMBGS* grimsonGMM;
    bool enableDPGrimsonGMMBGS;

    cv::Mat img_zivgmm;
    DPZivkovicAGMMBGS* zivkovicAGMM;
    bool enableDPZivkovicAGMMBGS;

    cv::Mat img_tmpmean;
    DPMeanBGS* temporalMean;
    bool enableDPMeanBGS;

    cv::Mat img_wrenga;
    DPWrenGABGS* wrenGA;
    bool enableDPWrenGABGS;

    cv::Mat img_pramed;
    DPPratiMediodBGS* pratiMediod;
    bool enableDPPratiMediodBGS;

    cv::Mat img_eigbkg;
    DPEigenbackgroundBGS* eigenBackground;
    bool enableDPEigenbackgroundBGS;

    cv::Mat img_texbgs;
    DPTextureBGS* textureBGS;
    bool enableDPTextureBGS;

    cv::Mat img_t2fgmm_um;
    T2FGMM_UM* type2FuzzyGMM_UM;
    bool enableT2FGMM_UM;

    cv::Mat img_t2fgmm_uv;
    T2FGMM_UV* type2FuzzyGMM_UV;
    bool enableT2FGMM_UV;

    cv::Mat img_t2fmrf_um;
    T2FMRF_UM* type2FuzzyMRF_UM;
    bool enableT2FMRF_UM;

    cv::Mat img_t2fmrf_uv;
    T2FMRF_UV* type2FuzzyMRF_UV;
    bool enableT2FMRF_UV;

    cv::Mat img_fsi;
    FuzzySugenoIntegral* fuzzySugenoIntegral;
    bool enableFuzzySugenoIntegral;

    cv::Mat img_fci;
    FuzzyChoquetIntegral* fuzzyChoquetIntegral;
    bool enableFuzzyChoquetIntegral;

    cv::Mat img_lb_sg;
    LBSimpleGaussian* lbSimpleGaussian;
    bool enableLBSimpleGaussian;

    cv::Mat img_lb_fg;
    LBFuzzyGaussian* lbFuzzyGaussian;
    bool enableLBFuzzyGaussian;

    cv::Mat img_lb_mog;
    LBMixtureOfGaussians* lbMixtureOfGaussians;
    bool enableLBMixtureOfGaussians;

    cv::Mat img_lb_som;
    LBAdaptiveSOM* lbAdaptiveSOM;
    bool enableLBAdaptiveSOM;

    cv::Mat img_lb_fsom;
    LBFuzzyAdaptiveSOM* lbFuzzyAdaptiveSOM;
    bool enableLBFuzzyAdaptiveSOM;

    cv::Mat img_lbp_mrf;
    LbpMrf* lbpMrf;
    bool enableLbpMrf;

    // don't works with opencv3
    /*cv::Mat img_mlbgs;
    MultiLayerBGS* multiLayerBGS;
    bool enableMultiLayerBGS;*/

    //cv::Mat img_pt_pbas;
    //PixelBasedAdaptiveSegmenter* pixelBasedAdaptiveSegmenter;
    //bool enablePBAS;

    cv::Mat img_vumeter;
    VuMeter* vuMeter;
    bool enableVuMeter;

    cv::Mat img_kde;
    KDE* kde;
    bool enableKDE;

    cv::Mat img_imbs;
    IndependentMultimodalBGS* imbs;
    bool enableIMBS;

    cv::Mat img_mcbgs;
    SJN_MultiCueBGS* mcbgs;
    bool enableMultiCueBGS;

    cv::Mat img_sdbgs;
    SigmaDeltaBGS* sdbgs;
    bool enableSigmaDeltaBGS;

    cv::Mat img_ssbgs;
    SuBSENSEBGS* ssbgs;
    bool enableSuBSENSEBGS;

    cv::Mat img_lobgs;
    LOBSTERBGS* lobgs;
    bool enableLOBSTERBGS;

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
