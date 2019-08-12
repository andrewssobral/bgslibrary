#-------------------------------------------------
#
# Project created by QtCreator
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bgslibrary_gui
TEMPLATE = app

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Qt resources can be found in the following locations:)
message(Documentation: $$[QT_INSTALL_DOCS])
message(Header files: $$[QT_INSTALL_HEADERS])
message(Libraries: $$[QT_INSTALL_LIBS])
message(Binary files (executables): $$[QT_INSTALL_BINS])
message(Plugins: $$[QT_INSTALL_PLUGINS])
message(Data files: $$[QT_INSTALL_DATA])
message(Translation files: $$[QT_INSTALL_TRANSLATIONS])
message(Settings: $$[QT_INSTALL_SETTINGS])
message(Examples: $$[QT_INSTALL_EXAMPLES])
message(Demonstrations: $$[QT_INSTALL_DEMOS])

CONFIG += c++14
#CONFIG += no_keywords # Python redefines some qt keywords
#CONFIG += console
CONFIG -= app_bundle
CONFIG += sdk_no_version_check # supress OS warning for 10.14

# For Windows x64 + Visual Studio 2015 + OpenCV 4.1.1
win32 {
  message("Building for Windows")
  INCLUDEPATH += E:/OpenCV/opencv-4.1.1/build/include
  release {
    LIBS += -LE:/OpenCV/opencv-4.1.1/build/x64/vc14/lib -lopencv_world411
  }
  debug {
    LIBS += -LE:/OpenCV/opencv-4.1.1/build/x64/vc14/lib -lopencv_world411d
  }
}

# For Linux or MacOS
!win32 {
  macx {
    message("Building for MacOS")
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
  } else {
    message("Building for Unix/Linux")
  }
  INCLUDEPATH += /usr/local/include
  INCLUDEPATH += /usr/local/include/opencv4
  LIBS += -L/usr/local/lib
  #LIBS += -L/usr/local/Cellar/ffmpeg/3.3.3/lib
  LIBS += -lopencv_core
  LIBS += -lopencv_imgproc
  LIBS += -lopencv_imgcodecs
  LIBS += -lopencv_video
  LIBS += -lopencv_videoio
  LIBS += -lopencv_highgui
  LIBS += -lopencv_features2d
  #LIBS += -lopencv_ml
  #LIBS += -lopencv_calib3d
  #LIBS += -lopencv_objdetect
  #LIBS += -lopencv_flann
  #LIBS += -lopencv_contrib
  #LIBS += -lopencv_legacy
  #LIBS += -lavcodec
  #LIBS += -lavformat
  #LIBS += -lavutil
  #LIBS += -lavdevice
  #LIBS += -lavfilter
  #LIBS += -lpostproc
  #LIBS += -lswscale
  #LIBS += -lswresample
}

RESOURCES = application.qrc

SOURCES += bgslibrary_gui.cpp\
    mainwindow.cpp \
    qt_utils.cpp \
    texteditor.cpp \
    ../../src/tools/ForegroundMaskAnalysis.cpp \
    ../../src/tools/PerformanceUtils.cpp \
    ../../src/tools/PixelUtils.cpp \
    ../../src/algorithms/dp/AdaptiveMedianBGS.cpp \
    ../../src/algorithms/dp/Eigenbackground.cpp \
    ../../src/algorithms/dp/Error.cpp \
    ../../src/algorithms/dp/GrimsonGMM.cpp \
    ../../src/algorithms/dp/Image.cpp \
    ../../src/algorithms/dp/MeanBGS.cpp \
    ../../src/algorithms/dp/PratiMediodBGS.cpp \
    ../../src/algorithms/dp/TextureBGS.cpp \
    ../../src/algorithms/dp/WrenGA.cpp \
    ../../src/algorithms/dp/ZivkovicAGMM.cpp \
    ../../src/algorithms/IMBS/IMBS.cpp \
    ../../src/algorithms/KDE/KernelTable.cpp \
    ../../src/algorithms/KDE/NPBGmodel.cpp \
    ../../src/algorithms/KDE/NPBGSubtractor.cpp \
    ../../src/algorithms/lb/BGModel.cpp \
    ../../src/algorithms/lb/BGModelFuzzyGauss.cpp \
    ../../src/algorithms/lb/BGModelFuzzySom.cpp \
    ../../src/algorithms/lb/BGModelGauss.cpp \
    ../../src/algorithms/lb/BGModelMog.cpp \
    ../../src/algorithms/lb/BGModelSom.cpp \
    ../../src/algorithms/LBP_MRF/graph.cpp \
    ../../src/algorithms/LBP_MRF/maxflow.cpp \
    ../../src/algorithms/LBP_MRF/MEDefs.cpp \
    ../../src/algorithms/LBP_MRF/MEHistogram.cpp \
    ../../src/algorithms/LBP_MRF/MEImage.cpp \
    ../../src/algorithms/LBP_MRF/MotionDetection.cpp \
    ../../src/algorithms/LBSP/BackgroundSubtractorLBSP.cpp \
    ../../src/algorithms/LBSP/BackgroundSubtractorLBSP_.cpp \
    ../../src/algorithms/LBSP/BackgroundSubtractorLOBSTER.cpp \
    ../../src/algorithms/LBSP/BackgroundSubtractorPAWCS.cpp \
    ../../src/algorithms/LBSP/BackgroundSubtractorSuBSENSE.cpp \
    ../../src/algorithms/LBSP/LBSP.cpp \
    ../../src/algorithms/LBSP/LBSP_.cpp \
    ../../src/algorithms/MultiLayer/blob.cpp \
    ../../src/algorithms/MultiLayer/BlobExtraction.cpp \
    ../../src/algorithms/MultiLayer/BlobResult.cpp \
    ../../src/algorithms/MultiLayer/CMultiLayerBGS.cpp \
    ../../src/algorithms/MultiLayer/LocalBinaryPattern.cpp \
    ../../src/algorithms/PBAS/PBAS.cpp \
    ../../src/algorithms/SigmaDelta/sdLaMa091.cpp \
    ../../src/algorithms/T2F/FuzzyUtils.cpp \
    ../../src/algorithms/T2F/MRF.cpp \
    ../../src/algorithms/T2F/T2FGMM.cpp \
    ../../src/algorithms/T2F/T2FMRF.cpp \
    ../../src/algorithms/TwoPoints/two_points.cpp \
    ../../src/algorithms/ViBe/vibe-background-sequential.cpp \
    ../../src/algorithms/VuMeter/TBackground.cpp \
    ../../src/algorithms/VuMeter/TBackgroundVuMeter.cpp \
    ../../src/algorithms/AdaptiveBackgroundLearning.cpp \
    ../../src/algorithms/AdaptiveSelectiveBackgroundLearning.cpp \
    ../../src/algorithms/DPAdaptiveMedian.cpp \
    ../../src/algorithms/DPEigenbackground.cpp \
    ../../src/algorithms/DPGrimsonGMM.cpp \
    ../../src/algorithms/DPMean.cpp \
    ../../src/algorithms/DPPratiMediod.cpp \
    ../../src/algorithms/DPTexture.cpp \
    ../../src/algorithms/DPWrenGA.cpp \
    ../../src/algorithms/DPZivkovicAGMM.cpp \
    ../../src/algorithms/FrameDifference.cpp \
    ../../src/algorithms/FuzzyChoquetIntegral.cpp \
    ../../src/algorithms/FuzzySugenoIntegral.cpp \
    ../../src/algorithms/GMG.cpp \
    ../../src/algorithms/IndependentMultimodal.cpp \
    ../../src/algorithms/KDE.cpp \
    ../../src/algorithms/KNN.cpp \
    ../../src/algorithms/LBAdaptiveSOM.cpp \
    ../../src/algorithms/LBFuzzyAdaptiveSOM.cpp \
    ../../src/algorithms/LBFuzzyGaussian.cpp \
    ../../src/algorithms/LBMixtureOfGaussians.cpp \
    ../../src/algorithms/LBP_MRF.cpp \
    ../../src/algorithms/LBSimpleGaussian.cpp \
    ../../src/algorithms/LOBSTER.cpp \
    ../../src/algorithms/MixtureOfGaussianV1.cpp \
    ../../src/algorithms/MixtureOfGaussianV2.cpp \
    ../../src/algorithms/MultiCue.cpp \
    ../../src/algorithms/MultiLayer.cpp \
    ../../src/algorithms/PAWCS.cpp \
    ../../src/algorithms/PixelBasedAdaptiveSegmenter.cpp \
    ../../src/algorithms/SigmaDelta.cpp \
    ../../src/algorithms/StaticFrameDifference.cpp \
    ../../src/algorithms/SuBSENSE.cpp \
    ../../src/algorithms/T2FGMM_UM.cpp \
    ../../src/algorithms/T2FGMM_UV.cpp \
    ../../src/algorithms/T2FMRF_UM.cpp \
    ../../src/algorithms/T2FMRF_UV.cpp \
    ../../src/algorithms/TwoPoints.cpp \
    ../../src/algorithms/ViBe.cpp \
    ../../src/algorithms/VuMeter.cpp \
    ../../src/algorithms/WeightedMovingMean.cpp \
    ../../src/algorithms/WeightedMovingVariance.cpp \
    ../../src/algorithms/CodeBook.cpp
    ../../src/algorithms/_template_/MyBGS.cpp \

HEADERS  += mainwindow.h \
    qt_utils.h \
    texteditor.h \
    ../../src/utils/GenericKeys.h \
    ../../src/utils/GenericMacros.h \
    ../../src/utils/ILoadSaveConfig.h \
    ../../src/tools/ForegroundMaskAnalysis.h \
    ../../src/tools/PerformanceUtils.h \
    ../../src/tools/PixelUtils.h \
    ../../src/algorithms/dp/AdaptiveMedianBGS.h \
    ../../src/algorithms/dp/Bgs.h \
    ../../src/algorithms/dp/BgsParams.h \
    ../../src/algorithms/dp/Eigenbackground.h \
    ../../src/algorithms/dp/Error.h \
    ../../src/algorithms/dp/GrimsonGMM.h \
    ../../src/algorithms/dp/Image.h \
    ../../src/algorithms/dp/MeanBGS.h \
    ../../src/algorithms/dp/PratiMediodBGS.h \
    ../../src/algorithms/dp/TextureBGS.h \
    ../../src/algorithms/dp/WrenGA.h \
    ../../src/algorithms/dp/ZivkovicAGMM.h \
    ../../src/algorithms/IMBS/IMBS.hpp \
    ../../src/algorithms/KDE/KernelTable.h \
    ../../src/algorithms/KDE/NPBGmodel.h \
    ../../src/algorithms/KDE/NPBGSubtractor.h \
    ../../src/algorithms/lb/BGModel.h \
    ../../src/algorithms/lb/BGModelFuzzyGauss.h \
    ../../src/algorithms/lb/BGModelFuzzySom.h \
    ../../src/algorithms/lb/BGModelGauss.h \
    ../../src/algorithms/lb/BGModelMog.h \
    ../../src/algorithms/lb/BGModelSom.h \
    ../../src/algorithms/lb/Types.h \
    ../../src/algorithms/LBP_MRF/block.h \
    ../../src/algorithms/LBP_MRF/graph.h \
    ../../src/algorithms/LBP_MRF/MEDefs.hpp \
    ../../src/algorithms/LBP_MRF/MEHistogram.hpp \
    ../../src/algorithms/LBP_MRF/MEImage.hpp \
    ../../src/algorithms/LBP_MRF/MotionDetection.hpp \
    ../../src/algorithms/LBSP/BackgroundSubtractorLBSP.h \
    ../../src/algorithms/LBSP/BackgroundSubtractorLBSP_.h \
    ../../src/algorithms/LBSP/BackgroundSubtractorLOBSTER.h \
    ../../src/algorithms/LBSP/BackgroundSubtractorPAWCS.h \
    ../../src/algorithms/LBSP/BackgroundSubtractorSuBSENSE.h \
    ../../src/algorithms/LBSP/DistanceUtils.h \
    ../../src/algorithms/LBSP/LBSP.h \
    ../../src/algorithms/LBSP/LBSP_.h \
    ../../src/algorithms/LBSP/RandUtils.h \
    ../../src/algorithms/MultiLayer/BackgroundSubtractionAPI.h \
    ../../src/algorithms/MultiLayer/BGS.h \
    ../../src/algorithms/MultiLayer/blob.h \
    ../../src/algorithms/MultiLayer/BlobExtraction.h \
    ../../src/algorithms/MultiLayer/BlobLibraryConfiguration.h \
    ../../src/algorithms/MultiLayer/BlobResult.h \
    ../../src/algorithms/MultiLayer/CMultiLayerBGS.h \
    ../../src/algorithms/MultiLayer/LocalBinaryPattern.h \
    ../../src/algorithms/MultiLayer/OpenCvDataConversion.h \
    ../../src/algorithms/MultiLayer/OpenCvLegacyIncludes.h \
    ../../src/algorithms/PBAS/PBAS.h \
    ../../src/algorithms/SigmaDelta/sdLaMa091.h \
    ../../src/algorithms/T2F/FuzzyUtils.h \
    ../../src/algorithms/T2F/MRF.h \
    ../../src/algorithms/T2F/T2FGMM.h \
    ../../src/algorithms/T2F/T2FMRF.h \
    ../../src/algorithms/TwoPoints/two_points.h \
    ../../src/algorithms/ViBe/vibe-background-sequential.h \
    ../../src/algorithms/VuMeter/TBackground.h \
    ../../src/algorithms/VuMeter/TBackgroundVuMeter.h \
    ../../src/algorithms/AdaptiveBackgroundLearning.h \
    ../../src/algorithms/AdaptiveSelectiveBackgroundLearning.h \
    ../../src/algorithms/bgslibrary.h \
    ../../src/algorithms/DPAdaptiveMedian.h \
    ../../src/algorithms/DPEigenbackground.h \
    ../../src/algorithms/DPGrimsonGMM.h \
    ../../src/algorithms/DPMean.h \
    ../../src/algorithms/DPPratiMediod.h \
    ../../src/algorithms/DPTexture.h \
    ../../src/algorithms/DPWrenGA.h \
    ../../src/algorithms/DPZivkovicAGMM.h \
    ../../src/algorithms/FrameDifference.h \
    ../../src/algorithms/FuzzyChoquetIntegral.h \
    ../../src/algorithms/FuzzySugenoIntegral.h \
    ../../src/algorithms/GMG.h \
    ../../src/algorithms/IBGS.h \
    ../../src/algorithms/IndependentMultimodal.h \
    ../../src/algorithms/KDE.h \
    ../../src/algorithms/KNN.h \
    ../../src/algorithms/LBAdaptiveSOM.h \
    ../../src/algorithms/LBFuzzyAdaptiveSOM.h \
    ../../src/algorithms/LBFuzzyGaussian.h \
    ../../src/algorithms/LBMixtureOfGaussians.h \
    ../../src/algorithms/LBP_MRF.h \
    ../../src/algorithms/LBSimpleGaussian.h \
    ../../src/algorithms/LOBSTER.h \
    ../../src/algorithms/MixtureOfGaussianV1.h \
    ../../src/algorithms/MixtureOfGaussianV2.h \
    ../../src/algorithms/MultiCue.h \
    ../../src/algorithms/MultiLayer.h \
    ../../src/algorithms/PAWCS.h \
    ../../src/algorithms/PixelBasedAdaptiveSegmenter.h \
    ../../src/algorithms/SigmaDelta.h \
    ../../src/algorithms/StaticFrameDifference.h \
    ../../src/algorithms/SuBSENSE.h \
    ../../src/algorithms/T2FGMM_UM.h \
    ../../src/algorithms/T2FGMM_UV.h \
    ../../src/algorithms/T2FMRF_UM.h \
    ../../src/algorithms/T2FMRF_UV.h \
    ../../src/algorithms/TwoPoints.h \
    ../../src/algorithms/ViBe.h \
    ../../src/algorithms/VuMeter.h \
    ../../src/algorithms/WeightedMovingMean.h \
    ../../src/algorithms/WeightedMovingVariance.h \
    ../../src/algorithms/CodeBook.h
    ../../src/algorithms/_template_/MyBGS.h \

FORMS    += mainwindow.ui

DISTFILES += \
    ../../src/algorithms/LBSP/LBSP_16bits_dbcross_1ch.i \
    ../../src/algorithms/LBSP/LBSP_16bits_dbcross_3ch1t.i \
    ../../src/algorithms/LBSP/LBSP_16bits_dbcross_3ch3t.i \
    ../../src/algorithms/LBSP/LBSP_16bits_dbcross_s3ch.i \
    ../../src/algorithms/ViBe/LICENSE
