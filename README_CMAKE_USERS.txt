-------------------------------------------------
-------------- WINDOWS CMAKE USERS --------------

How to build BGSLibrary with OpenCV 3.1.0 and Visual Studio 2013 from CMAKE.

For Linux users, please see the instruction in README_LINUX_USERS.txt file.

Dependencies:
* GIT (tested with git version 2.7.2.windows.1).
* CMAKE for Windows (tested with cmake version 3.1.1).
* Microsoft Visual Studio (tested with VS2013 and VS2015).
* OpenCV 3 (tested with opencv 3.1.0)

Please follow the instructions below:

1) Go to Windows console.

2) Clone BGSLibrary git repository:
e.g.: git clone https://github.com/andrewssobral/bgslibrary.git -b opencv3

3) Go to bgslibrary/build folder.
e.g.: C:\bgslibrary\build>_

2) Set your OpenCV PATH:
e.g.:
\> setlocal
\> set OpenCV_DIR=C:\OpenCV3.1.0\build
\> cmake -DDBUILD_WITH_STATIC_CRT=OFF -DOpenCV_DIR=%OpenCV_DIR% -G "Visual Studio 12 2013 Win64" ..

Now, you will see something like:
------------------------------------------------------------------------
-- The C compiler identification is MSVC 18.0.31101.0
-- The CXX compiler identification is MSVC 18.0.31101.0
-- Check for working C compiler using: Visual Studio 12 2013 Win64
-- Check for working C compiler using: Visual Studio 12 2013 Win64 -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working CXX compiler using: Visual Studio 12 2013 Win64
-- Check for working CXX compiler using: Visual Studio 12 2013 Win64 -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- OpenCV ARCH: x64
-- OpenCV RUNTIME: vc12
-- OpenCV STATIC: OFF
-- Found OpenCV 3.1.0 in C:/OpenCV3.1.0/build/x64/vc12/lib
-- You might need to add C:\OpenCV3.1.0\build\x64\vc12\bin to your PATH to be able to run your applications.
-- Configuring done
-- Generating done
CMake Warning:
  Manually-specified variables were not used by the project:

    DBUILD_WITH_STATIC_CRT


-- Build files have been written to: C:/bgslibrary/build
------------------------------------------------------------------------

3) Include OpenCV binaries in the system path:
\> set PATH=%PATH%;%OpenCV_DIR%\x64\vc12\bin

4) Open 'bgs.sln' in Visual Studio and switch to 'RELEASE' mode.

5) Click on 'bgs' project, and set:
[Configuration Type] Static library (.lib)
[Target Extension] .lib

6) Click on 'ALL_BUILD' project and build!

7) If everything goes well, you can run bgslibrary in the Windows console as follows:

7.1) Running BGSLibrary with a webcamera:
C:\bgslibrary> build\Release\bgs.exe --use_cam --camera=0

7.2) Running demo code:
C:\bgslibrary> build\Release\bgs_demo.exe dataset/video.avi

7.3) Running demo2 code:
C:\bgslibrary> build\Release\bgs_demo2.exe

Additional information:
* Note that bgslibrary requires a 'config' folder in the working directory.
e.g.: C:\bgslibrary\config
