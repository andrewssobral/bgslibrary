BGSLibrary
==========

A Background Subtraction Library

Last Page Update: **22/02/2014**

Latest Library Version: **1.9.0** (see Release Notes for more info)

The BGSLibrary was developed by [Andrews Sobral](http://about.me/andrewssobral/) and provides a C++ framework to perform background subtraction (BGS). The code works either on Windows or on Linux. Currently the library offers **34** BGS algorithms (*the PBAS algorithm was removed from BGSLibrary because it is based on patented algorithm [ViBE](http://www2.ulg.ac.be/telecom/research/vibe/)*). A large amount of algorithms were provided by several authors. The source code is available under GNU GPL v3 license, the library is free and open source for academic purposes. Any user can be download latest project source code using SVN client. In Windows, a demo project for Visual Studio 2010 is provided. An executable version of BGSLibrary is available for Windows 32 bits and 64 bits. For Linux users, a Makefile can be used to compile all files and generate an executable example.
<p align="center">
<img src="https://sites.google.com/site/andrewssobral/bgslibrary190_world_countries.png" />
</p>

<p align="center">
Have you found this software useful? Consider donating US$10 so it can get even better! <br />
<a href="https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=andrewssobral%40gmail%2ecom&lc=BR&item_name=bgslibrary&currency_code=USD&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHostedGuest"><img src="https://www.paypalobjects.com/en_US/i/btn/btn_donateCC_LG.gif" hspace="0" vspace="0" border="0" /></a><br />
This software is completely free and will always stay free. Enjoy!
</p>

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
month     = {Jun}
} 
```
Some algorithms of the BGSLibrary was used successfully in the following papers: 
* (2014) Sobral, Andrews; Vacavant, Antoine. A comprehensive review of background subtraction algorithms evaluated with synthetic and real videos. Computer Vision and Image Understanding (CVIU), 2014.
* (2013) Sobral, Andrews; Oliveira, Luciano; Schnitman, Leizer; Souza, Felippe. (**Best Paper Award**) Highway Traffic Congestion Classification Using Holistic Properties. In International Conference on Signal Processing, Pattern Recognition and Applications (SPPRA'2013), Innsbruck, Austria, Feb 2013.

List of the algorithms available in BGSLibrary
----------------------------------------------
* Basic methods, mean and variance over time:
* *  Static Frame Difference 

* *  Frame Difference

* *  Weighted Moving Mean

* *  Weighted Moving Variance
 
* *  Adaptive Background Learning

* *  Adaptive-Selective Background Learning
 
* *  (1) Temporal Mean

* *  (1) Adaptive Median of McFarlane and Schofield (1995) ([paper](http://link.springer.com/article/10.1007%2FBF01215814))

* *  (1) Temporal Median of Cucchiara et al (2003) and Calderara et al (2006)  ([paper1](http://ieeexplore.ieee.org/iel5/34/27651/01233909.pdf?tp=a&amp;arnumber=1233909&amp;authDecision=-203))
([paper2](http://dl.acm.org/citation.cfm?id=1178814))
([paper3](http://cvrr.ucsd.edu/aton/publications/pdfpapers/96_cucchiara_r.PDF))

* Fuzzy based methods:
* * (2) Fuzzy Sugeno Integral (with Adaptive-Selective Update) of Hongxun Zhang and De Xu (2006) ([paper](http://www.springerlink.com/content/m73165175218731l/))

* * (2) Fuzzy Choquet Integral (with Adaptive-Selective Update) of Baf et al (2008) ([paper](http://hal.inria.fr/docs/00/33/30/86/PDF/FUZZ-IEEE_2008.pdf))

* * (3) Fuzzy Gaussian of Laurence Bender (adapted version of Wren (1997) with Sigari et al (2008) approach) ([paper](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.113.9538))

* Statistical methods using one gaussian:
* * (1) Gaussian Average of Wren (1997) ([paper](http://www.cvmt.dk/education/teaching/e06/CVG9/CV/CVG9ExercisePapers/pfinder.pdf))

* * (3) Simple Gaussian of Benezeth et al (2008) ([paper](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&amp;arnumber=4760998))

* Statistical methods using multiple gaussians:
* * (1) Gaussian Mixture Model of Stauffer and Grimson (1999) ([paper](http://www.ai.mit.edu/projects/vsam/Publications/stauffer_cvpr98_track.pdf))

* * (0) Gaussian Mixture Model of KadewTraKuPong and Bowden (2001) ([paper](http://info.ee.surrey.ac.uk/CVSSP/Publications/papers/KaewTraKulPong-AVBS01.pdf))

* * (0) Gaussian Mixture Model of Zivkovic (2004) ([paper1](http://staff.science.uva.nl/~zivkovic/Publications/zivkovic2004ICPR.pdf))
([paper2](http://staff.science.uva.nl/~zivkovic/Publications/zivkovicPRL2006.pdf))

* * (1) Gaussian Mixture Model of Zivkovic (2004)

* * (3) Gaussian Mixture Model of Laurence Bender (implements the classic GMM with Mahalanobis distance) ([paper](http://hal.archives-ouvertes.fr/docs/00/33/82/06/PDF/RPCS_2008.pdf))

* Type-2 Fuzzy based methods:
* * (2) Type-2 Fuzzy GMM-UM of Baf et al (2008) ([paper](http://hal.inria.fr/docs/00/33/65/12/PDF/ISVC_2008.pdf))

* * (2) Type-2 Fuzzy GMM-UV of Baf et al (2008) ([paper](http://hal.inria.fr/docs/00/33/65/12/PDF/ISVC_2008.pdf))

* * (2) Type-2 Fuzzy GMM-UM with MRF of Zhao et al (2012) ([paper1](http://link.springer.com/chapter/10.1007%2F978-3-642-35286-7_23))
([paper2](https://sites.google.com/site/andrewssobral/2012_Zhao.pdf))

* * (2) Type-2 Fuzzy GMM-UV with MRF of Zhao et al (2012) ([paper1](http://link.springer.com/chapter/10.1007%2F978-3-642-35286-7_23))
([paper2](https://sites.google.com/site/andrewssobral/2012_Zhao.pdf))

* Statistical methods using color and texture features:
* * (1) Texture BGS of Heikkila et al. (2006) ([paper](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=1597122&tag=1))

* * (8) Texture-Based Foreground Detection with MRF of Csaba Kertész (2011) ([paper](http://www.sersc.org/journals/IJSIP/vol4_no4/5.pdf))

* * (4) Multi-Layer BGS of Jian Yao and Jean-Marc Odobez (2007) ([paper](http://www.idiap.ch/~odobez/human-detection/doc/YaoOdobezCVPR-VS2007.pdf))

* * (10) MultiCue BGS of SeungJong Noh and Moongu Jeon (2012) ([paper](http://link.springer.com/chapter/10.1007%2F978-3-642-37431-9_38))

* Non-parametric methods:
* * ~~(5) Pixel-Based Adaptive Segmenter (PBAS) of Hofmann et al (2012)~~ 
**The PBAS algorithm was removed from BGSLibrary because it is based on patented algorithm [ViBE](http://www2.ulg.ac.be/telecom/research/vibe/).** ([paper](http://www.mmk.ei.tum.de/publ//pdf/12/12hof2.pdf))

* * (0) GMG of Godbehere et al (2012) ([paper](http://goldberg.berkeley.edu/pubs/acc-2012-visual-tracking-final.pdf))

* * (6) VuMeter of Goyat et al (2006) ([paper](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=1706852))

* * (7) KDE of Elgammal et al (2000) ([paper](http://www.cs.umd.edu/users/elgammal/docs/bgmodel_ECCV00_postfinal.pdf))

* * (9) IMBS of Domenico Bloisi and Luca Iocchi (2012) ([paper](http://www.dis.uniroma1.it/~bloisi/papers/bloisi-iocchi-imbs.pdf))

* Eigenspace-based methods:
* * (1) Eigenbackground / SL-PCA of Oliver et al (2000) ([paper](http://cis.temple.edu/~latecki/Courses/CIS750-03/Papers/oliver00.pdf))

* Neural and neuro-fuzzy methods:
* * (3) Adaptive SOM of Maddalena and Petrosino (2008) ([paper](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&amp;arnumber=4527178))

* * (3) Fuzzy Adaptive SOM of Maddalena and Petrosino (2010) ([paper](http://www.springerlink.com/content/112681k31222013n/))

Legend:
* (0) native from [OpenCV](http://docs.opencv.org/trunk/doc/tutorials/video/background_subtraction/background_subtraction.html)
* (1) thanks to [Donovan Parks](http://dparks.wikidot.com/source-code)
* (2) thanks to [Thierry Bouwmans](http://sites.google.com/site/thierrybouwmans/), [Fida EL BAF](https://sites.google.com/site/fidaelbaf/) and Zhenjie Zhao
* (3) thanks to [Laurence Bender](http://scene.sourceforge.net/)
* (4) thanks to [Jian Yao and Jean-Marc Odobez](http://www.idiap.ch/~odobez/human-detection/related-publications.html)
* (5) thanks to [Martin Hofmann, Philipp Tiefenbacher and Gerhard Rigoll](https://sites.google.com/site/pbassegmenter/home)
* (6) thanks to Lionel Robinault and [Antoine Vacavant](http://isit.u-clermont1.fr/~anvacava/index.html)
* (7) thanks to [Ahmed Elgammal](http://www.cs.rutgers.edu/~elgammal/Web_Page/Ahmed_Elgammal_Web_Page.html)
* (8) thanks to [Csaba Kertész](http://www.linkedin.com/pub/csaba-kert%C3%A9sz/8/341/6b6)
* (9) thanks to [Domenico Daniele Bloisi](http://www.dis.uniroma1.it/~bloisi/software/imbs.html)
* (10) thanks to [SeungJong Noh](http://www.informatik.uni-trier.de/~ley/pers/hd/n/Noh:SeungJong)

![](https://sites.google.com/site/andrewssobral/bgslibrary_benchmark.png "")

Download links
--------------

* BGSLibrary v1.9.0 with MFC GUI v1.4.0 (x86/x64) (+src)

https://github.com/andrewssobral/bgslibrary/blob/master/binaries/mfc_bgslibrary_x86_v1.4.0.7z

* BGSLibrary v1.5.0 with Java GUI for Windows 32bits (x86)

https://github.com/andrewssobral/bgslibrary/blob/master/binaries/bgs_library_x86_v1.5.0_with_gui.7z

* BGSLibrary v1.5.0 with Java GUI for Windows 64bits (x64)

https://github.com/andrewssobral/bgslibrary/blob/master/binaries/bgs_library_x64_v1.5.0_with_gui.7z

![MFC BGSLibrary](https://sites.google.com/site/andrewssobral/mfc_bgslibrary_120.png "MFC BGSLibrary")

For Linux users
---------------
Check out latest project source code.

Read instructions in README.txt file.

![BGSLibrary Linux](https://sites.google.com/site/andrewssobral/bgslibrary_ubuntu.png "BGSLibrary Linux")

How to use BGS Library in other C++ code
----------------------------------------
Download latest project source code, copy package_bgs directory to your project and create config folder (bgslibrary use it to store xml configuration files). For Windows users, a demo project for Visual Studio 2010 is provided. 

See Demo.cpp example source code at:
https://github.com/andrewssobral/bgslibrary/blob/master/Demo.cpp

Example code
------------
```C++
#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "package_bgs/FrameDifferenceBGS.h"

void main(int argc, char **argv)
{
  CvCapture *capture = 0;
  capture = cvCaptureFromCAM(0);
  
  if(!capture){
    std::cerr << "Cannot initialize video!" << std::endl;
    return;
  }
  
  IBGS *bgs;
  bgs = new FrameDifferenceBGS;
  
  IplImage *frame;
  int key = 0;
  while(key != 'q')
  {
    frame = cvQueryFrame(capture);
    if(!frame) break;

    cv::Mat img_input(frame);
    cv::imshow("Input", img_input);

    cv::Mat img_mask;
    cv::Mat img_bkgmodel;
    
    // by default, it shows automatically the foreground mask image
    bgs->process(img_input, img_mask, img_bkgmodel);
    
    //if(!img_mask.empty())
    //  cv::imshow("Foreground", img_mask);
    //  do something
    
    key = cvWaitKey(33);
  }

  delete bgs;

  cvDestroyAllWindows();
  cvReleaseCapture(&capture);
}
```

Best public video databases
---------------------------
![Alt text](https://sites.google.com/site/andrewssobral/datasets_public.png "Optional title")
* ChangeDetection: http://changedetection.net/
* BMC: http://bmc.univ-bpclermont.fr/

Project Diagram
---------------
<p align="center">
<a href="http://sites.google.com/site/andrewssobral/_/rsrc/1332121146046/bgslibrary_arch.png" target="_blank">
<img src="http://sites.google.com/site/andrewssobral/_/rsrc/1332121146046/bgslibrary_arch.png?width=815" border="0" />
</a>
</p>

Release Notes:
--------------
* Version 1.9.0:
Added A New Framework for Background Subtraction Using Multiple Cues (SJN_MultiCueBGS) of SeungJong Noh and Moongu Jeon (2012). Added OpenCV 2.4.8 support (all dependencies are linked statically).

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
