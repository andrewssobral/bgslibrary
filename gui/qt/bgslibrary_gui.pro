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

# https://doc.qt.io/qt-5/qmake-variable-reference.html#config
CONFIG += c++14
#CONFIG += no_keywords # Python redefines some qt keywords
#CONFIG += console
CONFIG -= app_bundle
CONFIG += sdk_no_version_check # supress OS warning for 10.14
#CONFIG += staticlib

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
unix {
  macx {
    message("Building for MacOS")
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14
  } else {
    message("Building for Linux")
    #CONFIG += link_pkgconfig
    #PKGCONFIG += opencv
  }
  #pkg-config --cflags --libs opencv
  INCLUDEPATH += /usr/local/include
  #INCLUDEPATH += /usr/local/include/opencv
  INCLUDEPATH += /usr/local/include/opencv2
  #INCLUDEPATH += /usr/local/include/opencv4
  LIBS += -L/usr/local/lib
  #LIBS += -L/usr/local/Cellar/ffmpeg/3.3.3/lib
  LIBS += -lopencv_core
  LIBS += -lopencv_imgproc
  LIBS += -lopencv_imgcodecs
  LIBS += -lopencv_video
  LIBS += -lopencv_videoio
  LIBS += -lopencv_highgui
  LIBS += -lopencv_features2d
}

RESOURCES = application.qrc

SOURCES += bgslibrary_gui.cpp\
    mainwindow.cpp \
    qt_utils.cpp \
    texteditor.cpp
SOURCES += $$files("../../bgslibrary/algorithms/*.cpp", true)
SOURCES += $$files("../../bgslibrary/tools/*.cpp", true)
SOURCES += $$files("../../bgslibrary/utils/*.cpp", true)

HEADERS  += mainwindow.h \
    qt_utils.h \
    texteditor.h
HEADERS += $$files("../../bgslibrary/algorithms/*.h", true)
HEADERS += $$files("../../bgslibrary/tools/*.h", true)
HEADERS += $$files("../../bgslibrary/utils/*.h", true)

FORMS    += mainwindow.ui

DISTFILES += \
    ../../src/algorithms/LBSP/LBSP_16bits_dbcross_1ch.i \
    ../../src/algorithms/LBSP/LBSP_16bits_dbcross_3ch1t.i \
    ../../src/algorithms/LBSP/LBSP_16bits_dbcross_3ch3t.i \
    ../../src/algorithms/LBSP/LBSP_16bits_dbcross_s3ch.i \
    ../../src/algorithms/ViBe/LICENSE
