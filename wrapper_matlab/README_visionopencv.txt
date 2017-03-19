CONTENTS OF THIS FILE
---------------------
* Introduction
* Requirements
* Installation
* Contents
* Utility functions
* How to compile OpenCV mex function
* Example


INTRODUCTION
------------
"Computer Vision System Toolbox OpenCV Interface" is used to create mex files 
that link against OpenCV. The support package also contains graphics 
processing unit (GPU) support. 

REQUIREMENTS
------------
This package requires the following:
* Computer Vision System Toolbox® Version R2016a installation
* A compatible C++ compiler

The mex function uses pre-built OpenCV libraries which are shipped with the 
Computer Vision System Toolbox. Your compiler must be compatible with the 
pre-built OpenCV libraries. The following is a list of compatible compilers:
    Windows 32 bit: MS Visual Studio 2012
    Windows 64 bit: MS Visual Studio 2012
    Linux 64 bit: gcc-4.7.2 (g++)
    Mac 64 bit: Xcode 6.2.0 (Clang++)

INSTALLATION
------------
Use the support package installer which can be invoked using the  
visionSupportPackages function.
After the support package is installed, the location of the package can be 
found by executing the following MATLAB command:
>> fileparts(which('mexOpenCV.m'))

CONTENTS
--------
In addition to the files and folders required by the support package installer, 
the package contains the following folder:

example: Six subfolders containing examples. Each subfolder contains source 
	 file that calls the OpenCV function and the test script to test the 
	 generated mex file.

UTILITY FUNCTIONS
-----------------
The support package uses a set of utility functions to marshall data 
between OpenCV and MATLAB. It supports only CPP-linkage. GPU support is 
only available on glnxa64 and win64 platforms. The utility functions support 
CUDA-enabled NVIDIA GPU with compute capability 2.0 or higher. 

General purpose utility functions are available at:
  (matlabroot)/extern/include/opencvmex.hpp
Utility functions for GPU support are available at:
  (matlabroot)/extern/include/opencvgpumex.hpp

HOW TO COMPILE OPENCV MEX FUNCTION
----------------------------------
Follow these steps:
1. Change your current working folder to the folder where the source file is located. 
2. Call the mexOpenCV function with the source file. 
>> mexOpenCV yourfile.cpp

To get more information, type the following at the MATLAB command prompt:
>> help mexOpenCV

EXAMPLES
--------
There are three examples included in the package: 
* Template Matching (includes regular and GPU versions)
* Image registration using ORB detector and descriptor (includes regular, 
  GPU and code-generation versions)
* Foreground Detection

To run them, follow the steps in the README.txt file located in the corresponding 
sub-folders of the examples folder.

