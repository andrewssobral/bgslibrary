#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bgslibrary_gui
TEMPLATE = app

# For Windows x64 + Visual Studio 2015 + OpenCV 3.1.0
#INCLUDEPATH += C:/OpenCV3.1.0/build/include
#LIBS += -LC:/OpenCV3.1.0/build/x64/vc14/lib -lopencv_world310

# For Windows x64 + Visual Studio 2015 + OpenCV 3.2.0
#INCLUDEPATH += C:/OpenCV3.2.0/build/include
#LIBS += -LC:/OpenCV3.2.0/build/x64/vc14/lib -lopencv_world320

# For Windows x64 + Visual Studio 2017 + OpenCV 3.4.2
INCLUDEPATH += E:/OpenCV3.4.2/build/include
INCLUDEPATH += E:/OpenCV3.4.2/build/include/opencv
LIBS += -LE:/OpenCV3.4.2/build/x64/vc15/lib -lopencv_world342

# For Linux
# INCLUDEPATH += /usr/local/include/opencv
# LIBS += -L/usr/local/lib

RESOURCES = application.qrc

SOURCES += bgslibrary_gui.cpp\
    mainwindow.cpp \
    qt_utils.cpp \
    texteditor.cpp \
    ../package_analysis/ForegroundMaskAnalysis.cpp \
    ../package_analysis/PerformanceUtils.cpp \
    ../package_analysis/PixelUtils.cpp \
    ../package_bgs/_template_/Amber.cpp \
    ../package_bgs/_template_/MyBGS.cpp \
    ../package_bgs/dp/AdaptiveMedianBGS.cpp \
    ../package_bgs/dp/Eigenbackground.cpp \
    ../package_bgs/dp/Error.cpp \
    ../package_bgs/dp/GrimsonGMM.cpp \
    ../package_bgs/dp/Image.cpp \
    ../package_bgs/dp/MeanBGS.cpp \
    ../package_bgs/dp/PratiMediodBGS.cpp \
    ../package_bgs/dp/TextureBGS.cpp \
    ../package_bgs/dp/WrenGA.cpp \
    ../package_bgs/dp/ZivkovicAGMM.cpp \
    ../package_bgs/IMBS/IMBS.cpp \
    ../package_bgs/KDE/KernelTable.cpp \
    ../package_bgs/KDE/NPBGmodel.cpp \
    ../package_bgs/KDE/NPBGSubtractor.cpp \
    ../package_bgs/lb/BGModel.cpp \
    ../package_bgs/lb/BGModelFuzzyGauss.cpp \
    ../package_bgs/lb/BGModelFuzzySom.cpp \
    ../package_bgs/lb/BGModelGauss.cpp \
    ../package_bgs/lb/BGModelMog.cpp \
    ../package_bgs/lb/BGModelSom.cpp \
    ../package_bgs/LBP_MRF/graph.cpp \
    ../package_bgs/LBP_MRF/maxflow.cpp \
    ../package_bgs/LBP_MRF/MEDefs.cpp \
    ../package_bgs/LBP_MRF/MEHistogram.cpp \
    ../package_bgs/LBP_MRF/MEImage.cpp \
    ../package_bgs/LBP_MRF/MotionDetection.cpp \
    ../package_bgs/LBSP/BackgroundSubtractorLBSP.cpp \
    ../package_bgs/LBSP/BackgroundSubtractorLBSP_.cpp \
    ../package_bgs/LBSP/BackgroundSubtractorLOBSTER.cpp \
    ../package_bgs/LBSP/BackgroundSubtractorPAWCS.cpp \
    ../package_bgs/LBSP/BackgroundSubtractorSuBSENSE.cpp \
    ../package_bgs/LBSP/LBSP.cpp \
    ../package_bgs/LBSP/LBSP_.cpp \
    ../package_bgs/MultiLayer/blob.cpp \
    ../package_bgs/MultiLayer/BlobExtraction.cpp \
    ../package_bgs/MultiLayer/BlobResult.cpp \
    ../package_bgs/MultiLayer/CMultiLayerBGS.cpp \
    ../package_bgs/MultiLayer/LocalBinaryPattern.cpp \
    ../package_bgs/PBAS/PBAS.cpp \
    ../package_bgs/SigmaDelta/sdLaMa091.cpp \
    ../package_bgs/T2F/FuzzyUtils.cpp \
    ../package_bgs/T2F/MRF.cpp \
    ../package_bgs/T2F/T2FGMM.cpp \
    ../package_bgs/T2F/T2FMRF.cpp \
    ../package_bgs/TwoPoints/two_points.cpp \
    ../package_bgs/ViBe/vibe-background-sequential.cpp \
    ../package_bgs/VuMeter/TBackground.cpp \
    ../package_bgs/VuMeter/TBackgroundVuMeter.cpp \
    ../package_bgs/AdaptiveBackgroundLearning.cpp \
    ../package_bgs/AdaptiveSelectiveBackgroundLearning.cpp \
    ../package_bgs/DPAdaptiveMedian.cpp \
    ../package_bgs/DPEigenbackground.cpp \
    ../package_bgs/DPGrimsonGMM.cpp \
    ../package_bgs/DPMean.cpp \
    ../package_bgs/DPPratiMediod.cpp \
    ../package_bgs/DPTexture.cpp \
    ../package_bgs/DPWrenGA.cpp \
    ../package_bgs/DPZivkovicAGMM.cpp \
    ../package_bgs/FrameDifference.cpp \
    ../package_bgs/FuzzyChoquetIntegral.cpp \
    ../package_bgs/FuzzySugenoIntegral.cpp \
    ../package_bgs/GMG.cpp \
    ../package_bgs/IndependentMultimodal.cpp \
    ../package_bgs/KDE.cpp \
    ../package_bgs/KNN.cpp \
    ../package_bgs/LBAdaptiveSOM.cpp \
    ../package_bgs/LBFuzzyAdaptiveSOM.cpp \
    ../package_bgs/LBFuzzyGaussian.cpp \
    ../package_bgs/LBMixtureOfGaussians.cpp \
    ../package_bgs/LBP_MRF.cpp \
    ../package_bgs/LBSimpleGaussian.cpp \
    ../package_bgs/LOBSTER.cpp \
    ../package_bgs/MixtureOfGaussianV1.cpp \
    ../package_bgs/MixtureOfGaussianV2.cpp \
    ../package_bgs/MultiCue.cpp \
    ../package_bgs/MultiLayer.cpp \
    ../package_bgs/PAWCS.cpp \
    ../package_bgs/PixelBasedAdaptiveSegmenter.cpp \
    ../package_bgs/SigmaDelta.cpp \
    ../package_bgs/StaticFrameDifference.cpp \
    ../package_bgs/SuBSENSE.cpp \
    ../package_bgs/T2FGMM_UM.cpp \
    ../package_bgs/T2FGMM_UV.cpp \
    ../package_bgs/T2FMRF_UM.cpp \
    ../package_bgs/T2FMRF_UV.cpp \
    ../package_bgs/TwoPoints.cpp \
    ../package_bgs/ViBe.cpp \
    ../package_bgs/VuMeter.cpp \
    ../package_bgs/WeightedMovingMean.cpp \
    ../package_bgs/WeightedMovingVariance.cpp \
    ../package_bgs/_template_/amber/amber.c \
    ../package_bgs/CodeBook.cpp

HEADERS  += mainwindow.h \
    qt_utils.h \
    texteditor.h \
    ../package_analysis/ForegroundMaskAnalysis.h \
    ../package_analysis/PerformanceUtils.h \
    ../package_analysis/PixelUtils.h \
    ../package_bgs/_template_/amber/amber.h \
    ../package_bgs/_template_/Amber.h \
    ../package_bgs/_template_/MyBGS.h \
    ../package_bgs/dp/AdaptiveMedianBGS.h \
    ../package_bgs/dp/Bgs.h \
    ../package_bgs/dp/BgsParams.h \
    ../package_bgs/dp/Eigenbackground.h \
    ../package_bgs/dp/Error.h \
    ../package_bgs/dp/GrimsonGMM.h \
    ../package_bgs/dp/Image.h \
    ../package_bgs/dp/MeanBGS.h \
    ../package_bgs/dp/PratiMediodBGS.h \
    ../package_bgs/dp/TextureBGS.h \
    ../package_bgs/dp/WrenGA.h \
    ../package_bgs/dp/ZivkovicAGMM.h \
    ../package_bgs/IMBS/IMBS.hpp \
    ../package_bgs/KDE/KernelTable.h \
    ../package_bgs/KDE/NPBGmodel.h \
    ../package_bgs/KDE/NPBGSubtractor.h \
    ../package_bgs/lb/BGModel.h \
    ../package_bgs/lb/BGModelFuzzyGauss.h \
    ../package_bgs/lb/BGModelFuzzySom.h \
    ../package_bgs/lb/BGModelGauss.h \
    ../package_bgs/lb/BGModelMog.h \
    ../package_bgs/lb/BGModelSom.h \
    ../package_bgs/lb/Types.h \
    ../package_bgs/LBP_MRF/block.h \
    ../package_bgs/LBP_MRF/graph.h \
    ../package_bgs/LBP_MRF/MEDefs.hpp \
    ../package_bgs/LBP_MRF/MEHistogram.hpp \
    ../package_bgs/LBP_MRF/MEImage.hpp \
    ../package_bgs/LBP_MRF/MotionDetection.hpp \
    ../package_bgs/LBSP/BackgroundSubtractorLBSP.h \
    ../package_bgs/LBSP/BackgroundSubtractorLBSP_.h \
    ../package_bgs/LBSP/BackgroundSubtractorLOBSTER.h \
    ../package_bgs/LBSP/BackgroundSubtractorPAWCS.h \
    ../package_bgs/LBSP/BackgroundSubtractorSuBSENSE.h \
    ../package_bgs/LBSP/DistanceUtils.h \
    ../package_bgs/LBSP/LBSP.h \
    ../package_bgs/LBSP/LBSP_.h \
    ../package_bgs/LBSP/RandUtils.h \
    ../package_bgs/MultiLayer/BackgroundSubtractionAPI.h \
    ../package_bgs/MultiLayer/BGS.h \
    ../package_bgs/MultiLayer/blob.h \
    ../package_bgs/MultiLayer/BlobExtraction.h \
    ../package_bgs/MultiLayer/BlobLibraryConfiguration.h \
    ../package_bgs/MultiLayer/BlobResult.h \
    ../package_bgs/MultiLayer/CMultiLayerBGS.h \
    ../package_bgs/MultiLayer/LocalBinaryPattern.h \
    ../package_bgs/MultiLayer/OpenCvDataConversion.h \
    ../package_bgs/MultiLayer/OpenCvLegacyIncludes.h \
    ../package_bgs/PBAS/PBAS.h \
    ../package_bgs/SigmaDelta/sdLaMa091.h \
    ../package_bgs/T2F/FuzzyUtils.h \
    ../package_bgs/T2F/MRF.h \
    ../package_bgs/T2F/T2FGMM.h \
    ../package_bgs/T2F/T2FMRF.h \
    ../package_bgs/TwoPoints/two_points.h \
    ../package_bgs/ViBe/vibe-background-sequential.h \
    ../package_bgs/VuMeter/TBackground.h \
    ../package_bgs/VuMeter/TBackgroundVuMeter.h \
    ../package_bgs/AdaptiveBackgroundLearning.h \
    ../package_bgs/AdaptiveSelectiveBackgroundLearning.h \
    ../package_bgs/bgslibrary.h \
    ../package_bgs/DPAdaptiveMedian.h \
    ../package_bgs/DPEigenbackground.h \
    ../package_bgs/DPGrimsonGMM.h \
    ../package_bgs/DPMean.h \
    ../package_bgs/DPPratiMediod.h \
    ../package_bgs/DPTexture.h \
    ../package_bgs/DPWrenGA.h \
    ../package_bgs/DPZivkovicAGMM.h \
    ../package_bgs/FrameDifference.h \
    ../package_bgs/FuzzyChoquetIntegral.h \
    ../package_bgs/FuzzySugenoIntegral.h \
    ../package_bgs/GMG.h \
    ../package_bgs/IBGS.h \
    ../package_bgs/IndependentMultimodal.h \
    ../package_bgs/KDE.h \
    ../package_bgs/KNN.h \
    ../package_bgs/LBAdaptiveSOM.h \
    ../package_bgs/LBFuzzyAdaptiveSOM.h \
    ../package_bgs/LBFuzzyGaussian.h \
    ../package_bgs/LBMixtureOfGaussians.h \
    ../package_bgs/LBP_MRF.h \
    ../package_bgs/LBSimpleGaussian.h \
    ../package_bgs/LOBSTER.h \
    ../package_bgs/MixtureOfGaussianV1.h \
    ../package_bgs/MixtureOfGaussianV2.h \
    ../package_bgs/MultiCue.h \
    ../package_bgs/MultiLayer.h \
    ../package_bgs/PAWCS.h \
    ../package_bgs/PixelBasedAdaptiveSegmenter.h \
    ../package_bgs/SigmaDelta.h \
    ../package_bgs/StaticFrameDifference.h \
    ../package_bgs/SuBSENSE.h \
    ../package_bgs/T2FGMM_UM.h \
    ../package_bgs/T2FGMM_UV.h \
    ../package_bgs/T2FMRF_UM.h \
    ../package_bgs/T2FMRF_UV.h \
    ../package_bgs/TwoPoints.h \
    ../package_bgs/ViBe.h \
    ../package_bgs/VuMeter.h \
    ../package_bgs/WeightedMovingMean.h \
    ../package_bgs/WeightedMovingVariance.h \
    ../package_bgs/CodeBook.h

FORMS    += mainwindow.ui

DISTFILES += \
    ../package_bgs/LBSP/LBSP_16bits_dbcross_1ch.i \
    ../package_bgs/LBSP/LBSP_16bits_dbcross_3ch1t.i \
    ../package_bgs/LBSP/LBSP_16bits_dbcross_3ch3t.i \
    ../package_bgs/LBSP/LBSP_16bits_dbcross_s3ch.i \
    ../package_bgs/ViBe/LICENSE
