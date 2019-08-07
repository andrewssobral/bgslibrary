#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bgslibrary_gui
TEMPLATE = app

# For Windows x64 + Visual Studio 2015 + OpenCV 4.1.1
INCLUDEPATH += E:/OpenCV/opencv-4.1.1/build/include
Release:LIBS += -LE:/OpenCV/opencv-4.1.1/build/x64/vc14/lib -lopencv_world411
Debug:LIBS += -LE:/OpenCV/opencv-4.1.1/build/x64/vc14/lib -lopencv_world411d

# For Linux
# INCLUDEPATH += /usr/local/include/opencv
# LIBS += -L/usr/local/lib

RESOURCES = application.qrc

SOURCES += bgslibrary_gui.cpp\
    mainwindow.cpp \
    qt_utils.cpp \
    texteditor.cpp \
    ../../src/package_analysis/ForegroundMaskAnalysis.cpp \
    ../../src/package_analysis/PerformanceUtils.cpp \
    ../../src/package_analysis/PixelUtils.cpp \
    ../../src/package_bgs/dp/AdaptiveMedianBGS.cpp \
    ../../src/package_bgs/dp/Eigenbackground.cpp \
    ../../src/package_bgs/dp/Error.cpp \
    ../../src/package_bgs/dp/GrimsonGMM.cpp \
    ../../src/package_bgs/dp/Image.cpp \
    ../../src/package_bgs/dp/MeanBGS.cpp \
    ../../src/package_bgs/dp/PratiMediodBGS.cpp \
    ../../src/package_bgs/dp/TextureBGS.cpp \
    ../../src/package_bgs/dp/WrenGA.cpp \
    ../../src/package_bgs/dp/ZivkovicAGMM.cpp \
    ../../src/package_bgs/IMBS/IMBS.cpp \
    ../../src/package_bgs/KDE/KernelTable.cpp \
    ../../src/package_bgs/KDE/NPBGmodel.cpp \
    ../../src/package_bgs/KDE/NPBGSubtractor.cpp \
    ../../src/package_bgs/lb/BGModel.cpp \
    ../../src/package_bgs/lb/BGModelFuzzyGauss.cpp \
    ../../src/package_bgs/lb/BGModelFuzzySom.cpp \
    ../../src/package_bgs/lb/BGModelGauss.cpp \
    ../../src/package_bgs/lb/BGModelMog.cpp \
    ../../src/package_bgs/lb/BGModelSom.cpp \
    ../../src/package_bgs/LBP_MRF/graph.cpp \
    ../../src/package_bgs/LBP_MRF/maxflow.cpp \
    ../../src/package_bgs/LBP_MRF/MEDefs.cpp \
    ../../src/package_bgs/LBP_MRF/MEHistogram.cpp \
    ../../src/package_bgs/LBP_MRF/MEImage.cpp \
    ../../src/package_bgs/LBP_MRF/MotionDetection.cpp \
    ../../src/package_bgs/LBSP/BackgroundSubtractorLBSP.cpp \
    ../../src/package_bgs/LBSP/BackgroundSubtractorLBSP_.cpp \
    ../../src/package_bgs/LBSP/BackgroundSubtractorLOBSTER.cpp \
    ../../src/package_bgs/LBSP/BackgroundSubtractorPAWCS.cpp \
    ../../src/package_bgs/LBSP/BackgroundSubtractorSuBSENSE.cpp \
    ../../src/package_bgs/LBSP/LBSP.cpp \
    ../../src/package_bgs/LBSP/LBSP_.cpp \
    ../../src/package_bgs/MultiLayer/blob.cpp \
    ../../src/package_bgs/MultiLayer/BlobExtraction.cpp \
    ../../src/package_bgs/MultiLayer/BlobResult.cpp \
    ../../src/package_bgs/MultiLayer/CMultiLayerBGS.cpp \
    ../../src/package_bgs/MultiLayer/LocalBinaryPattern.cpp \
    ../../src/package_bgs/PBAS/PBAS.cpp \
    ../../src/package_bgs/SigmaDelta/sdLaMa091.cpp \
    ../../src/package_bgs/T2F/FuzzyUtils.cpp \
    ../../src/package_bgs/T2F/MRF.cpp \
    ../../src/package_bgs/T2F/T2FGMM.cpp \
    ../../src/package_bgs/T2F/T2FMRF.cpp \
    ../../src/package_bgs/TwoPoints/two_points.cpp \
    ../../src/package_bgs/ViBe/vibe-background-sequential.cpp \
    ../../src/package_bgs/VuMeter/TBackground.cpp \
    ../../src/package_bgs/VuMeter/TBackgroundVuMeter.cpp \
    ../../src/package_bgs/AdaptiveBackgroundLearning.cpp \
    ../../src/package_bgs/AdaptiveSelectiveBackgroundLearning.cpp \
    ../../src/package_bgs/DPAdaptiveMedian.cpp \
    ../../src/package_bgs/DPEigenbackground.cpp \
    ../../src/package_bgs/DPGrimsonGMM.cpp \
    ../../src/package_bgs/DPMean.cpp \
    ../../src/package_bgs/DPPratiMediod.cpp \
    ../../src/package_bgs/DPTexture.cpp \
    ../../src/package_bgs/DPWrenGA.cpp \
    ../../src/package_bgs/DPZivkovicAGMM.cpp \
    ../../src/package_bgs/FrameDifference.cpp \
    ../../src/package_bgs/FuzzyChoquetIntegral.cpp \
    ../../src/package_bgs/FuzzySugenoIntegral.cpp \
    ../../src/package_bgs/GMG.cpp \
    ../../src/package_bgs/IndependentMultimodal.cpp \
    ../../src/package_bgs/KDE.cpp \
    ../../src/package_bgs/KNN.cpp \
    ../../src/package_bgs/LBAdaptiveSOM.cpp \
    ../../src/package_bgs/LBFuzzyAdaptiveSOM.cpp \
    ../../src/package_bgs/LBFuzzyGaussian.cpp \
    ../../src/package_bgs/LBMixtureOfGaussians.cpp \
    ../../src/package_bgs/LBP_MRF.cpp \
    ../../src/package_bgs/LBSimpleGaussian.cpp \
    ../../src/package_bgs/LOBSTER.cpp \
    ../../src/package_bgs/MixtureOfGaussianV1.cpp \
    ../../src/package_bgs/MixtureOfGaussianV2.cpp \
    ../../src/package_bgs/MultiCue.cpp \
    ../../src/package_bgs/MultiLayer.cpp \
    ../../src/package_bgs/PAWCS.cpp \
    ../../src/package_bgs/PixelBasedAdaptiveSegmenter.cpp \
    ../../src/package_bgs/SigmaDelta.cpp \
    ../../src/package_bgs/StaticFrameDifference.cpp \
    ../../src/package_bgs/SuBSENSE.cpp \
    ../../src/package_bgs/T2FGMM_UM.cpp \
    ../../src/package_bgs/T2FGMM_UV.cpp \
    ../../src/package_bgs/T2FMRF_UM.cpp \
    ../../src/package_bgs/T2FMRF_UV.cpp \
    ../../src/package_bgs/TwoPoints.cpp \
    ../../src/package_bgs/ViBe.cpp \
    ../../src/package_bgs/VuMeter.cpp \
    ../../src/package_bgs/WeightedMovingMean.cpp \
    ../../src/package_bgs/WeightedMovingVariance.cpp \
    ../../src/package_bgs/CodeBook.cpp
    ../../src/package_bgs/_template_/MyBGS.cpp \

HEADERS  += mainwindow.h \
    qt_utils.h \
    texteditor.h \
    ../../src/package_analysis/ForegroundMaskAnalysis.h \
    ../../src/package_analysis/PerformanceUtils.h \
    ../../src/package_analysis/PixelUtils.h \
    ../../src/package_bgs/dp/AdaptiveMedianBGS.h \
    ../../src/package_bgs/dp/Bgs.h \
    ../../src/package_bgs/dp/BgsParams.h \
    ../../src/package_bgs/dp/Eigenbackground.h \
    ../../src/package_bgs/dp/Error.h \
    ../../src/package_bgs/dp/GrimsonGMM.h \
    ../../src/package_bgs/dp/Image.h \
    ../../src/package_bgs/dp/MeanBGS.h \
    ../../src/package_bgs/dp/PratiMediodBGS.h \
    ../../src/package_bgs/dp/TextureBGS.h \
    ../../src/package_bgs/dp/WrenGA.h \
    ../../src/package_bgs/dp/ZivkovicAGMM.h \
    ../../src/package_bgs/IMBS/IMBS.hpp \
    ../../src/package_bgs/KDE/KernelTable.h \
    ../../src/package_bgs/KDE/NPBGmodel.h \
    ../../src/package_bgs/KDE/NPBGSubtractor.h \
    ../../src/package_bgs/lb/BGModel.h \
    ../../src/package_bgs/lb/BGModelFuzzyGauss.h \
    ../../src/package_bgs/lb/BGModelFuzzySom.h \
    ../../src/package_bgs/lb/BGModelGauss.h \
    ../../src/package_bgs/lb/BGModelMog.h \
    ../../src/package_bgs/lb/BGModelSom.h \
    ../../src/package_bgs/lb/Types.h \
    ../../src/package_bgs/LBP_MRF/block.h \
    ../../src/package_bgs/LBP_MRF/graph.h \
    ../../src/package_bgs/LBP_MRF/MEDefs.hpp \
    ../../src/package_bgs/LBP_MRF/MEHistogram.hpp \
    ../../src/package_bgs/LBP_MRF/MEImage.hpp \
    ../../src/package_bgs/LBP_MRF/MotionDetection.hpp \
    ../../src/package_bgs/LBSP/BackgroundSubtractorLBSP.h \
    ../../src/package_bgs/LBSP/BackgroundSubtractorLBSP_.h \
    ../../src/package_bgs/LBSP/BackgroundSubtractorLOBSTER.h \
    ../../src/package_bgs/LBSP/BackgroundSubtractorPAWCS.h \
    ../../src/package_bgs/LBSP/BackgroundSubtractorSuBSENSE.h \
    ../../src/package_bgs/LBSP/DistanceUtils.h \
    ../../src/package_bgs/LBSP/LBSP.h \
    ../../src/package_bgs/LBSP/LBSP_.h \
    ../../src/package_bgs/LBSP/RandUtils.h \
    ../../src/package_bgs/MultiLayer/BackgroundSubtractionAPI.h \
    ../../src/package_bgs/MultiLayer/BGS.h \
    ../../src/package_bgs/MultiLayer/blob.h \
    ../../src/package_bgs/MultiLayer/BlobExtraction.h \
    ../../src/package_bgs/MultiLayer/BlobLibraryConfiguration.h \
    ../../src/package_bgs/MultiLayer/BlobResult.h \
    ../../src/package_bgs/MultiLayer/CMultiLayerBGS.h \
    ../../src/package_bgs/MultiLayer/LocalBinaryPattern.h \
    ../../src/package_bgs/MultiLayer/OpenCvDataConversion.h \
    ../../src/package_bgs/MultiLayer/OpenCvLegacyIncludes.h \
    ../../src/package_bgs/PBAS/PBAS.h \
    ../../src/package_bgs/SigmaDelta/sdLaMa091.h \
    ../../src/package_bgs/T2F/FuzzyUtils.h \
    ../../src/package_bgs/T2F/MRF.h \
    ../../src/package_bgs/T2F/T2FGMM.h \
    ../../src/package_bgs/T2F/T2FMRF.h \
    ../../src/package_bgs/TwoPoints/two_points.h \
    ../../src/package_bgs/ViBe/vibe-background-sequential.h \
    ../../src/package_bgs/VuMeter/TBackground.h \
    ../../src/package_bgs/VuMeter/TBackgroundVuMeter.h \
    ../../src/package_bgs/AdaptiveBackgroundLearning.h \
    ../../src/package_bgs/AdaptiveSelectiveBackgroundLearning.h \
    ../../src/package_bgs/bgslibrary.h \
    ../../src/package_bgs/DPAdaptiveMedian.h \
    ../../src/package_bgs/DPEigenbackground.h \
    ../../src/package_bgs/DPGrimsonGMM.h \
    ../../src/package_bgs/DPMean.h \
    ../../src/package_bgs/DPPratiMediod.h \
    ../../src/package_bgs/DPTexture.h \
    ../../src/package_bgs/DPWrenGA.h \
    ../../src/package_bgs/DPZivkovicAGMM.h \
    ../../src/package_bgs/FrameDifference.h \
    ../../src/package_bgs/FuzzyChoquetIntegral.h \
    ../../src/package_bgs/FuzzySugenoIntegral.h \
    ../../src/package_bgs/GMG.h \
    ../../src/package_bgs/IBGS.h \
    ../../src/package_bgs/IndependentMultimodal.h \
    ../../src/package_bgs/KDE.h \
    ../../src/package_bgs/KNN.h \
    ../../src/package_bgs/LBAdaptiveSOM.h \
    ../../src/package_bgs/LBFuzzyAdaptiveSOM.h \
    ../../src/package_bgs/LBFuzzyGaussian.h \
    ../../src/package_bgs/LBMixtureOfGaussians.h \
    ../../src/package_bgs/LBP_MRF.h \
    ../../src/package_bgs/LBSimpleGaussian.h \
    ../../src/package_bgs/LOBSTER.h \
    ../../src/package_bgs/MixtureOfGaussianV1.h \
    ../../src/package_bgs/MixtureOfGaussianV2.h \
    ../../src/package_bgs/MultiCue.h \
    ../../src/package_bgs/MultiLayer.h \
    ../../src/package_bgs/PAWCS.h \
    ../../src/package_bgs/PixelBasedAdaptiveSegmenter.h \
    ../../src/package_bgs/SigmaDelta.h \
    ../../src/package_bgs/StaticFrameDifference.h \
    ../../src/package_bgs/SuBSENSE.h \
    ../../src/package_bgs/T2FGMM_UM.h \
    ../../src/package_bgs/T2FGMM_UV.h \
    ../../src/package_bgs/T2FMRF_UM.h \
    ../../src/package_bgs/T2FMRF_UV.h \
    ../../src/package_bgs/TwoPoints.h \
    ../../src/package_bgs/ViBe.h \
    ../../src/package_bgs/VuMeter.h \
    ../../src/package_bgs/WeightedMovingMean.h \
    ../../src/package_bgs/WeightedMovingVariance.h \
    ../../src/package_bgs/CodeBook.h
    ../../src/package_bgs/_template_/MyBGS.h \

FORMS    += mainwindow.ui

DISTFILES += \
    ../../src/package_bgs/LBSP/LBSP_16bits_dbcross_1ch.i \
    ../../src/package_bgs/LBSP/LBSP_16bits_dbcross_3ch1t.i \
    ../../src/package_bgs/LBSP/LBSP_16bits_dbcross_3ch3t.i \
    ../../src/package_bgs/LBSP/LBSP_16bits_dbcross_s3ch.i \
    ../../src/package_bgs/ViBe/LICENSE
