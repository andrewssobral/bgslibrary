---------------------------------------
- BUILDING BGSLIBRARY MATLAB WRAPPER -
---------------------------------------
Tested on MATLAB R2015b and R2016b

* Dependencies:
* * Computer Vision System Toolbox OpenCV Interface
https://fr.mathworks.com/matlabcentral/fileexchange/47953-computer-vision-system-toolbox-opencv-interface
* * * It provides wrapper files for OpenCV 2.4.9

* Compatible compilers:
    Windows 32 bit: MS Visual Studio 2012
    Windows 64 bit: MS Visual Studio 2012
    Linux 64 bit: gcc-4.7.2 (g++)
    Mac 64 bit: Xcode 6.2.0 (Clang++)

* * Note: It works successfully with MS Visual Studio 2013

* * For Xcode 7.3.1 and MATLAB R2015b please see:
https://fr.mathworks.com/matlabcentral/answers/246507-why-can-t-mex-find-a-supported-compiler-in-matlab-r2015b-after-i-upgraded-to-xcode-7-0#answer_194526

* First install [Computer Vision System Toolbox OpenCV Interface]
* * Go to: bgslibrary/wrapper_matlab
* * Double-click on [opencvinterface.mlpkginstall] inside your MATLAB.
-- wait installation, it takes a few minutes ---

* Run: compile.m

* Run demo: demo.m

* See [run_demo.m] for more info
