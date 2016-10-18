BGSLibrary
==========

A Background Subtraction Library

Last Page Update: **18/10/2016**

Latest Library Version: **1.9.2** (see Release Notes for more info)

The BGSLibrary was developed by [Andrews Sobral](https://sites.google.com/site/andrewssobral) and provides an easy-to-use C++ framework based on [OpenCV](http://www.opencv.org/) to perform background subtraction (BGS) in videos. The BGSLibrary compiles under Linux, Mac OS X and Windows. Currently the library offers **37**¹ BGS algorithms. A large amount of algorithms were provided by several authors. The source code is available under GNU GPL v3 license, the library is free and open source for academic purposes².

For Windows users, a demo project for Visual Studio 2010/2013 is provided. An executable version of BGSLibrary is available for Windows 32 bits and 64 bits. For Linux and Mac users, a Makefile can be used to compile all files.

Note: the BGSLibrary is based on OpenCV 2.X, if you want to use with OpenCV 3.x please check-out our [opencv3](https://github.com/andrewssobral/bgslibrary/tree/opencv3) branch.

***¹ The [PBAS](https://sites.google.com/site/pbassegmenter/home) algorithm was removed from BGSLibrary because it is based on patented algorithm [ViBE](http://www2.ulg.ac.be/telecom/research/vibe/).***

***² Some algorithms of the bgslibrary are free for commercial purposes and others not. First you need to contact the authors of your desired background subtraction method and check with them the appropriate license. For more additional information, please see: [Can I use a GPLv3 software as a part of my commercial application?](http://stackoverflow.com/questions/2280742/gplv3-can-i-use-these-programs-in-commercial).***

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
One chapter about the BGSLibrary has been published in the Handbook on "[Background Modeling and Foreground Detection for Video Surveillance](https://sites.google.com/site/backgroundmodeling/)".
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
* Sobral, Andrews. BGSLibrary: An OpenCV C++ Background Subtraction Library. IX Workshop de Visão Computacional (WVC'2013), Rio de Janeiro, Brazil, Jun. 2013. ([PDF](http://www.researchgate.net/publication/257424214_BGSLibrary_An_OpenCV_C_Background_Subtraction_Library) in brazilian portuguese with english abstract).
* Sobral, Andrews; Bouwmans, Thierry. "BGS Library: A Library Framework for Algorithm’s Evaluation in Foreground/Background Segmentation". Chapter on the handbook "Background Modeling and Foreground Detection for Video Surveillance", CRC Press, Taylor and Francis Group, 2014. ([PDF](http://www.researchgate.net/publication/257424214_BGSLibrary_An_OpenCV_C_Background_Subtraction_Library) in english).

References
----------
Some algorithms of the BGSLibrary was used successfully in my following papers: 
* (2014) Sobral, Andrews; Vacavant, Antoine. A comprehensive review of background subtraction algorithms evaluated with synthetic and real videos. Computer Vision and Image Understanding (CVIU), 2014. ([Online](http://dx.doi.org/10.1016/j.cviu.2013.12.005)) ([PDF](http://www.researchgate.net/publication/259340906_A_comprehensive_review_of_background_subtraction_algorithms_evaluated_with_synthetic_and_real_videos))
* (2013) Sobral, Andrews; Oliveira, Luciano; Schnitman, Leizer; Souza, Felippe. (**Best Paper Award**) Highway Traffic Congestion Classification Using Holistic Properties. In International Conference on Signal Processing, Pattern Recognition and Applications (SPPRA'2013), Innsbruck, Austria, Feb 2013. ([Online](http://dx.doi.org/10.2316/P.2013.798-105)) ([PDF](http://www.researchgate.net/publication/233427564_HIGHWAY_TRAFFIC_CONGESTION_CLASSIFICATION_USING_HOLISTIC_PROPERTIES))

List of the algorithms available in BGSLibrary
----------------------------------------------
|<sub>Type                       </sub>|<sub>Algorithm name <br/>(click to see the source code)                              </sub>|<sub>                   Author(s)                          </sub>|<sub> Contributor(s) </sub>|<sub> License        </sub>|
|--------------------------------------|-------------------------------------------------------|:--------------------------------------------------------------:|:-------------------------:|:-------------------------:|
|<sub>**Basic:**                 </sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[Static Frame Difference](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/StaticFrameDifferenceBGS.cpp)                     </sub>|<sub> -                                                   </sub>|<sub>        -       </sub>|<sub>Free</sub>            |
|                                      |<sub>[Frame Difference](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/FrameDifferenceBGS.cpp)                            </sub>|<sub> -                                                   </sub>|<sub>        -       </sub>|<sub>Free</sub>            |
|                                      |<sub>[Weighted Moving Mean](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/WeightedMovingMeanBGS.cpp)                        </sub>|<sub> -                                                   </sub>|<sub>        -       </sub>|<sub>Free</sub>            |
|                                      |<sub>[Weighted Moving Variance](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/WeightedMovingVarianceBGS.cpp)                    </sub>|<sub> -                                                   </sub>|<sub>        -       </sub>|<sub>Free</sub>            |
|                                      |<sub>[Adaptive Background Learning](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/AdaptiveBackgroundLearning.cpp)                </sub>|<sub> -                                                   </sub>|<sub>        -       </sub>|<sub>Free</sub>            |
|                                      |<sub>[Adaptive-Selective Background Learning](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/AdaptiveSelectiveBackgroundLearning.cpp)      </sub>|<sub> -                                                   </sub>|<sub>        -       </sub>|<sub>Free</sub>            |
|                                      |<sub>[Temporal Mean](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/dp/DPMeanBGS.cpp)                               </sub>|<sub> -                                                   </sub>|<sub>        1       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Adaptive Median](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/dp/DPAdaptiveMedianBGS.cpp)                             </sub>|<sub>[McFarlane and Schofield (1995)](http://link.springer.com/article/10.1007%2FBF01215814)                      </sub>|<sub>        1       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Temporal Median](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/dp/DPPratiMediodBGS.cpp)                             </sub>|<sub>[Cucchiara et al. (2003)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=957036) and [Calderara et al. (2006)](http://dl.acm.org/citation.cfm?id=1178814) </sub>|<sub>        1       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Sigma-Delta](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/bl/SigmaDeltaBGS.cpp)                                 </sub>|<sub>[Manzanera and Richefeu (2004)](http://dl.acm.org/citation.cfm?id=1222999)                       </sub>|<sub>       11       </sub>|<sub>GPLv3</sub>           |
|                                      |                                                       |                                                                |                           |                           |
|<sub>**Fuzzy:**                 </sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[Fuzzy Sugeno Integral](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/tb/FuzzySugenoIntegral.cpp)                       </sub>|<sub>[Hongxun Zhang and De Xu (2006)](http://www.springerlink.com/content/m73165175218731l/)                       </sub>|<sub>        2       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Fuzzy Choquet Integral](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/tb/FuzzyChoquetIntegral.cpp)                      </sub>|<sub>[Baf et al. (2008)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=4630604)                                    </sub>|<sub>        2       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Fuzzy Gaussian](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/lb/LBFuzzyGaussian.cpp)                              </sub>|<sub>[Wren (1997)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=598236) with [Sigari et al. (2008)](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.113.9538) approach       </sub>|<sub>        3       </sub>|<sub>GPLv2</sub>           |
|                                      |                                                       |                                                                |                           |                           |
|<sub>**Single gaussian:**       </sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[Gaussian Average](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/dp/DPWrenGABGS.cpp)                            </sub>|<sub>[Wren (1997)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=598236)                                          </sub>|<sub>        1       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Simple Gaussian](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/lb/LBSimpleGaussian.cpp)                             </sub>|<sub>[Benezeth et al. (2008)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=4760998)                               </sub>|<sub>        3       </sub>|<sub>GPLv2</sub>           |
|                                      |                                                       |                                                                |                           |                           |
|<sub>**Multiple gaussians:**    </sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[Gaussian Mixture Model (GMM or MoG)](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/dp/DPGrimsonGMMBGS.cpp)         </sub>|<sub>[Stauffer and Grimson (1999)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=784637)                          </sub>|<sub>        1       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Gaussian Mixture Model (GMM or MoG)](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/MixtureOfGaussianV1BGS.cpp)         </sub>|<sub>[KadewTraKuPong and Bowden (2001)](http://link.springer.com/chapter/10.1007%2F978-1-4615-0913-4_11)                     </sub>|<sub>        0       </sub>|<sub>BSD</sub>             |
|                                      |<sub>[Gaussian Mixture Model (GMM or MoG)](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/MixtureOfGaussianV2BGS.cpp)         </sub>|<sub>[Zivkovic (2004)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=1333992)                                      </sub>|<sub>        0       </sub>|<sub>BSD</sub>             |
|                                      |<sub>[Gaussian Mixture Model (GMM or MoG)](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/dp/DPZivkovicAGMMBGS.cpp)         </sub>|<sub>[Zivkovic (2004)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=1333992)                                      </sub>|<sub>        1       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Gaussian Mixture Model (GMM or MoG)](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/lb/LBMixtureOfGaussians.cpp)         </sub>|<sub>Laurence Bender implementation (GMM with Mahalanobis distance) ([related paper](https://hal.archives-ouvertes.fr/hal-00338206/en/))      </sub>|<sub>        3       </sub>|<sub>GPLv2</sub>           |
|                                      |                                                       |                                                                |                           |                           |
|<sub>**Type-2 Fuzzy:**          </sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[Type-2 Fuzzy GMM-UM](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/tb/T2FGMM_UM.cpp)                         </sub>|<sub>[Baf et al. (2008)](http://dl.acm.org/citation.cfm?id=1486056)                                    </sub>|<sub>        2       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Type-2 Fuzzy GMM-UV](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/tb/T2FGMM_UV.cpp)                         </sub>|<sub>[Baf et al. (2008)](http://dl.acm.org/citation.cfm?id=1486056)                                    </sub>|<sub>        2       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Type-2 Fuzzy GMM-UM with MRF](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/tb/T2FMRF_UM.cpp)                </sub>|<sub>[Zhao et al. (2012)](http://link.springer.com/chapter/10.1007%2F978-3-642-35286-7_23)                                   </sub>|<sub>        2       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Type-2 Fuzzy GMM-UV with MRF](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/tb/T2FMRF_UV.cpp)                </sub>|<sub>[Zhao et al. (2012)](http://link.springer.com/chapter/10.1007%2F978-3-642-35286-7_23)                                   </sub>|<sub>        2       </sub>|<sub>GPLv3</sub>           |
|                                      |                                                       |                                                                |                           |                           |
|<sub>**Multiple features:**     </sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[Texture BGS](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/dp/DPTextureBGS.cpp)                                 </sub>|<sub>[Heikkila et al. (2006)](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=1597122&tag=1)                               </sub>|<sub>        1       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Texture-Based Foreground Detection with MRF](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/ck/LbpMrf.cpp) </sub>|<sub>[Csaba Kertész (2011)](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.359.4338)                                 </sub>|<sub>        8       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Multi-Layer BGS](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/jmo/MultiLayerBGS.cpp)                             </sub>|<sub>[Jian Yao and Jean-Marc Odobez (2007)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=4270495)                 </sub>|<sub>        4       </sub>|<sub>BSD</sub>             |
|                                      |<sub>[MultiCue BGS](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/sjn/SJN_MultiCueBGS.cpp)                                </sub>|<sub>[SeungJong Noh and Moongu Jeon (2012)](http://link.springer.com/chapter/10.1007%2F978-3-642-37431-9_38)                 </sub>|<sub>        10      </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[SuBSENSE](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/pl/SuBSENSE.cpp)                                    </sub>|<sub>[Pierre-Luc et al. (2014)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=6910015)                             </sub>|<sub>        12      </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[LOBSTER](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/pl/LOBSTER.cpp)                                     </sub>|<sub>[Pierre-Luc and Guillaume-Alexandre (2014)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=6836059)            </sub>|<sub>        12      </sub>|<sub>GPLv3</sub>           |
|                                      |                                                       |                                                                |                           |                           |
|<sub>**Non-parametric:**        </sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[GMG](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/GMG.cpp)                                         </sub>|<sub>[Godbehere et al. (2012)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=6315174)                              </sub>|<sub>        0       </sub>|<sub>BSD</sub>             |
|                                      |<sub>[VuMeter](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/av/VuMeter.cpp)                                     </sub>|<sub>[Goyat et al. (2006)](http://ieeexplore.ieee.org/xpls/abs_all.jsp?arnumber=1706852)                                  </sub>|<sub>        6       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[KDE](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/ae/KDE.cpp)                                         </sub>|<sub>[Elgammal et al. (2000)](http://link.springer.com/chapter/10.1007%2F3-540-45053-X_48)                               </sub>|<sub>        7       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[IMBS](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/db/IndependentMultimodalBGS.cpp)                                        </sub>|<sub>[Domenico Bloisi and Luca Iocchi (2012)](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.414.7754)               </sub>|<sub>        9       </sub>|<sub>GPLv3</sub>           |
|<sub>*(only in opencv3 branch)* </sub>|<sub>[KNN Background Subtractor](https://github.com/andrewssobral/bgslibrary/blob/opencv3/package_bgs/KNNBGS.cpp)                   </sub>|<sub>[Zoran Zivkovic and Ferdinand van der Heijden](http://www.sciencedirect.com/science/article/pii/S0167865505003521)                                                     </sub>|<sub>        0       </sub>|<sub>BSD</sub>             |
|                                      |                                                       |                                                                |                           |                           |
|<sub>**Subspace:**              </sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[Eigenbackground / SL-PCA](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/dp/DPEigenbackgroundBGS.cpp)                    </sub>|<sub>[Oliver et al. (2000)](http://ieeexplore.ieee.org/xpl/freeabs_all.jsp?arnumber=868684)                                 </sub>|<sub>        1       </sub>|<sub>GPLv3</sub>           |
|                                      |                                                       |                                                                |                           |                           |
|<sub>**Neural and neuro-fuzzy:**</sub>|                                                       |                                                                |                           |                           |
|                                      |<sub>[Adaptive SOM](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/lb/LBAdaptiveSOM.cpp)                                </sub>|<sub>[Maddalena and Petrosino (2008)](http://dl.acm.org/citation.cfm?id=2321600)                       </sub>|<sub>        3       </sub>|<sub>GPLv3</sub>           |
|                                      |<sub>[Fuzzy Adaptive SOM](https://github.com/andrewssobral/bgslibrary/blob/master/package_bgs/lb/LBFuzzyAdaptiveSOM.cpp)                          </sub>|<sub>[Maddalena and Petrosino (2010)](http://www.springerlink.com/content/112681k31222013n/)                       </sub>|<sub>        3       </sub>|<sub>GPLv3</sub>           |

<sub>Legend:</sub>
* <sub>(0) native from [OpenCV](http://docs.opencv.org/trunk/doc/tutorials/video/background_subtraction/background_subtraction.html).</sub>
* <sub>(1) thanks to [Donovan Parks](http://dparks.wikidot.com/source-code).</sub>
* <sub>(2) thanks to [Thierry Bouwmans](http://sites.google.com/site/thierrybouwmans/), [Fida EL BAF](https://sites.google.com/site/fidaelbaf/) and Zhenjie Zhao.</sub>
* <sub>(3) thanks to [Laurence Bender](http://scene.sourceforge.net/).</sub>
* <sub>(4) thanks to [Jian Yao and Jean-Marc Odobez](http://www.idiap.ch/~odobez/human-detection/related-publications.html).</sub>
* <sub>(5) thanks to [Martin Hofmann, Philipp Tiefenbacher and Gerhard Rigoll](https://sites.google.com/site/pbassegmenter/home).</sub>
* <sub>(6) thanks to Lionel Robinault and [Antoine Vacavant](http://isit.u-clermont1.fr/~anvacava/index.html).</sub>
* <sub>(7) thanks to [Ahmed Elgammal](http://www.cs.rutgers.edu/~elgammal/Web_Page/Ahmed_Elgammal_Web_Page.html).</sub>
* <sub>(8) thanks to [Csaba Kertész](http://www.linkedin.com/pub/csaba-kert%C3%A9sz/8/341/6b6).</sub>
* <sub>(9) thanks to [Domenico Daniele Bloisi](http://www.dis.uniroma1.it/~bloisi/software/imbs.html).</sub>
* <sub>(10) thanks to [SeungJong Noh](http://www.informatik.uni-trier.de/~ley/pers/hd/n/Noh:SeungJong).</sub>
* <sub>(11) thanks to [Benjamin Laugraud](http://www.montefiore.ulg.ac.be/~blaugraud/index.php).</sub>
* <sub>(12) thanks to [Pierre-Luc St-Charles](http://scholar.google.ca/citations?user=30mr9vYAAAAJ&hl=en).</sub>


Are you in doubt about which algorithm to choose?
-------------------------------------------------
Frequently, the question arises that given a problem, what is the best algorithm to choose? Unfortunately there is no exact answer, the performance of each algorithm may vary due to application and environment. However, the following resources are available to help you in this challenge:

* [A comprehensive review of background subtraction algorithms evaluated with synthetic and real videos](https://www.researchgate.net/publication/259340906_A_comprehensive_review_of_background_subtraction_algorithms_evaluated_with_synthetic_and_real_videos) (by Andrews Sobral and Antoine Vacavant)
* [Background Subtraction - A Survey](https://sites.google.com/site/thierrybouwmans/recherche---background-subtraction---survey) and [Background Subtraction Website](https://sites.google.com/site/backgroundsubtraction/overview) (by Thierry Bouwmans)
* [Comparing background subtraction algorithms](http://tommesani.com/index.php/video/comparing-background-subtraction-algorithms.html) (by Stefano Tommesani)
* [Results for CD.net 2014](http://wordpress-jodoin.dmi.usherb.ca/results2014/)
* See also: [Books, Journals, Workshops, Resources, Datasets, Codes, ...](https://en.wikipedia.org/wiki/Background_subtraction)

Algorithms benchmark
----------------------------------------------

![](https://sites.google.com/site/andrewssobral/bgslibrary_benchmark.png "")

Download links
--------------

* BGSLibrary v1.9.2 with MFC GUI v1.4.2 (x86/x64)

https://github.com/andrewssobral/bgslibrary/releases/download/v1.9.2_x86_mfc_gui/bgslibrary_x86_v1.9.2_with_mfc_gui_v1.4.2.7z

* BGSLibrary v1.9.2 with Java GUI for Windows 32bits (x86)

https://github.com/andrewssobral/bgslibrary/releases/download/v1.9.2_x86_java_gui/bgslibrary_x86_v1.9.2_with_java_gui_v1.0.4.7z

* BGSLibrary v1.9.2 with Java GUI for Windows 64bits (x64)

https://github.com/andrewssobral/bgslibrary/releases/download/v1.9.2_x64_java_gui/bgslibrary_x64_v1.9.2_with_java_gui_v1.0.4.7z

![MFC BGSLibrary](https://sites.google.com/site/andrewssobral/mfc_bgslibrary_120.png "MFC BGSLibrary")

For Linux and Mac users
-----------------------
Check out latest project source code.

Read instructions in README.txt file.

<p align="center"><img src="https://sites.google.com/site/andrewssobral/bgslibrary_ubuntu.png" border="0" /></p>

Docker images
----------------------------------------
Docker images are available for BGSLibrary with OpenCV 2.4.13 and OpenCV 3.1.0.

* **Ubuntu 16.04 + VNC + OpenCV 2.4.13 + Python 2.7 + BGSLibrary (master branch)**
https://hub.docker.com/r/andrewssobral/bgslibrary_opencv2/

* **Ubuntu 16.04 + VNC + OpenCV 3.1.0 (+ contrib) + Python 2.7 + BGSLibrary (opencv3 branch)**
https://hub.docker.com/r/andrewssobral/bgslibrary_opencv3/

<p align="center"><img src="https://sites.google.com/site/andrewssobral/bgslibrary_opencv3_docker.png?width=640" border="0" /></p>

How to use BGS Library in other C++ code
----------------------------------------
Download latest project source code, copy package_bgs directory to your project and create config folder (bgslibrary use it to store xml configuration files). For Windows users, a demo project for Visual Studio 2010 is provided.

See Demo.cpp example source code at:
https://github.com/andrewssobral/bgslibrary/blob/master/Demo.cpp

How to contribute with BGSLibrary project
-----------------------------------------
Everyone is invited to cooperate with the BGSLibrary project by sending any implementation of background subtraction (BS) algorithms. Please see the following tutorial:
https://github.com/andrewssobral/bgslibrary/blob/master/docs/bgslibrary_how_to_contribute.pdf

Full list of BGSLibrary collaborators
-------------------------------------
I would like to thanks all those who have contributed in some way to the success of this library, especially, the following peoples (in alphabetical order):

Ahmed Elgammal	(USA), Antoine Vacavant	(France), Benjamin Laugraud	(Belgium), Csaba Kertész	(Finland), Domenico Bloisi	(Italy), Donovan Parks	(Canada), Eduardo Barreto Alexandre	(Brazil), Fida EL BAF	(France), Iñigo Martínez, Jean-Marc Odobez	(Switzerland), Jean-Philippe Jodoin	(Canada), JIA Pei	(China), Jian Yao	(China), Hemang Shah, Holger Friedrich, Laurence Bender	(Argentina), Lionel Robinault	(France), Luca Iocchi	(Italy), Luiz Vitor Martinez Cardoso	(Brazil), Martin Hofmann, Philipp Tiefenbacher and Gerhard Rigoll	(Germany), Rim Trabelsi	(Tunisia), Simone Gasparini	(France), Stefano Tommesani	(Italy), Thierry Bouwmans	(France), Vikas Reddy	(Australia), Yani Ioannou	(Canada), Zhenjie Zhao	(China) and Zoran Zivkovic	(Netherlands).

Example code
------------
```C++
#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "package_bgs/FrameDifferenceBGS.h"

int main(int argc, char **argv)
{
  CvCapture *capture = 0;
  capture = cvCaptureFromCAM(0);

  if(!capture){
    std::cerr << "Cannot initialize video!" << std::endl;
    return -1;
  }

  IBGS *bgs;
  bgs = new FrameDifferenceBGS;

  IplImage *frame;
  while(1)
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

    if(cvWaitKey(33) >= 0)
		  break;
  }

  delete bgs;

  cvDestroyAllWindows();
  cvReleaseCapture(&capture);

  return 0;
}
```

Videos
------

<p align="center">
<a href="https://www.youtube.com/watch?v=yjorJu7rRiE" target="_blank">
<img src="https://sites.google.com/site/andrewssobral/bgslibrary_mfc_youtube.png" border="0" />
</a>
</p>

<p align="center">
<a href="https://www.youtube.com/watch?v=Ccqa9KBO9_U" target="_blank">
<img src="https://sites.google.com/site/andrewssobral/bgslibrary_youtube.png" border="0" />
</a>
</p>

<!--
[![ScreenShot](https://sites.google.com/site/andrewssobral/bgslibrary_mfc_youtube.png)](https://www.youtube.com/watch?v=yjorJu7rRiE)
-->

Project Diagram
---------------
<p align="center">
<a href="http://sites.google.com/site/andrewssobral/_/rsrc/1332121146046/bgslibrary_arch.png" target="_blank">
<img src="http://sites.google.com/site/andrewssobral/_/rsrc/1332121146046/bgslibrary_arch.png?width=815" border="0" />
</a>
</p>

Java GUI
--------
<p align="center">
<a href="https://sites.google.com/site/andrewssobral/bgslibrary_gui_screen01.png" target="_blank">
<img src="https://sites.google.com/site/andrewssobral/bgslibrary_gui_screen01.png?width=815" border="0" />
</a>
</p>

<p align="center">
<a href="https://sites.google.com/site/andrewssobral/bgslibrary_gui_screen02.png" target="_blank">
<img src="https://sites.google.com/site/andrewssobral/bgslibrary_gui_screen02.png?width=815" border="0" />
</a>
</p>

<p align="center">
<a href="https://sites.google.com/site/andrewssobral/bgslibrary_gui_screen03.png" target="_blank">
<img src="https://sites.google.com/site/andrewssobral/bgslibrary_gui_screen03.png?width=815" border="0" />
</a>
</p>

<p align="center">
<a href="https://sites.google.com/site/andrewssobral/bgslibrary_gui_screen04.png" target="_blank">
<img src="https://sites.google.com/site/andrewssobral/bgslibrary_gui_screen04.png?width=815" border="0" />
</a>
</p>

Release Notes:
--------------
* Version 1.9.2:
Added SuBSENSE and LOBSTER algorithms of Pierre-Luc et al. (2014).

* Version 1.9.1:
Added Sigma-Delta background subtraction algorithm (SigmaDeltaBGS) of Manzanera and Richefeu (2004).

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
