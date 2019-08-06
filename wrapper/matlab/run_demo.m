%% Installation instructions
% First install the 'Computer Vision System Toolbox OpenCV Interface'
% by double-clicking on 'opencvinterface.mlpkginstall'
%
% More info:
% https://fr.mathworks.com/help/vision/ug/opencv-interface.html
% https://fr.mathworks.com/help/vision/opencv-interface-support-package.html
% https://fr.mathworks.com/matlabcentral/fileexchange/47953-computer-vision-system-toolbox-opencv-interface

%% Compile the BGSLibrary wrapper
% Run 'compile' for your first usage.
% compile;

%% Run demo
demo;

%% Run a specific algorithm
% demo('FrameDifference')
% demo('StaticFrameDifference')
% demo('WeightedMovingMean')
% demo('WeightedMovingVariance')
% demo('MixtureOfGaussianV1') % only for OpenCV 2.x
% demo('MixtureOfGaussianV2')
% demo('AdaptiveBackgroundLearning')
% demo('AdaptiveSelectiveBackgroundLearning')
% demo('GMG') % only on OpenCV >= 2.4.3
% demo('KNN') % only on OpenCV 3.x
% demo('DPAdaptiveMedian')
% demo('DPGrimsonGMM')
% demo('DPZivkovicAGMM')
% demo('DPMean')
% demo('DPWrenGA')
% demo('DPPratiMediod')
% demo('DPEigenbackground')
% demo('DPTexture')
% demo('T2FGMM_UM')
% demo('T2FGMM_UV')
% demo('T2FMRF_UM')
% demo('T2FMRF_UV')
% demo('FuzzySugenoIntegral')
% demo('FuzzyChoquetIntegral')
% demo('LBSimpleGaussian')
% demo('LBFuzzyGaussian')
% demo('LBMixtureOfGaussians')
% demo('LBAdaptiveSOM')
% demo('LBFuzzyAdaptiveSOM')
% demo('LBP_MRF')
% demo('MultiLayer')
% demo('PixelBasedAdaptiveSegmenter')
% demo('VuMeter')
% demo('KDE')
% demo('IndependentMultimodal')
% demo('MultiCue')
% demo('SigmaDelta')
% demo('SuBSENSE')
% demo('LOBSTER')
% demo('PAWCS')
% demo('TwoPoints')
% demo('ViBe')
% demo('CodeBook')