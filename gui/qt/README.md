## BGSLibrary v2.0.0 with QT GUI 

* Download binaries for Windows (x86/x64):

* * Download [bgslibrary2_qtgui_opencv320_x64.zip](https://github.com/andrewssobral/bgslibrary/releases/download/bgslib_qtgui_2.0.0/bgslibrary2_qtgui_opencv320_x64.zip)

* * More info: https://github.com/andrewssobral/bgslibrary/releases/tag/bgslib_qtgui_2.0.0

![BGSLibrary QT GUI](https://sites.google.com/site/andrewssobral/bgslibrary_qt_gui.png "BGSLibrary QT GUI")

## Building QT GUI from scratch

* Dependencies:

* * OpenCV 2.x or 3.x (Tested with OpenCV 3.2.0)

* * Qt 5 library (Tested with Qt 5.6.2)

### On Windows (with CMAKE and Visual Studio 2015 x64)

1) Click on 'Qt 5.6 64-bit for Desktop (MSVC 2015)'

2) Go to **bgslibrary/gui_qt/build** folder.

3) Set your OpenCV PATH:
```
set OpenCV_DIR=C:\OpenCV3.2.0\build
```

4) Launch CMAKE:
```
cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 14 Win64" ..
```

5) Include OpenCV binaries in the system path:
```
set PATH=%PATH%;%OpenCV_DIR%\x64\vc14\bin
```

6) Open the **bgslibrary_gui.sln** file in your Visual Studio and switch to **'RELEASE'** mode 

7) Click on **'ALL_BUILD'** project and build!

8) Go to **bgslibrary/gui_qt/build/Release** and copy **bgslibrary_gui.exe** to **bgslibrary/**.

9) Run **bgslibrary_gui.exe** and enjoy! ;-)

### On Linux or Mac OS X 

* Installing dependencies on Mac OS X:
```
brew install opencv3 --with-ffmpeg --with-qt5 --HEAD
brew link --overwrite --dry-run opencv3
```

* Step-by-step Instructions
```
git clone --recursive https://github.com/andrewssobral/bgslibrary.git

cd bgslibrary/gui_qt/build
cmake ..
make

cp bgslibrary_gui ../../ 

./bgslibrary_gui 
```


![BGSLibrary QT GUI](https://sites.google.com/site/andrewssobral/bgslibrary2_qt_gui_osx.png "BGSLibrary QT GUI")

### (Video) Build & Execute BGSLibrary QT GUI on MacOSX using CMake

<p align="center"><a href=https://youtu.be/vl0c-mXWQEo>https://youtu.be/vl0c-mXWQEo</a></p>
<p align="center">
<a href="https://youtu.be/vl0c-mXWQEo" target="_blank">
<img src="https://sites.google.com/site/andrewssobral/bgslib_qtgui_macosx.png?width=600" border="0" />
</a>
</p>




* Possible issues:

If you have something like this:
```
CMake Warning at CMakeLists.txt:33 (find_package):
  By not providing "FindQt5Widgets.cmake" in CMAKE_MODULE_PATH this project
  has asked CMake to find a package configuration file provided by
  "Qt5Widgets", but CMake did not find one.

  Could not find a package configuration file provided by "Qt5Widgets" with
  any of the following names:

    Qt5WidgetsConfig.cmake
    qt5widgets-config.cmake

  Add the installation prefix of "Qt5Widgets" to CMAKE_PREFIX_PATH or set
  "Qt5Widgets_DIR" to a directory containing one of the above files.  If
  "Qt5Widgets" provides a separate development package or SDK, be sure it has
  been installed.


CMake Error at CMakeLists.txt:44 (QT5_ADD_RESOURCES):
  Unknown CMake command "QT5_ADD_RESOURCES".


-- Configuring incomplete, errors occurred!
```

You just need to specify the PATH of your QT installation, for example:

On Linux:
`cmake .. -DCMAKE_PREFIX_PATH=$HOME/Qt/5.12.1/gcc_64`

On MacOS:
`cmake .. -DCMAKE_PREFIX_PATH=$HOME/Qt/5.13.0/clang_64`
