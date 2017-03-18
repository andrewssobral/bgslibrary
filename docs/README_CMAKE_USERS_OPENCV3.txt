-------------------------------------------------
-------------- WINDOWS CMAKE USERS --------------

How to build BGSLibrary with OpenCV 3.2.0 and Visual Studio 2015 from CMAKE.

For Linux users, please see the instruction in README_LINUX_USERS.txt file.

Dependencies:
* GIT (tested with git version 2.7.2.windows.1).
* CMAKE for Windows (tested with cmake version 3.1.1).
* Microsoft Visual Studio (tested with VS2015).

Please follow the instructions below:

1) Go to Windows console.

2) Clone BGSLibrary git repository:
git clone https://github.com/andrewssobral/bgslibrary.git

3) Go to bgslibrary/build folder.

4) Set your OpenCV PATH:
setlocal
set OpenCV_DIR=C:\OpenCV3.2.0\build

5) Launch CMAKE:
cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 14 Win64" ..

Now, you will see something like:
-------------------------------------------------
-- The C compiler identification is MSVC 19.0.24215.1
-- The CXX compiler identification is MSVC 19.0.24215.1
-- Check for working C compiler using: Visual Studio 14 2015 Win64
-- Check for working C compiler using: Visual Studio 14 2015 Win64 -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler using: Visual Studio 14 2015 Win64
-- Check for working CXX compiler using: Visual Studio 14 2015 Win64 -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- OpenCV ARCH: x64
-- OpenCV RUNTIME: vc14
-- OpenCV STATIC: ON
-- Found OpenCV 3.2.0 in C:/OpenCV3.2.0/build/x64/vc14/lib
-- You might need to add C:\OpenCV3.2.0\build\x64\vc14\bin to your PATH to be able to run your applications.
-- OpenCV library status:
--     version: 3.2.0
--     libraries: opencv_world;opencv_videostab;opencv_videoio;opencv_video;opencv_superres;opencv_stitching;opencv_shape;opencv_photo;opencv_objdetect;opencv_ml;opencv_imgproc;opencv_imgcodecs;opencv_highgui;opencv_flann;opencv_features2d;opencv_core;opencv_calib3d
--     include path: C:/OpenCV3.2.0/build/include;C:/OpenCV3.2.0/build/include/opencv
-- Configuring done
-- Generating done
-- Build files have been written to: C:/bgslibrary/build
-------------------------------------------------

6) Include OpenCV binaries in the system path:
set PATH=%PATH%;%OpenCV_DIR%\x64\vc14\bin

7) Open 'bgs.sln' in Visual Studio and switch to 'RELEASE' mode 
7.1) Note if you are using a Visual Studio version superior than 2015, you will need to CANCEL the project wizard update. However, you can go to step (2) and change the Visual Studio version, e.g.: -G "Visual Studio XX", where XX is your Visual Studio version.

8) Click on 'ALL_BUILD' project and build!

9) If everything goes well, you can run bgslibrary in the Windows console as follows:

9.1) Running BGSLibrary with a webcamera:
C:\bgslibrary> build\bgslibrary.exe --use_cam --camera=0

9.2) Running demo code:
C:\bgslibrary> build\bgs_demo.exe dataset/video.avi

9.3) Running demo2 code:
C:\bgslibrary> build\bgs_demo2.exe

Additional information:
* Note that bgslibrary requires a 'config' folder in the working directory.
