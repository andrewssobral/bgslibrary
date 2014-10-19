
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

#include <boost/lexical_cast.hpp>
#include <boost/chrono.hpp>
typedef boost::chrono::high_resolution_clock Clock;
typedef boost::chrono::milliseconds milliseconds;
typedef boost::chrono::microseconds microseconds;

#include <iostream>

#include <cv.h>
#include <highgui.h>

// BGSLibrary algorithms

#include "../package_bgs/FrameDifferenceBGS.h"
#include "../package_bgs/StaticFrameDifferenceBGS.h"
#include "../package_bgs/WeightedMovingMeanBGS.h"
#include "../package_bgs/WeightedMovingVarianceBGS.h"
#include "../package_bgs/MixtureOfGaussianV1BGS.h"
#include "../package_bgs/MixtureOfGaussianV2BGS.h"
#include "../package_bgs/AdaptiveBackgroundLearning.h"
#include "../package_bgs/AdaptiveSelectiveBackgroundLearning.h"
#include "../package_bgs/GMG.h"

#include "../package_bgs/dp/DPAdaptiveMedianBGS.h"
#include "../package_bgs/dp/DPGrimsonGMMBGS.h"
#include "../package_bgs/dp/DPZivkovicAGMMBGS.h"
#include "../package_bgs/dp/DPMeanBGS.h"
#include "../package_bgs/dp/DPWrenGABGS.h"
#include "../package_bgs/dp/DPPratiMediodBGS.h"
#include "../package_bgs/dp/DPEigenbackgroundBGS.h"
#include "../package_bgs/dp/DPTextureBGS.h"

#include "../package_bgs/tb/T2FGMM_UM.h"
#include "../package_bgs/tb/T2FGMM_UV.h"
#include "../package_bgs/tb/T2FMRF_UM.h"
#include "../package_bgs/tb/T2FMRF_UV.h"
#include "../package_bgs/tb/FuzzySugenoIntegral.h"
#include "../package_bgs/tb/FuzzyChoquetIntegral.h"

#include "../package_bgs/lb/LBSimpleGaussian.h"
#include "../package_bgs/lb/LBFuzzyGaussian.h"
#include "../package_bgs/lb/LBMixtureOfGaussians.h"
#include "../package_bgs/lb/LBAdaptiveSOM.h"
#include "../package_bgs/lb/LBFuzzyAdaptiveSOM.h"

#include "../package_bgs/jmo/MultiLayerBGS.h"
// The PBAS algorithm was removed from BGSLibrary because it is based on patented algorithm ViBE
// #include "../package_bgs/pt/PixelBasedAdaptiveSegmenter.h"
#include "../package_bgs/av/VuMeter.h"
#include "../package_bgs/ae/KDE.h"
#include "../package_bgs/db/IndependentMultimodalBGS.h"
#include "../package_bgs/sjn/SJN_MultiCueBGS.h"
#include "../package_bgs/bl/SigmaDeltaBGS.h"

#include "../package_bgs/pl/SuBSENSE.h"
#include "../package_bgs/pl/LOBSTER.h"
