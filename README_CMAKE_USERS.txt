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
e.g.: git clone https://github.com/andrewssobral/bgslibrary.git

3) Go to bgslibrary/build folder.
e.g.: C:\bgslibrary\build>_

2) Set your OpenCV PATH:
e.g.:
\> setlocal
\> set OpenCV_DIR=C:\OpenCV2.4.10\build
\> cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 12" ..
or:
\> cmake -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 12 Win64" ..

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

3) Include OpenCV binaries in the system path:
\> set PATH=%PATH%;%OpenCV_DIR%\x86\vc12\bin
or:
\> set PATH=%PATH%;%OpenCV_DIR%\x64\vc12\bin

4) Open 'bgs.sln' in Visual Studio and switch to 'RELEASE' mode 
4.1) Note if you are using a Visual Studio version superior than 2013, you will need to CANCEL the project wizard update. However, you can go to step (2) and change the Visual Studio version, e.g.: -G "Visual Studio XX", where XX is your Visual Studio version.

5) Click on 'ALL_BUILD' project and build!

6) If everything goes well, you can run bgslibrary in the Windows console as follows:

6.1) Running BGSLibrary with a webcamera:
C:\bgslibrary> build\bgslibrary.exe --use_cam --camera=0

6.2) Running demo code:
C:\bgslibrary> build\bgs_demo.exe dataset/video.avi

6.3) Running demo2 code:
C:\bgslibrary> build\bgs_demo2.exe

Additional information:
* Note that bgslibrary requires a 'config' folder in the working directory.
e.g.: C:\bgslibrary\config
