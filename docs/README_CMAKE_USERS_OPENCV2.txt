-------------------------------------------------
-------------- WINDOWS CMAKE USERS --------------

How to build BGSLibrary with OpenCV 2.4.10 and Visual Studio 2013 from CMAKE.

For Linux users, please see the instruction in README_LINUX_USERS.txt file.

Dependencies:
* GIT (tested with git version 2.7.2.windows.1).
* CMAKE for Windows (tested with cmake version 3.1.1).
* Microsoft Visual Studio (tested with VS2013).

Please follow the instructions below:

1) Go to Windows console.

2) Clone BGSLibrary git repository:
git clone https://github.com/andrewssobral/bgslibrary.git

3) Go to bgslibrary/build folder.

4) Set your OpenCV PATH:
setlocal
set OpenCV_DIR=C:\OpenCV2.4.10\build

5) Launch CMAKE:
(For Windows x86 32bits) cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 12" ..
(For Windows x64 64bits) cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 12 Win64" ..

Now, you will see something like (for win64):
-------------------------------------------------
-- The C compiler identification is MSVC 18.0.40629.0
-- The CXX compiler identification is MSVC 18.0.40629.0
-- Check for working C compiler using: Visual Studio 12 2013 Win64
-- Check for working C compiler using: Visual Studio 12 2013 Win64 -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler using: Visual Studio 12 2013 Win64
-- Check for working CXX compiler using: Visual Studio 12 2013 Win64 -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- OpenCV ARCH: x64
-- OpenCV RUNTIME: vc12
-- OpenCV STATIC: OFF
-- Found OpenCV 2.4.10 in C:/OpenCV2.4.10/build/x64/vc12/lib
-- You might need to add C:\OpenCV2.4.10\build\x64\vc12\bin to your PATH to be able to run your applications.
-- Configuring done
-- Generating done
-- Build files have been written to: C:/bgslibrary/build
-------------------------------------------------

6) Include OpenCV binaries in the system path:
(For Windows x86 32bits) set PATH=%PATH%;%OpenCV_DIR%\x86\vc12\bin
(For Windows x64 64bits) set PATH=%PATH%;%OpenCV_DIR%\x64\vc12\bin

7) Open 'bgs.sln' in Visual Studio and switch to 'RELEASE' mode 
7.1) Note if you are using a Visual Studio version superior than 2013, you will need to CANCEL the project wizard update. However, you can go to step (5) and change the Visual Studio version, e.g.: -G "Visual Studio XX", where XX is your Visual Studio version.

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
