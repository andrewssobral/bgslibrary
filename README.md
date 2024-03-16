## BGSLibrary
A Background Subtraction Library

[![Release](https://img.shields.io/badge/Release-3.3.0-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![License: GPL v3](https://img.shields.io/badge/License-MIT-blue.svg)](http://www.gnu.org/licenses/gpl-3.0) [![Platform: Windows, Linux, OS X](https://img.shields.io/badge/Platform-Windows%2C%20Linux%2C%20OS%20X-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![OpenCV](https://img.shields.io/badge/OpenCV-2.4.x%2C%203.x%2C%204.x-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![Wrapper: Python, MATLAB](https://img.shields.io/badge/Wrapper-Java%2C%20Python%2C%20MATLAB-orange.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![Algorithms](https://img.shields.io/badge/Algorithms-43-red.svg)](https://github.com/andrewssobral/bgslibrary/wiki/List-of-available-algorithms)

<p align="center">
<a href="https://youtu.be/_UbERwuQ0OU" target="_blank">
<img src="https://raw.githubusercontent.com/andrewssobral/bgslibrary/master/docs/images/bgs_giphy2.gif" border="0" />
</a>
</p>

Last page update: **04/03/2023**

Library Version: **3.3.0** (see **[Build Status](https://github.com/andrewssobral/bgslibrary/wiki/Build-status)** and **[Release Notes](https://github.com/andrewssobral/bgslibrary/wiki/Release-notes)** for more info)

The **BGSLibrary** was developed in early 2012 by [Andrews Sobral](http://andrewssobral.wixsite.com/home) as a C++ framework (with wrappers available for Python, Java and MATLAB) for foreground-background separation in videos using [OpenCV](http://www.opencv.org/). The bgslibrary is compatible with OpenCV versions 2.4.x, 3.x and 4.x, and can be compiled on Windows, Linux, and Mac OS X. It currently contains **43** algorithms and is available free of charge to all users, both academic and commercial. The library's source code is available under the [MIT license](https://opensource.org/licenses/MIT).

* [List of available algorithms](https://github.com/andrewssobral/bgslibrary/wiki/List-of-available-algorithms)
* [Algorithms benchmark](https://github.com/andrewssobral/bgslibrary/wiki/Algorithms-benchmark)
* [Which algorithms really matter?](https://github.com/andrewssobral/bgslibrary/wiki/Which-algorithms-really-matter%3F)
* [Library architecture](https://github.com/andrewssobral/bgslibrary/wiki/Library-architecture)

* Installation instructions

You can either install BGSLibrary via [pre-built binary package](https://github.com/andrewssobral/bgslibrary/releases) or build it from source

* * [Windows installation](https://github.com/andrewssobral/bgslibrary/wiki/Installation-instructions---Windows)

* * [Ubuntu / OS X installation](https://github.com/andrewssobral/bgslibrary/wiki/Installation-instructions-Ubuntu-or-OSX)

Supported Compilers are:

    GCC 4.8 and above
    Clang 3.4 and above
    MSVC 2015, 2017, 2019 or newer

Other compilers might work, but are not officially supported.
The bgslibrary requires some features from the ISO C++ 2014 standard.

* Graphical User Interface

*  * [C++ QT](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-QT) ***(Official)***
*  * [C++ MFC](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-MFC) ***(Deprecated)***
*  * [Java](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-Java) ***(Obsolete)***

* Wrappers

*  * [Python](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-Python)
*  * [MATLAB](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-MATLAB)
*  * [Java](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-Java)


* Usage examples

* * BGSlibrary examples folder: https://github.com/andrewssobral/bgslibrary/tree/master/examples

* * BGSlibrary examples in C++: https://github.com/andrewssobral/bgslibrary-examples-cpp

* * BGSlibrary examples in Python: https://github.com/andrewssobral/bgslibrary-examples-python

* [Docker images](https://github.com/andrewssobral/bgslibrary/wiki/Docker-images)
* [How to integrate BGSLibrary in your own CPP code](https://github.com/andrewssobral/bgslibrary/wiki/How-to-integrate-BGSLibrary-in-your-own-CPP-code)
* [How to contribute](https://github.com/andrewssobral/bgslibrary/wiki/How-to-contribute)
* [List of collaborators](https://github.com/andrewssobral/bgslibrary/wiki/List-of-collaborators)
* [Release notes](https://github.com/andrewssobral/bgslibrary/wiki/Release-notes)


## Algorithm compatibility across OpenCV versions
---------------------------------------------------
| Algorithm | OpenCV < 3.0 (42) | 3.0 <= OpenCV <= 3.4.7 (41) | 3.4.7 < OpenCV < 4.0 (39) | OpenCV >= 4.0 (26) |
|--------------------------------|:-----------:|:----------------------:|:---------------------:|:------------:|
| AdaptiveBackgroundLearning | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| AdaptiveSelectiveBackgroundLearning | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| CodeBook | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| DPAdaptiveMedian | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| DPEigenbackground | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| DPGrimsonGMM | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| DPMean | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| DPPratiMediod | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| DPTexture | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| DPWrenGA | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| DPZivkovicAGMM | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| FrameDifference | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| FuzzyChoquetIntegral | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| FuzzySugenoIntegral | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| GMG | :heavy_check_mark: | :x: | :x: | :x: |
| IndependentMultimodal | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| KDE | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| KNN | :x: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| LBAdaptiveSOM | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| LBFuzzyAdaptiveSOM | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| LBFuzzyGaussian | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| LBMixtureOfGaussians | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| LBP_MRF | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| LBSimpleGaussian | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| LOBSTER | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| MixtureOfGaussianV1 | :heavy_check_mark: | :x: | :x: | :x: |
| MixtureOfGaussianV2 | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| MultiCue | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| MultiLayer | :heavy_check_mark: | :heavy_check_mark: | :x: | :x: |
| PAWCS | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| PixelBasedAdaptiveSegmenter | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| SigmaDelta | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| StaticFrameDifference | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| SuBSENSE | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| T2FGMM_UM | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| T2FGMM_UV | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| T2FMRF_UM | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| T2FMRF_UV | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :x: |
| TwoPoints | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| ViBe | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| VuMeter | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| WeightedMovingMean | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |
| WeightedMovingVariance | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: | :heavy_check_mark: |



## Stargazers over time

[![Stargazers over time](https://starchart.cc/andrewssobral/bgslibrary.svg)](https://starchart.cc/andrewssobral/bgslibrary)


Citation
--------

If you use this library for your publications, please cite it as:
```
@inproceedings{bgslibrary,
author    = {Sobral, Andrews},
title     = {{BGSLibrary}: An OpenCV C++ Background Subtraction Library},
booktitle = {IX Workshop de Visão Computacional (WVC'2013)},
address   = {Rio de Janeiro, Brazil},
year      = {2013},
month     = {Jun},
url       = {https://github.com/andrewssobral/bgslibrary}
}
```
A chapter about the BGSLibrary has been published in the handbook on [Background Modeling and Foreground Detection for Video Surveillance](https://sites.google.com/site/backgroundmodeling/).
```
@incollection{bgslibrarychapter,
author    = {Sobral, Andrews and Bouwmans, Thierry},
title     = {BGS Library: A Library Framework for Algorithm’s Evaluation in Foreground/Background Segmentation},
booktitle = {Background Modeling and Foreground Detection for Video Surveillance},
publisher = {CRC Press, Taylor and Francis Group.}
year      = {2014},
}
```


Download PDF:
* Sobral, Andrews. BGSLibrary: An OpenCV C++ Background Subtraction Library. IX Workshop de Visão Computacional (WVC'2013), Rio de Janeiro, Brazil, Jun. 2013. ([PDF](http://www.researchgate.net/publication/257424214_BGSLibrary_An_OpenCV_C_Background_Subtraction_Library) in brazilian-portuguese containing an english abstract).

* Sobral, Andrews; Bouwmans, Thierry. "BGS Library: A Library Framework for Algorithm’s Evaluation in Foreground/Background Segmentation". Chapter on the handbook "Background Modeling and Foreground Detection for Video Surveillance", CRC Press, Taylor and Francis Group, 2014. ([PDF](http://www.researchgate.net/publication/257424214_BGSLibrary_An_OpenCV_C_Background_Subtraction_Library) in english).


Some references
---------------

Some algorithms of the BGSLibrary were used successfully in the following papers: 

* (2014) Sobral, Andrews; Vacavant, Antoine. A comprehensive review of background subtraction algorithms evaluated with synthetic and real videos. Computer Vision and Image Understanding (CVIU), 2014. ([Online](http://dx.doi.org/10.1016/j.cviu.2013.12.005)) ([PDF](http://www.researchgate.net/publication/259340906_A_comprehensive_review_of_background_subtraction_algorithms_evaluated_with_synthetic_and_real_videos))

* (2013) Sobral, Andrews; Oliveira, Luciano; Schnitman, Leizer; Souza, Felippe. (**Best Paper Award**) Highway Traffic Congestion Classification Using Holistic Properties. In International Conference on Signal Processing, Pattern Recognition and Applications (SPPRA'2013), Innsbruck, Austria, Feb 2013. ([Online](http://dx.doi.org/10.2316/P.2013.798-105)) ([PDF](http://www.researchgate.net/publication/233427564_HIGHWAY_TRAFFIC_CONGESTION_CLASSIFICATION_USING_HOLISTIC_PROPERTIES))


Videos
------

<p align="center">
<a href="https://www.youtube.com/watch?v=_UbERwuQ0OU" target="_blank">
<img src="https://raw.githubusercontent.com/andrewssobral/bgslibrary/master/docs/images/bgslibrary_qt_gui_video.png" width="600" border="0" />
</a>
</p>

<p align="center">
<a href="https://www.youtube.com/watch?v=Ccqa9KBO9_U" target="_blank">
<img src="https://raw.githubusercontent.com/andrewssobral/bgslibrary/master/docs/images/bgslibrary_youtube.png" width="600" border="0" />
</a>
</p>
