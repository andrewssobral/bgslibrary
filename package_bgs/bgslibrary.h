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

#include "FrameDifference.h"
#include "StaticFrameDifference.h"
#include "WeightedMovingMean.h"
#include "WeightedMovingVariance.h"
#include "MixtureOfGaussianV1.h" // Only for OpenCV >= 2
#include "MixtureOfGaussianV2.h"
#include "AdaptiveBackgroundLearning.h"
#include "AdaptiveSelectiveBackgroundLearning.h"
#include "KNN.h" // Only for OpenCV >= 3
#include "GMG.h" // Only for OpenCV >= 2.4.3
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

//#include "_template_/MyBGS.h"
//#include "_template_/Amber.h"

using namespace bgslibrary::algorithms;
