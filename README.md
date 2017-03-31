## BGSLibrary
A Background Subtraction Library

[![Release](https://img.shields.io/badge/Release-v2.0.0-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0) [![Platform: Windows, Linux, OS X](https://img.shields.io/badge/Platform-Windows%2C%20Linux%2C%20OS%20X-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![OpenCV](https://img.shields.io/badge/OpenCV-2.x%2C%203.x-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![Wrapper: Python, MATLAB](https://img.shields.io/badge/Wrapper-Java%2C%20Python%2C%20MATLAB-orange.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![Algorithms](https://img.shields.io/badge/Algorithms-43-red.svg)](https://github.com/andrewssobral/bgslibrary/wiki/List-of-available-algorithms)

[![bgslibrary](http://i.giphy.com/5A94AZahSIVOw.gif)](https://youtu.be/_UbERwuQ0OU)

Page Update: **01/04/2017**

Library Version: **2.0.0** (see **[Build Status](https://github.com/andrewssobral/bgslibrary/wiki/Build-status)** and **[Release Notes](https://github.com/andrewssobral/bgslibrary/wiki/Release-notes)** for more info)

The **BGSLibrary** was developed by [Andrews Sobral](http://andrewssobral.wixsite.com/home) and provides an easy-to-use C++ framework based on [OpenCV](http://www.opencv.org/) to perform foreground-background separation in videos. The bgslibrary is compatible with OpenCV 2.x and 3.x, and compiles under Windows, Linux, and Mac OS X. Currently the library contains **43** algorithms. The source code is available under [GNU GPLv3 license](https://www.gnu.org/licenses/gpl-3.0.en.html), the library is free and open source for academic purposes.

* [List of available algorithms](https://github.com/andrewssobral/bgslibrary/wiki/List-of-available-algorithms)
* [Algorithms benchmark](https://github.com/andrewssobral/bgslibrary/wiki/Algorithms-benchmark)
* [Which algorithms really matter?](https://github.com/andrewssobral/bgslibrary/wiki/Which-algorithms-really-matter%3F)
* [Library architecture](https://github.com/andrewssobral/bgslibrary/wiki/Library-architecture)

* Installation instructions

* * [Windows installation](https://github.com/andrewssobral/bgslibrary/wiki/Installation-instructions---Windows)

* * [Ubuntu / OS X installation](https://github.com/andrewssobral/bgslibrary/wiki/Installation-instructions-Ubuntu-or-OSX)

* Graphical User Interface:

*  * [C++ QT](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-QT) ***(NEW)***
*  * [C++ MFC](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-MFC)
*  * [Java](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-Java)

* Wrappers:

*  * [Python](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-Python) ***(NEW)***
*  * [MATLAB](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-MATLAB) ***(NEW)***
*  * [Java](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-Java) ***(NEW)***

* [Docker images](https://github.com/andrewssobral/bgslibrary/wiki/Docker-images)
* [How to integrate BGSLibrary in your own CPP code](https://github.com/andrewssobral/bgslibrary/wiki/How-to-integrate-BGSLibrary-in-your-own-CPP-code)
* [How to contribute](https://github.com/andrewssobral/bgslibrary/wiki/How-to-contribute)
* [List of collaborators](https://github.com/andrewssobral/bgslibrary/wiki/List-of-collaborators)
* [Release notes](https://github.com/andrewssobral/bgslibrary/wiki/Release-notes)


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
<img src="https://sites.google.com/site/andrewssobral/bgslibrary_qt_gui_video.png?width=600" border="0" />
</a>
</p>

<p align="center">
<a href="https://www.youtube.com/watch?v=Ccqa9KBO9_U" target="_blank">
<img src="https://sites.google.com/site/andrewssobral/bgslibrary_youtube.png?width=600" border="0" />
</a>
</p>
