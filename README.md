bgslibrary
==========

A Background Subtraction Library

The BGSLibrary was developed by Andrews Sobral (http://about.me/andrewssobral/) and provides a C++ framework to perform background subtraction (BGS). The code works either on Windows or on Linux. Currently the library offers 35 BGS algorithms. A large amount of algorithms were provided by several authors. The source code is available under GNU GPL v3 license, the library is free and open source. Any user can be download latest project source code using SVN client. In Windows, a demo project for Visual Studio 2010 is provided. An executable version of BGSLibrary is available for Windows 32 bits and 64 bits. For Linux users, a Makefile can be used to compile all files and generate an executable example. Table 1 lists some of algorithms available in BGSLibrary. The algorithms are classified by their similarities.

Citation:
If you use this library for your publications, please cite it as:
@inproceedings{bgslibrary,
author = {Sobral, Andrews},
title = {{BGSLibrary}: An OpenCV C++ Background Subtraction Library},
booktitle = {IX Workshop de Visão Computacional (WVC'2013)},
address = {Rio de Janeiro, Brazil},
year = {2013},
month = {Jun}
} 

Some algorithms of the BGSLibrary was used successfully in the following papers: 
* (2014) Sobral, Andrews; Vacavant, Antoine. A comprehensive review of background subtraction algorithms evaluated with synthetic and real videos. Computer Vision and Image Understanding (CVIU), 2014.
* (2013) Sobral, Andrews; Oliveira, Luciano; Schnitman, Leizer; Souza, Felippe. (Best Paper Award) Highway Traffic Congestion Classification Using Holistic Properties. In International Conference on Signal Processing, Pattern Recognition and Applications (SPPRA'2013), Innsbruck, Austria, Feb 2013.

List of the algorithms available in BGSLibrary
----------------------------------------------
* Basic methods, mean and variance over time:
* Static Frame Difference
* Frame Difference
* Weighted Moving Mean
* Weighted Moving Variance
* Adaptive Background Learning
* Adaptive-Selective Background Learning
*1 Temporal Mean
*1 Adaptive Median of McFarlane and Schofield (1995)
*1 Temporal Median of Cucchiara et al (2003) and Calderara et al (2006)

Fuzzy based methods:
*2 Fuzzy Sugeno Integral (with Adaptive-Selective Update) of Hongxun Zhang and De Xu (2006)
*2 Fuzzy Choquet Integral (with Adaptive-Selective Update) of Baf et al (2008)
*3 Fuzzy Gaussian of Laurence Bender (adapted version of Wren (1997) with Sigari et al (2008) approach)

Statistical methods using one gaussian:
*1 Gaussian Average of Wren (1997)
*3 Simple Gaussian of Benezeth et al (2008)

Statistical methods using multiple gaussians:
*1 Gaussian Mixture Model of Stauffer and Grimson (1999)
*0 Gaussian Mixture Model of KadewTraKuPong and Bowden (2001)
*0 Gaussian Mixture Model of Zivkovic (2004)
*1 Gaussian Mixture Model of Zivkovic (2004)
*3 Gaussian Mixture Model of Laurence Bender (implements the classic GMM with Mahalanobis distance)

Type-2 Fuzzy based methods:
*2 Type-2 Fuzzy GMM-UM of Baf et al (2008)
*2 Type-2 Fuzzy GMM-UV of Baf et al (2008)
*2 Type-2 Fuzzy GMM-UM with MRF of Zhao et al (2012)
*2 Type-2 Fuzzy GMM-UV with MRF of Zhao et al (2012)

Statistical methods using color and texture features:
*1 Texture BGS of Heikkila et al. (2006)
*8 Texture-Based Foreground Detection with MRF of Csaba Kertész (2011)
*4 Multi-Layer BGS of Jian Yao and Jean-Marc Odobez (2007)
*10 MultiCue BGS of SeungJong Noh and Moongu Jeon (2012)

Non-parametric methods:
*5 Pixel-Based Adaptive Segmenter (PBAS) of Hofmann et al (2012)
*0 GMG of Godbehere et al (2012)
*6 VuMeter of Goyat et al (2006)
*7 KDE of Elgammal et al (2000)
*9 IMBS of Domenico Bloisi and Luca Iocchi (2012)

Eigenspace-based methods:
*1 Eigenbackground / SL-PCA of Oliver et al (2000)

Neural and neuro-fuzzy methods:
*3 Adaptive SOM of Maddalena and Petrosino (2008)
*3 Fuzzy Adaptive SOM of Maddalena and Petrosino (2010)

Legend:
*0 native from OpenCV
*1 thanks to Donovan Parks
*2 thanks to Thierry Bouwmans, Fida EL BAF and Zhenjie Zhao
*3 thanks to Laurence Bender
*4 thanks to Jian Yao and Jean-Marc Odobez
*5 thanks to Martin Hofmann, Philipp Tiefenbacher and Gerhard Rigoll
*6 thanks to Lionel Robinault and Antoine Vacavant
*7 thanks to Ahmed Elgammal
*8 thanks to Csaba Kertész
*9 thanks to Domenico Daniele Bloisi
*10 thanks to SeungJong Noh

Download links:
---------------

MFC BGS Library x86 1.4.0 (with src) (uses bgslibrary v1.9.0)
https://github.com/andrewssobral/bgslibrary/blob/master/binaries/mfc_bgslibrary_x86_v1.4.0.7z

Windows 32bits
https://github.com/andrewssobral/bgslibrary/blob/master/binaries/bgs_library_x86_v1.5.0_with_gui.7z

Windows 64bits
https://github.com/andrewssobral/bgslibrary/blob/master/binaries/bgs_library_x64_v1.5.0_with_gui.7z

For Linux users:
----------------
Check out latest project source code.

Read instructions in README.txt file.

How to use BGS Library in other C++ code: 
-----------------------------------------
Download latest project source code, copy package_bgs directory to your project and create config folder (bgslibrary use it to store xml configuration files). For Windows users, a demo project for Visual Studio 2010 is provided. 

See Demo.cpp example source code at:
https://github.com/andrewssobral/bgslibrary/blob/master/Demo.cpp

Release Notes:
--------------
* Version 1.9.0:
Added A New Framework for Background Subtraction Using Multiple Cues (SJN_MultiCueBGS) of SeungJong Noh and Moongu Jeon (2012). 

* Version 1.8.0: 
Added Independent Multimodal Background Subtraction (IMBS) of Domenico Daniele Bloisi (2012).
Added Adaptive-Selective Background Model Learning. 

* Version 1.7.0: 
Added Texture-Based Foreground Detection with MRF of Csaba Kertész (2011). Some improvements and bug fixes, ... 

* Version 1.6.0: 
Added KDE of A. Elgammal, D. Harwood, L. S. Davis, “Non-parametric Model for Background Subtraction” ECCV'00 (thanks to Elgammal). 
Added Texture-based Background Subtraction of Marko Heikkila and Matti Pietikainen “A texture-based method for modeling the background and detecting moving objects” PAMI'06. 
Added OpenCV 2.4.5 support, some improvements and bug fixes, ... 

* Version 1.5.0: 
Added VuMeter of Yann Goyat, Thierry Chateau, Laurent Malaterre and Laurent Trassoudaine (thanks to Antoine Vacavant). 
Added OpenCV C++ MFC App (with source code) using BGS Library for Windows users. 
Added OpenCV 2.4.4 support (all dependencies are linked statically -- bye DLL's), some improvements and bug fixes, ... 

* Version 1.4.0: 
Added PBAS (Pixel-based adaptive Segmenter) of M. Hofmann, P. Tiefenbacher and G. Rigoll.
Added T2F-GMM with MRF of Zhenjie Zhao, Thierry Bouwmans, Xubo Zhang and Yongchun Fang. (thanks to Zhenjie Zhao and Thierry Bouwmans)
Added GMG of A. Godbehere, A. Matsukawa, K. Goldberg (opencv native).
Added OpenCV 2.4.3 support (all dependencies are linked statically -- bye DLL's), some improvements and bug fixes, ... 

* Version 1.3.0: 
Added Fuzzy Sugeno and Choquet Integral with Adaptive-Selective Background Model Update (thanks to Thierry Bouwmans) 
Foreground Mask Analysis upgrade, now with number of True Positives (TP), True Negatives (TN), False Positives (FP), False Negatives (FN), Detection Rate, Precision, Fmeasure, Accuracy, False Negative Rate (FNR), False Positive Rate (FPR), True Positive Rate (TPR) and ROC images (thanks to Thierry Bouwmans) 
Added OpenCV 2.4 support 
Some improvements, bug fixes, ... 

* Version 1.2.0: 
Added Multi-Layer BGS (thanks to Jian Yao and Jean-Marc Odobez) 
Added Background Subtraction Models from Laurence Bender (Simple Gaussian, Fuzzy Gaussian, Mixture of Gaussians, Adaptive SOM and Fuzzy Adaptive SOM) 
Added Foreground Mask Analysis (Similarity Measure) 

* Version 1.1.0: 
Added Type2-Fuzzy GMM UM and UV (thanks to Thierry Bouwmans) 
Added support to calculate average time of algorithms (see param tictoc in ./config/FrameProcessor.xml) 

* Version 1.0.0:
First stable version 
Added 14 background subtraction algorithms (07 adapted from Donovan Parks)