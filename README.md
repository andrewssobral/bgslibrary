<p align="center">
<img src="https://github.com/andrewssobral/bgslibrary/blob/master/docs/images/bgslibrary-logo.jpg?raw=true" alt="BGSLibrary" width="200">
</p>

# BGSLibrary: A Background Subtraction Library

[![Release](https://img.shields.io/badge/Release-3.3.1-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT) [![Platform: Windows, Linux, OS X](https://img.shields.io/badge/Platform-Windows%2C%20Linux%2C%20OS%20X-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![OpenCV](https://img.shields.io/badge/OpenCV-2.4.x%2C%203.x%2C%204.x-blue.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![Wrapper: Python, MATLAB](https://img.shields.io/badge/Wrapper-Java%2C%20Python%2C%20MATLAB-orange.svg)](https://github.com/andrewssobral/bgslibrary/wiki/Build-status) [![Algorithms](https://img.shields.io/badge/Algorithms-43-red.svg)](https://github.com/andrewssobral/bgslibrary/wiki/List-of-available-algorithms) [![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/andrewssobral/bgslibrary)

<p align="center">
<a href="https://youtu.be/_UbERwuQ0OU" target="_blank">
<img src="https://raw.githubusercontent.com/andrewssobral/bgslibrary/master/docs/images/bgs_giphy2.gif" border="0" />
</a>
</p>

## Introduction

The **BGSLibrary** (Background Subtraction Library) is a comprehensive C++ framework designed for background subtraction in computer vision applications, particularly for detecting moving objects in video streams. It provides an easy-to-use and extensible platform for researchers and developers to experiment with and implement various background subtraction techniques.

## Library Version

**3.3.1** (see **[Build Status](https://github.com/andrewssobral/bgslibrary/wiki/Build-status)** and **[Release Notes](https://github.com/andrewssobral/bgslibrary/wiki/Release-notes)** for more info)

## Background and Development

The BGSLibrary was developed in early 2012 by [Andrews Cordolino Sobral](http://andrewssobral.wixsite.com/home) as a C++ framework with wrappers available for Python, Java, and MATLAB. It aims to facilitate foreground-background separation in videos using the OpenCV library.

## Compatibility

The library is compatible with OpenCV versions 2.4.x, 3.x, and 4.x. It can be compiled and used on Windows, Linux, and Mac OS X systems.

## Licensing

The library's source code is available under the [MIT license](https://opensource.org/licenses/MIT), making it free for both academic and commercial use.

## Getting started

* [List of available algorithms](https://github.com/andrewssobral/bgslibrary/wiki/List-of-available-algorithms)
* [Algorithms benchmark](https://github.com/andrewssobral/bgslibrary/wiki/Algorithms-benchmark)
* [Which algorithms really matter?](https://github.com/andrewssobral/bgslibrary/wiki/Which-algorithms-really-matter%3F)
* [Library architecture](https://github.com/andrewssobral/bgslibrary/wiki/Library-architecture)

[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/andrewssobral/bgslibrary)

```cpp
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>

// Include the OpenCV and BGSLibrary libraries
#include <opencv2/opencv.hpp>
#include <bgslibrary/algorithms/algorithms.h>

int main( int argc, char** argv )
{
    // Gets the names of the background subtraction algorithms registered in the BGSLibrary factory
    auto algorithmsName = BGS_Factory::Instance()->GetRegisteredAlgorithmsName();

    // Displays the number of available background subtraction algorithms in the BGSLibrary
    std::cout << "Number of available algorithms: " << algorithmsName.size() << std::endl;

    // Displays the list of available background subtraction algorithms in the BGSLibrary
    std::cout << "List of available algorithms:" << std::endl;
    std::copy(algorithmsName.begin(), algorithmsName.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

    // Returns 0 to indicate that the execution was successful
    return 0;
}
```

### Installation instructions

You can either install BGSLibrary via [pre-built binary package](https://github.com/andrewssobral/bgslibrary/releases) or build it from source.

**Building from source? Clone with submodules.** BGSLibrary bundles `pybind11` as a git submodule (used by the
Python wrapper), so clone recursively:

```bash
git clone --recursive https://github.com/andrewssobral/bgslibrary.git
```

Already cloned without `--recursive`? Fetch the submodule with:

```bash
git submodule update --init --recursive
```

> The `pip install pybgs` path below does **not** need this — the published PyPI package already bundles `pybind11`.

Supported Compilers:

* GCC 4.8 and above
* Clang 3.4 and above
* MSVC 2015, 2017, 2019 or newer

Other compilers might work, but are not officially supported.
The bgslibrary requires some features from the ISO C++ 2014 standard.

#### 🐍 Python wrapper (`pybgs`) via pip — quickest for Python users

If you only need the Python bindings, install the published package:

```bash
pip install pybgs
```

The same package is also published under the name [`bgslibrary`](https://pypi.org/project/bgslibrary/)
(`pip install bgslibrary`) — either way the importable module is always `pybgs`.

Minimal usage (background subtraction on a video):

```python
import cv2
import pybgs as bgs

algorithm = bgs.FrameDifference()          # see the note below on which algorithms are available
capture = cv2.VideoCapture("dataset/video.avi")
while True:
    ok, frame = capture.read()
    if not ok:
        break
    fg_mask = algorithm.apply(frame)             # foreground mask (CV_8UC1)
    bg_model = algorithm.getBackgroundModel()    # current background model
    cv2.imshow("frame", frame)
    cv2.imshow("foreground", fg_mask)
    cv2.imshow("background", bg_model)
    if cv2.waitKey(10) & 0xFF == 27:             # Esc to quit (mask for 64-bit platforms)
        break
cv2.destroyAllWindows()
```

Runnable demos: [`demo.py`](demo.py), [`demo2.py`](demo2.py), and the
[Python examples repo](https://github.com/andrewssobral/bgslibrary-examples-python).

> [!IMPORTANT]
> - `pip install pybgs` **builds from source**, so it needs a C++ compiler **and an OpenCV
>   development install** on your machine (e.g. `libopencv-dev` on Ubuntu, `brew install opencv` on
>   macOS). If you don't have OpenCV, prefer the **Pixi** build below — it provides OpenCV for you.
> - **The available algorithms depend on the OpenCV `pybgs` was *compiled* against — not on your
>   `opencv-python`/`cv2` version.** On OpenCV ≥ 4 the `DP*`/`T2F*` family is unavailable (26
>   algorithms vs ~39 on OpenCV 2/3 — see the [compatibility table](#algorithm-compatibility-across-opencv-versions)).
>   Always gate on the binding, e.g. `if hasattr(bgs, "DPAdaptiveMedian"): ...`, never on `cv2.__version__`.

#### 🚀 Build Using Pixi (Recommended)

Pixi provides a clean and reproducible way to build BGSLibrary with no manual OpenCV installation. It works on **macOS, Linux, and Windows (WSL recommended).**

##### 0. Install Pixi

```bash
curl -fsSL https://pixi.sh/install.sh | sh
```

Restart your terminal after installation.

##### 1. Dependencies and build tasks are preconfigured

The repository already ships a `pixi.toml` that declares all build
dependencies — OpenCV, CMake, Ninja, compilers and pkg-config, plus
Python and NumPy for the Python wrapper — and all the build tasks used
below. Just clone the repository (recursively — see [above](#installation-instructions)); there is no need to
run `pixi init` or add anything by hand.

List the available tasks at any time with:

```bash
pixi task list
```

##### 2. Activate the Pixi environment

```bash
pixi shell
```

##### 3. Configure the build

```bash
pixi run configure
```

##### 4. Build bgslibrary

```bash
pixi run build
```

This generates:
- `build/bgslibrary`
- `build/libbgslibrary_core.*`

##### 5. Build the Python wrapper (pybgs)

To build and use the Python bindings (`import pybgs`):

```bash
pixi run build_python     # configure + build pybgs (BGS_PYTHON_SUPPORT=ON, into build_py/)
pixi run install_python   # install pybgs into the Pixi environment's site-packages
python -c "import pybgs"  # verify the import works
```

`pybgs` is compiled against the OpenCV, Python and NumPy provided by the
Pixi environment, so no system Python or OpenCV installation is required.

##### 6. Build C++ examples (optional)

```bash
pixi run build_examples
```

This produces:
- `examples/build/bgs_demo`
- `examples/build/bgs_demo2`

##### 7. Run bgslibrary

Camera demo:

```bash
pixi run run
```

Demo using a video file:

```bash
pixi run run_bgs_demo
```

Demo using an image sequence:

```bash
pixi run run_bgs_demo2
```

##### 8. Install bgslibrary into the Pixi environment (optional)

```bash
pixi run install
```

The library and headers go into `.pixi/envs/default/`

You can then use bgslibrary from other CMake projects in the same environment, for example:

```cmake
find_package(BGSLibrary REQUIRED)
```

##### 9. Clean and rebuild

To clean build artifacts:

```bash
pixi run clean          # Clean main build
pixi run clean_examples # Clean examples
```

To rebuild everything from scratch:

```bash
pixi run rebuild     # Rebuild main project
pixi run rebuild_all # Rebuild main project + examples
```

These tasks automatically handle dependencies, ensuring a consistent build state.

#### 🛠 Build Using CMake (Classic Method)

* [Windows installation](https://github.com/andrewssobral/bgslibrary/wiki/Installation-instructions---Windows)
* [Ubuntu / OS X installation](https://github.com/andrewssobral/bgslibrary/wiki/Installation-instructions-Ubuntu-or-OSX)

### Troubleshooting

- **`pip install pybgs` fails to build / cannot find OpenCV** — it compiles from source and needs a C++
  compiler and OpenCV development files. Install OpenCV (`libopencv-dev` on Ubuntu, `brew install opencv` on
  macOS) and a compiler, or use the Pixi build above, which provides OpenCV for you.
- **`AttributeError: module 'pybgs' has no attribute 'DPAdaptiveMedian'`** (or another `DP*`/`T2F*` algorithm) —
  those algorithms only compile on OpenCV 2.x/3.x, so they are absent when `pybgs` was built against OpenCV ≥ 4.
  Check availability with `hasattr(bgs, "DPAdaptiveMedian")`; to use them, build against OpenCV 2/3 (see the
  compatibility table below).
- **`import cv2` raises an ABI / NumPy error with OpenCV 3.4.x** — `numpy 2.x` is incompatible with the older
  `opencv-python 3.4.x`; pin `numpy<2` in that environment.

### Graphical User Interface

* [C++ QT](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-QT) ***(Official)***
* [C++ MFC](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-MFC) ***(Deprecated)***
* [Java](https://github.com/andrewssobral/bgslibrary/wiki/Graphical-User-Interface:-Java) ***(Obsolete)***

### Wrappers

* [Python](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-Python) [![Downloads](https://static.pepy.tech/badge/pybgs)](https://pepy.tech/project/pybgs) [![Downloads](https://static.pepy.tech/badge/pybgs/month)](https://pepy.tech/project/pybgs) [![Downloads](https://static.pepy.tech/badge/pybgs/week)](https://pepy.tech/project/pybgs)
* [MATLAB](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-MATLAB)
* [Java](https://github.com/andrewssobral/bgslibrary/wiki/Wrapper:-Java)

### Usage examples

* BGSlibrary examples folder
* * <https://github.com/andrewssobral/bgslibrary/tree/master/examples>
* BGSlibrary examples in C++
* * <https://github.com/andrewssobral/bgslibrary-examples-cpp>
* BGSlibrary examples in Python
* * <https://github.com/andrewssobral/bgslibrary-examples-python>

### More

* [Docker images](https://github.com/andrewssobral/bgslibrary/wiki/Docker-images)
* [How to integrate BGSLibrary in your own CPP code](https://github.com/andrewssobral/bgslibrary/wiki/How-to-integrate-BGSLibrary-in-your-own-CPP-code)
* [How to contribute](https://github.com/andrewssobral/bgslibrary/wiki/How-to-contribute)
* [List of collaborators](https://github.com/andrewssobral/bgslibrary/wiki/List-of-collaborators)
* [Release notes](https://github.com/andrewssobral/bgslibrary/wiki/Release-notes)

## Algorithm compatibility across OpenCV versions

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

## Citation

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

A chapter about the BGSLibrary has been published in the handbook on [Background Modeling and Foreground Detection for Video Surveillance](https://sites.google.com/site/backgroundsubtraction/).

```
@incollection{bgslibrarychapter,
author    = {Sobral, Andrews and Bouwmans, Thierry},
title     = {BGS Library: A Library Framework for Algorithm’s Evaluation in Foreground/Background Segmentation},
booktitle = {Background Modeling and Foreground Detection for Video Surveillance},
publisher = {CRC Press, Taylor and Francis Group.}
year      = {2014},
}
```

## References

* Sobral, Andrews. BGSLibrary: An OpenCV C++ Background Subtraction Library. IX Workshop de Visão Computacional (WVC'2013), Rio de Janeiro, Brazil, Jun. 2013. ([PDF](http://www.researchgate.net/publication/257424214_BGSLibrary_An_OpenCV_C_Background_Subtraction_Library) in brazilian-portuguese containing an english abstract).

* Sobral, Andrews; Bouwmans, Thierry. "BGS Library: A Library Framework for Algorithm’s Evaluation in Foreground/Background Segmentation". Chapter on the handbook "Background Modeling and Foreground Detection for Video Surveillance", CRC Press, Taylor and Francis Group, 2014. ([PDF](http://www.researchgate.net/publication/257424214_BGSLibrary_An_OpenCV_C_Background_Subtraction_Library) in english).

Some algorithms of the BGSLibrary were used successfully in the following papers:

* (2014) Sobral, Andrews; Vacavant, Antoine. A comprehensive review of background subtraction algorithms evaluated with synthetic and real videos. Computer Vision and Image Understanding (CVIU), 2014. ([Online](http://dx.doi.org/10.1016/j.cviu.2013.12.005)) ([PDF](http://www.researchgate.net/publication/259340906_A_comprehensive_review_of_background_subtraction_algorithms_evaluated_with_synthetic_and_real_videos))

* (2013) Sobral, Andrews; Oliveira, Luciano; Schnitman, Leizer; Souza, Felippe. (**Best Paper Award**) Highway Traffic Congestion Classification Using Holistic Properties. In International Conference on Signal Processing, Pattern Recognition and Applications (SPPRA'2013), Innsbruck, Austria, Feb 2013. ([Online](http://dx.doi.org/10.2316/P.2013.798-105)) ([PDF](http://www.researchgate.net/publication/233427564_HIGHWAY_TRAFFIC_CONGESTION_CLASSIFICATION_USING_HOLISTIC_PROPERTIES))

## Videos

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
