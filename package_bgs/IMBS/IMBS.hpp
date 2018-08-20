/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
*  IMBS Background Subtraction Library
*
*  This file imbs.hpp contains the C++ OpenCV based implementation for
*  IMBS algorithm described in
*  D. D. Bloisi and L. Iocchi
*  "Independent Multimodal Background Subtraction"
*  In Proc. of the Third Int. Conf. on Computational Modeling of Objects
*  Presented in Images: Fundamentals, Methods and Applications, pp. 39-44, 2012.
*  Please, cite the above paper if you use IMBS.
*
*  This software is provided without any warranty about its usability.
*  It is for educational purposes and should be regarded as such.
*
*  Written by Domenico D. Bloisi
*
*  Please, report suggestions/comments/bugs to
*  domenico.bloisi@gmail.com
*
*/
#pragma once

//OPENCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>
//C++
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

class BackgroundSubtractorIMBS
{
public:
  //! the default constructor
  BackgroundSubtractorIMBS();
  //! the full constructor
  BackgroundSubtractorIMBS(double fps,
    unsigned int fgThreshold = 15,
    unsigned int associationThreshold = 5,
    double samplingPeriod = 500.,
    unsigned int minBinHeight = 2,
    unsigned int numSamples = 30,
    double alpha = 0.65,
    double beta = 1.15,
    double tau_s = 60.,
    double tau_h = 40.,
    double minArea = 30.,
    double persistencePeriod = 10000.,
    bool morphologicalFiltering = false
  );
  //! the destructor
  ~BackgroundSubtractorIMBS();
  //! the update operator
  void apply(InputArray image, OutputArray fgmask, double learningRate = -1.);

  //! computes a background image which shows only the highest bin for each pixel
  void getBackgroundImage(OutputArray backgroundImage) const;

  //! re-initiaization method
  void initialize(Size frameSize, int frameType);

private:
  //method for creating the background model
  void createBg(unsigned int bg_sample_number);
  //method for updating the background model
  void updateBg();
  //method for computing the foreground mask
  void getFg();
  //method for suppressing shadows and highlights
  void hsvSuppression();
  //method for refining foreground mask
  void filterFg();
  //method for filtering out blobs smaller than a given area
  void areaThresholding();
  //method for getting the current time
  double getTimestamp();
  //method for converting from RGB to HSV
  Mat convertImageRGBtoHSV(const Mat& imageRGB);
  //method for changing the bg in case of sudden changes
  void changeBg();

  //current input RGB frame
  Mat frame;
  vector<Mat> frameBGR;
  //frame size
  Size frameSize;
  //frame type
  int frameType;
  //total number of pixels in frame
  unsigned int numPixels;
  //current background sample
  Mat bgSample;
  vector<Mat> bgSampleBGR;
  //current background image which shows only the highest bin for each pixel
  //(just for displaying purposes)
  Mat bgImage;
  //current foreground mask
  Mat fgmask;

  Mat fgfiltered;

  //number of fps
  double fps;
  //time stamp in milliseconds (ms)
  double timestamp;
  //previous time stamp in milliseconds (ms)
  double prev_timestamp;
  double initial_tick_count;
  //initial message to be shown until the first bg model is ready
  Mat initialMsgGray;
  Mat initialMsgRGB;

  //struct for modeling the background values for a single pixel
  typedef struct Bins {
    void initialize(unsigned int numSamples) {
      binValues = new Vec3b[numSamples];
      binHeights = new uchar[numSamples];
      isFg = new bool[numSamples];
    }
    ~Bins() {
      if (binValues)  { delete[] binValues; }
      if (binHeights) { delete[] binHeights; }
      if (isFg)       { delete[] isFg; }
    }
    Vec3b* binValues;
    uchar* binHeights;
    bool* isFg;
  } Bins;
  Bins* bgBins;

public:
  //struct for modeling the background values for the entire frame
  typedef struct BgModel {
    void initialize(unsigned int maxBgBins) {
      values = new Vec3b[maxBgBins];
      isValid = new bool[maxBgBins];
      isValid[0] = false;
      isFg = new bool[maxBgBins];
      counter = new uchar[maxBgBins];
    }
    ~BgModel() {
      if (values)  { delete[] values; }
      if (isValid) { delete[] isValid; }
      if (isFg)    { delete[] isFg; }
      if (counter) { delete[] counter; }
    }
    Vec3b* values;
    bool* isValid;
    bool* isFg;
    uchar* counter;
  } BgModel;
private:
  BgModel* bgModel;

  //SHADOW SUPPRESSION PARAMETERS
  float alpha;
  float beta;
  uchar tau_s;
  uchar tau_h;

  unsigned int minBinHeight;
  unsigned int numSamples;
  unsigned int samplingPeriod;
  unsigned long prev_bg_frame_time;
  unsigned int bg_frame_counter;
  unsigned int associationThreshold;
  unsigned int maxBgBins;
  unsigned int nframes;

  double minArea;
  bool bg_reset;
  unsigned int persistencePeriod;
  bool prev_area;
  bool sudden_change;
  unsigned int fgThreshold;
  uchar SHADOW_LABEL;
  uchar PERSISTENCE_LABEL;
  uchar FOREGROUND_LABEL;
  //persistence map
  unsigned int* persistenceMap;
  Mat persistenceImage;

  bool morphologicalFiltering;

public:
  unsigned int getMaxBgBins() {
    return maxBgBins;
  }
  unsigned int getFgThreshold() {
    return fgThreshold;
  }
  void getBgModel(BgModel bgModel_copy[], unsigned int size);
};
