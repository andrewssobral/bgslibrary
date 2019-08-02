### For Windows users

* Running CMAKE with Visual Studio 2015 and OpenCV 3.2.0 (x64)

* * Working directory: **bgslibrary\wrapper_java**

```
:: Set OpenCV env
echo "Setting up OpenCV env"
set OpenCV_DIR=C:\OpenCV3.2.0\build
set PATH=%PATH%;%OpenCV_DIR%\x64\vc14\bin

:: Run CMake
echo "Running CMake"
rmdir /S /Q build_cmake
if not exist "build_cmake" mkdir build_cmake
cd build_cmake
cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 14 Win64" ..
cd ..
```

* Open **libbgs_java_module.sln** in your Visual Studio IDE and switch to **RELEASE** mode

* Click on **ALL_BUILD** and build

* Copy **libbgs_java_module.dll** to **bgslibrary\wrapper_java**

* Compile Java source

```
:: Compile
echo "Compiling"
javac -cp .;src/;libs/opencv-320.jar src/bgslibrary/Main.java
```

* Run

```
:: Run
echo "Running Main class"
java -cp .;src/;libs/opencv-320.jar -Djava.library.path=. bgslibrary.Main
```

[![bgslibrary](https://sites.google.com/site/andrewssobral/bgslib_java_wrapper_screen.png)]()


#### Generating JNI files (optional)

```
:: Generate JNI
cd src
echo "Generating JNI"
javah -cp .;../libs/opencv-320.jar bgslibrary.Main
cd ..
```
