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

#include "imbs.hpp"

using namespace std;
using namespace cv;

BackgroundSubtractorIMBS::BackgroundSubtractorIMBS()
{
  fps = 0.;
  fgThreshold = 15;
  associationThreshold = 5;
  samplingPeriod = 250.;//500.ms
  minBinHeight = 2;
  numSamples = 10; //30
  alpha = 0.65f;
  beta = 1.15f;
  tau_s = 60.;
  tau_h = 40.;
  minArea = 30.;
  persistencePeriod = samplingPeriod*numSamples/3.;//ms

  initial_tick_count = (double)getTickCount();

  //morphological Opening and closing
  morphologicalFiltering = false;
}

BackgroundSubtractorIMBS::BackgroundSubtractorIMBS(
  double fps,
  unsigned int fgThreshold,
  unsigned int associationThreshold,
  double samplingPeriod,
  unsigned int minBinHeight,
  unsigned int numSamples,
  double alpha,
  double beta,
  double tau_s,
  double tau_h,
  double minArea,
  double persistencePeriod,
  bool morphologicalFiltering)
{
  this->fps = fps;
  this->fgThreshold = fgThreshold;
  this->persistencePeriod = persistencePeriod;
  if(minBinHeight <= 1){
    this->minBinHeight = 1;
  }
  else {
    this->minBinHeight = minBinHeight;
  }
  this->associationThreshold = associationThreshold;
  this->samplingPeriod = samplingPeriod;//ms
  this->minBinHeight = minBinHeight;
  this->numSamples = numSamples;
  this->alpha = alpha;
  this->beta = beta;
  this->tau_s = tau_s;
  this->tau_h = tau_h;
  this->minArea = minArea;

  if(fps == 0.)
    initial_tick_count = (double)getTickCount();
  else
    initial_tick_count = 0;

  //morphological Opening and closing
  this->morphologicalFiltering = morphologicalFiltering;
}

BackgroundSubtractorIMBS::~BackgroundSubtractorIMBS()
{
  delete[] bgBins;
  delete[] bgModel;
  delete[] persistenceMap;
}

void BackgroundSubtractorIMBS::initialize(Size frameSize, int frameType)
{
  /*cout << "INPUT: WIDTH " << frameSize.width << "  HEIGHT " << frameSize.height <<
    "  FPS " << fps << endl;
  cout << endl;*/

  this->frameSize = frameSize;
  this->frameType = frameType;
  this->numPixels = frameSize.width*frameSize.height;

  persistenceMap = new unsigned int[numPixels];
  for(unsigned int i = 0; i < numPixels; i++) {
    persistenceMap[i] = 0;
  }

  bgBins = new Bins[numPixels];
  bgModel = new BgModel[numPixels];
  maxBgBins = numSamples / minBinHeight;

  timestamp = 0.;//ms
  prev_timestamp = 0.;//ms
  prev_bg_frame_time = 0;
  bg_frame_counter = 0;
  bg_reset = false;
  prev_area = 0;
  sudden_change = false;

  SHADOW_LABEL = 80;
  PERSISTENCE_LABEL = 180;
  FOREGROUND_LABEL = 255;

  fgmask.create(frameSize, CV_8UC1);
  fgfiltered.create(frameSize, CV_8UC1);
  persistenceImage = Mat::zeros(frameSize, CV_8UC1);
  bgSample.create(frameSize, CV_8UC3);
  bgImage = Mat::zeros(frameSize, CV_8UC3);

  //initial message to be shown until the first fg mask is computed
  initialMsgGray = Mat::zeros(frameSize, CV_8UC1);
  putText(initialMsgGray, "Creating", Point(10,20), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));
  putText(initialMsgGray, "initial", Point(10,40), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));
  putText(initialMsgGray, "background...", Point(10,60), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));

  initialMsgRGB = Mat::zeros(frameSize, CV_8UC3);
  putText(initialMsgRGB, "Creating", Point(10,20), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));
  putText(initialMsgRGB, "initial", Point(10,40), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));
  putText(initialMsgRGB, "background...", Point(10,60), FONT_HERSHEY_SIMPLEX, 0.4, CV_RGB(255, 255, 255));

  if(minBinHeight <= 1){
    minBinHeight = 1;
  }

  for(unsigned int p = 0; p < numPixels; ++p)
  {
    bgBins[p].binValues = new Vec3b[numSamples];
    bgBins[p].binHeights = new uchar[numSamples];
    bgBins[p].isFg = new bool[numSamples];

    bgModel[p].values = new Vec3b[maxBgBins];
    bgModel[p].isValid = new bool[maxBgBins];
    bgModel[p].isValid[0] = false;
    bgModel[p].isFg = new bool[maxBgBins];
    bgModel[p].counter = new uchar[maxBgBins];
  }
}

void BackgroundSubtractorIMBS::apply(InputArray _frame, OutputArray _fgmask, double learningRate)
{
  frame = _frame.getMat();

  CV_Assert(frame.depth() == CV_8U);
  CV_Assert(frame.channels() == 3);

  bool needToInitialize = nframes == 0 || frame.type() != frameType;
  if( needToInitialize ) {
    initialize(frame.size(), frame.type());
  }

  _fgmask.create(frameSize, CV_8UC1);
  fgmask = _fgmask.getMat();
  fgmask = Scalar(0);

  //get current time
  prev_timestamp = timestamp;
  if(fps == 0.) {
    timestamp = getTimestamp();//ms
  }
  else {
    timestamp += 1000./fps;//ms
  }

  //check for global changes
  if(sudden_change) {
    changeBg();
  }

  //wait for the first model to be generated
  if(bgModel[0].isValid[0]) {
    getFg();    	
    hsvSuppression();
    filterFg();
  }	
  //update the bg model
  updateBg();

  //show an initial message if the first bg is not yet ready
  if(!bgModel[0].isValid[0]) {
    initialMsgGray.copyTo(fgmask);
    initialMsgRGB.copyTo(bgImage);
  }
  ++nframes;
}

void BackgroundSubtractorIMBS::updateBg() {
  if(bg_reset) {
    if(bg_frame_counter > numSamples - 1) {
      bg_frame_counter = numSamples - 1;
    }
  }

  if(prev_bg_frame_time > timestamp) {
    prev_bg_frame_time = timestamp;
  }

  if(bg_frame_counter == numSamples - 1) {
    createBg(bg_frame_counter);
    bg_frame_counter = 0;
  }
  else { //bg_frame_counter < (numSamples - 1)

    if((timestamp - prev_bg_frame_time) >= samplingPeriod)
    {
      //get a new sample for creating the bg model
      prev_bg_frame_time = timestamp;
      frame.copyTo(bgSample);
      createBg(bg_frame_counter);
      bg_frame_counter++;
    }
  }
}

double BackgroundSubtractorIMBS::getTimestamp() {
  return ((double)getTickCount() - initial_tick_count)*1000./getTickFrequency();
}

void BackgroundSubtractorIMBS::hsvSuppression() {

  uchar h_i, s_i, v_i;
  uchar h_b, s_b, v_b;
  float h_diff, s_diff, v_ratio;

  Mat bgrPixel(cv::Size(1, 1), CV_8UC3);

  vector<Mat> imHSV;
  cv::split(convertImageRGBtoHSV(frame), imHSV);

  for(unsigned int p = 0; p < numPixels; ++p) {
    if(fgmask.data[p]) {

      h_i = imHSV[0].data[p];
      s_i = imHSV[1].data[p];
      v_i = imHSV[2].data[p];

      for(unsigned int n = 0; n < maxBgBins; ++n) {
        if(!bgModel[p].isValid[n]) {
          break;
        }

        if(bgModel[p].isFg[n]) {
          continue;
        }

        bgrPixel.at<cv::Vec3b>(0,0) = bgModel[p].values[n];

        cv::Mat hsvPixel = convertImageRGBtoHSV(bgrPixel);

        h_b = hsvPixel.at<cv::Vec3b>(0,0)[0];
        s_b = hsvPixel.at<cv::Vec3b>(0,0)[1];
        v_b = hsvPixel.at<cv::Vec3b>(0,0)[2];

        v_ratio = (float)v_i / (float)v_b;
        s_diff = std::abs(s_i - s_b);
        h_diff = std::min( std::abs(h_i - h_b), 255 - std::abs(h_i - h_b));

        if(	h_diff <= tau_h &&
          s_diff <= tau_s &&
          v_ratio >= alpha &&
          v_ratio < beta)
        {
          fgmask.data[p] = SHADOW_LABEL;
          break;
        }
      }//for
    }//if
  }//numPixels
}

void BackgroundSubtractorIMBS::createBg(unsigned int bg_sample_number) {
  if(!bgSample.data) {
    //cerr << "createBg -- an error occurred: " <<
    //		" unable to retrieve frame no. " << bg_sample_number << endl;

    //TODO vedere gestione errori
    abort();
  }
  //aux variable
  Vec3b currentPixel;
  //split bgSample in channels
  cv::split(bgSample, bgSampleBGR);
  //create a statistical model for each pixel (a set of bins of variable size)
  for(unsigned int p = 0; p < numPixels; ++p) {
    //create an initial bin for each pixel from the first sample (bg_sample_number = 0)
    if(bg_sample_number == 0) {
      for(int k = 0; k < 3; ++k) {
        bgBins[p].binValues[0][k] = bgSampleBGR[k].data[p];
      }
      bgBins[p].binHeights[0] = 1;
      for(unsigned int s = 1; s < numSamples; ++s)	{
        bgBins[p].binHeights[s] = 0;
      }
      //if the sample pixel is from foreground keep track of that situation
      if(fgmask.data[p] == FOREGROUND_LABEL) {
        bgBins[p].isFg[0] = true;
      }
      else {
        bgBins[p].isFg[0] = false;
      }
    }//if(bg_sample_number == 0)
    else { //bg_sample_number > 0
      for(int k = 0; k < 3; ++k) {
        currentPixel[k] = bgSampleBGR[k].data[p];
      }
      int den = 0;
      for(unsigned int s = 0; s < bg_sample_number; ++s) {
        //try to associate the current pixel values to an existing bin
        if( std::abs(currentPixel[2] - bgBins[p].binValues[s][2]) <= associationThreshold &&
          std::abs(currentPixel[1] - bgBins[p].binValues[s][1]) <= associationThreshold &&
          std::abs(currentPixel[0] - bgBins[p].binValues[s][0]) <= associationThreshold )
        {
          den = (bgBins[p].binHeights[s] + 1);
          for(int k = 0; k < 3; ++k) {
            bgBins[p].binValues[s][k] =
              (bgBins[p].binValues[s][k] * bgBins[p].binHeights[s] + currentPixel[k]) / den;
          }
          bgBins[p].binHeights[s]++; //increment the height of the bin
          if(fgmask.data[p] == FOREGROUND_LABEL) {
            bgBins[p].isFg[s] = true;
          }
          break;
        }
        //if the association is not possible, create a new bin
        else if(bgBins[p].binHeights[s] == 0)	{
          bgBins[p].binValues[s] = currentPixel;
          bgBins[p].binHeights[s]++;
          if(fgmask.data[p] == FOREGROUND_LABEL) {
            bgBins[p].isFg[s] = true;
          }
          else {
            bgBins[p].isFg[s] = false;
          }
          break;
        }
        else continue;
      }//for(unsigned int s = 0; s <= bg_sample_number; ++s)

      //if all samples have been processed
      //it is time to compute the fg mask
      if(bg_sample_number == (numSamples - 1)) {
        unsigned int index = 0;
        int max_height = -1;
        for(unsigned int s = 0; s < numSamples; ++s){
          if(bgBins[p].binHeights[s] == 0) {
            bgModel[p].isValid[index] = false;
            break;
          }
          if(index == maxBgBins) {
            break;
          }
          else if(bgBins[p].binHeights[s] >= minBinHeight) {
            if(fgmask.data[p] == PERSISTENCE_LABEL) {
              for(unsigned int n = 0; n < maxBgBins; n++) {
                if(!bgModel[p].isValid[n]) {
                  break;
                }
                unsigned int d = std::max((int)std::abs(bgModel[p].values[n][0] - bgBins[p].binValues[s][0]),
                  std::abs(bgModel[p].values[n][1] - bgBins[p].binValues[s][1]) );
                d = std::max((int)d, std::abs(bgModel[p].values[n][2] - bgBins[p].binValues[s][2]) );
                if(d < fgThreshold){
                  bgModel[p].isFg[n] = false;
                  bgBins[p].isFg[s] = false;
                }
              }
            }

            if(bgBins[p].binHeights[s] > max_height) {
              max_height = bgBins[p].binHeights[s];

              for(int k = 0; k < 3; ++k) {
                bgModel[p].values[index][k] = bgModel[p].values[0][k];
              }
              bgModel[p].isValid[index] = true;
              bgModel[p].isFg[index] = bgModel[p].isFg[0];
              bgModel[p].counter[index] = bgModel[p].counter[0];

              for(int k = 0; k < 3; ++k) {
                bgModel[p].values[0][k] = bgBins[p].binValues[s][k];
              }
              bgModel[p].isValid[0] = true;
              bgModel[p].isFg[0] = bgBins[p].isFg[s];
              bgModel[p].counter[0] = bgBins[p].binHeights[s];
            }
            else {
              for(int k = 0; k < 3; ++k) {
                bgModel[p].values[index][k] = bgBins[p].binValues[s][k];
              }
              bgModel[p].isValid[index] = true;
              bgModel[p].isFg[index] = bgBins[p].isFg[s];
              bgModel[p].counter[index] = bgBins[p].binHeights[s];
            }
            ++index;
          }
        } //for all numSamples
      }//bg_sample_number == (numSamples - 1)
    }//else --> if(frame_number == 0)
  }//numPixels

  if(bg_sample_number == (numSamples - 1)) {
    //std::cout << "new bg created" << std::endl;		
    persistenceImage = Scalar(0);

    bg_reset = false;
    if(sudden_change) {
      numSamples *= 3.;
      samplingPeriod *= 2.;
      sudden_change = false;
    }

    for(unsigned int i = 0; i < numPixels; i++) {
      persistenceMap[i] = 0;
    }

    unsigned int p = 0;
    for(int i = 0; i < bgImage.rows; ++i) {
      for(int j = 0; j < bgImage.cols; ++j, ++p) {
        bgImage.at<cv::Vec3b>(i,j) = bgModel[p].values[0];
      }
    }
  }
}

void BackgroundSubtractorIMBS::getFg() {
  fgmask = Scalar(0);
  cv::split(frame, frameBGR);

  bool isFg = true;
  bool conditionalUpdated = false;
  unsigned int d = 0;
  for(unsigned int p = 0; p < numPixels; ++p) {
    isFg = true;
    conditionalUpdated = false;
    d = 0;
    for(unsigned int n = 0; n < maxBgBins; ++n) {
      if(!bgModel[p].isValid[n]) {
        if(n == 0) {
          isFg = false;
        }
        break;
      }
      else { //the model is valid
        d = std::max(
          (int)std::abs(bgModel[p].values[n][0] - frameBGR[0].data[p]),
          std::abs(bgModel[p].values[n][1] - frameBGR[1].data[p]) );
        d = std::max(
          (int)d, std::abs(bgModel[p].values[n][2] - frameBGR[2].data[p]) );
        if(d < fgThreshold){
          //check if it is a potential background pixel
          //from stationary object
          if(bgModel[p].isFg[n]) {
            conditionalUpdated = true;
            break;
          }
          else {
            isFg = false;
            persistenceMap[p] = 0;
          }
        }
      }
    }
    if(isFg) {
      if(conditionalUpdated) {
        fgmask.data[p] = PERSISTENCE_LABEL;
        persistenceMap[p] += (timestamp - prev_timestamp);
        if(persistenceMap[p] > persistencePeriod) {
          for(unsigned int n = 0; n < maxBgBins; ++n) {
            if(!bgModel[p].isValid[n]) {
              break;
            }
            bgModel[p].isFg[n] = false;
          }
        }
      }
      else {
        fgmask.data[p] = FOREGROUND_LABEL;
        persistenceMap[p] = 0;
      }
    }
  }
}

void BackgroundSubtractorIMBS::areaThresholding()
{
  double maxArea = 0.6 * numPixels;

  std::vector < std::vector<Point> > contours;
  Mat tmpBinaryImage = fgfiltered.clone();
  findContours(tmpBinaryImage, contours, RETR_LIST, CHAIN_APPROX_NONE);

  tmpBinaryImage = Scalar(0);

  for (size_t contourIdx = 0; contourIdx < contours.size(); ++contourIdx)
  {
    Moments moms = moments(Mat(contours[contourIdx]));
    double area = moms.m00;
    if (area < minArea || area >= maxArea)
      continue;
    else {
      drawContours( tmpBinaryImage, contours, contourIdx, Scalar(255), CV_FILLED );
    }
  }	
  for(int i = 0; i < fgfiltered.rows; ++i) {
    for(int j = 0; j < fgfiltered.cols; ++j) {
      if(!tmpBinaryImage.at<uchar>(i,j)) {
        fgfiltered.at<uchar>(i,j) = 0;
      }
    }
  }
}

// Create a HSV image from the RGB image using the full 8-bits, since OpenCV only allows Hues up to 180 instead of 255.
// ref: "http://cs.haifa.ac.il/hagit/courses/ist/Lectures/Demos/ColorApplet2/t_convert.html"
// Remember to free the generated HSV image.
Mat BackgroundSubtractorIMBS::convertImageRGBtoHSV(const Mat& imageRGB)
{
  float fR, fG, fB;
  float fH, fS, fV;
  const float FLOAT_TO_BYTE = 255.0f;
  const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;

  // Create a blank HSV image
  Mat imageHSV(imageRGB.size(), CV_8UC3);
  //if (!imageHSV || imageRGB->depth != 8 || imageRGB->nChannels != 3) {
  //printf("ERROR in convertImageRGBtoHSV()! Bad input image.\n");
  //exit(1);
  //}

  int h = imageRGB.rows;		// Pixel height.
  int w = imageRGB.cols;		// Pixel width.
  //int rowSizeRGB = imageRGB->widthStep;	// Size of row in bytes, including extra padding.
  //char *imRGB = imageRGB->imageData;	// Pointer to the start of the image pixels.
  //int rowSizeHSV = imageHSV->widthStep;	// Size of row in bytes, including extra padding.
  //char *imHSV = imageHSV->imageData;	// Pointer to the start of the image pixels.
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      // Get the RGB pixel components. NOTE that OpenCV stores RGB pixels in B,G,R order.
      //uchar *pRGB = (uchar*)(imRGB + y*rowSizeRGB + x*3);
      int bB = imageRGB.at<Vec3b>(y,x)[0]; //*(uchar*)(pRGB+0);	// Blue component
      int bG = imageRGB.at<Vec3b>(y,x)[1]; //*(uchar*)(pRGB+1);	// Green component
      int bR = imageRGB.at<Vec3b>(y,x)[2]; //*(uchar*)(pRGB+2);	// Red component

      // Convert from 8-bit integers to floats.
      fR = bR * BYTE_TO_FLOAT;
      fG = bG * BYTE_TO_FLOAT;
      fB = bB * BYTE_TO_FLOAT;

      // Convert from RGB to HSV, using float ranges 0.0 to 1.0.
      float fDelta;
      float fMin, fMax;
      int iMax;
      // Get the min and max, but use integer comparisons for slight speedup.
      if (bB < bG) {
        if (bB < bR) {
          fMin = fB;
          if (bR > bG) {
            iMax = bR;
            fMax = fR;
          }
          else {
            iMax = bG;
            fMax = fG;
          }
        }
        else {
          fMin = fR;
          fMax = fG;
          iMax = bG;
        }
      }
      else {
        if (bG < bR) {
          fMin = fG;
          if (bB > bR) {
            fMax = fB;
            iMax = bB;
          }
          else {
            fMax = fR;
            iMax = bR;
          }
        }
        else {
          fMin = fR;
          fMax = fB;
          iMax = bB;
        }
      }
      fDelta = fMax - fMin;
      fV = fMax;				// Value (Brightness).
      if (iMax != 0) {			// Make sure its not pure black.
        fS = fDelta / fMax;		// Saturation.
        float ANGLE_TO_UNIT = 1.0f / (6.0f * fDelta);	// Make the Hues between 0.0 to 1.0 instead of 6.0
        if (iMax == bR) {		// between yellow and magenta.
          fH = (fG - fB) * ANGLE_TO_UNIT;
        }
        else if (iMax == bG) {		// between cyan and yellow.
          fH = (2.0f/6.0f) + ( fB - fR ) * ANGLE_TO_UNIT;
        }
        else {				// between magenta and cyan.
          fH = (4.0f/6.0f) + ( fR - fG ) * ANGLE_TO_UNIT;
        }
        // Wrap outlier Hues around the circle.
        if (fH < 0.0f)
          fH += 1.0f;
        if (fH >= 1.0f)
          fH -= 1.0f;
      }
      else {
        // color is pure Black.
        fS = 0;
        fH = 0;	// undefined hue
      }

      // Convert from floats to 8-bit integers.
      int bH = (int)(0.5f + fH * 255.0f);
      int bS = (int)(0.5f + fS * 255.0f);
      int bV = (int)(0.5f + fV * 255.0f);

      // Clip the values to make sure it fits within the 8bits.
      if (bH > 255)
        bH = 255;
      if (bH < 0)
        bH = 0;
      if (bS > 255)
        bS = 255;
      if (bS < 0)
        bS = 0;
      if (bV > 255)
        bV = 255;
      if (bV < 0)
        bV = 0;

      // Set the HSV pixel components.
      imageHSV.at<Vec3b>(y, x)[0] = bH;		// H component
      imageHSV.at<Vec3b>(y, x)[1] = bS;		// S component
      imageHSV.at<Vec3b>(y, x)[2] = bV;		// V component
    }
  }
  return imageHSV;
}

void BackgroundSubtractorIMBS::getBackgroundImage(OutputArray backgroundImage) const
{
  bgImage.copyTo(backgroundImage);        
}

void BackgroundSubtractorIMBS::filterFg() {

  unsigned int cnt = 0;
  for(unsigned int p = 0; p < numPixels; ++p) {
    if(fgmask.data[p] == (uchar)255) {
      fgfiltered.data[p] = 255;
      cnt++;
    }
    else {
      fgfiltered.data[p] = 0;
    }
  }

  if(cnt > numPixels*0.5) {
    sudden_change = true;
  }

  if(morphologicalFiltering) {
    cv::Mat element3(3,3,CV_8U,cv::Scalar(1));
    cv::morphologyEx(fgfiltered, fgfiltered, cv::MORPH_OPEN, element3);
    cv::morphologyEx(fgfiltered, fgfiltered, cv::MORPH_CLOSE, element3);
  }

  areaThresholding();

  for(unsigned int p = 0; p < numPixels; ++p) {
    if(fgmask.data[p] == PERSISTENCE_LABEL) {
      fgfiltered.data[p] = PERSISTENCE_LABEL;
    }
    else if(fgmask.data[p] == SHADOW_LABEL) {
      fgfiltered.data[p] = SHADOW_LABEL;
    }
  }

  fgfiltered.copyTo(fgmask);
}

void BackgroundSubtractorIMBS::changeBg() {

  std::cout << "\n\n\n\nWARNING: changeBg\n\n\n\n\n" << std::endl;

  //samplingPeriod /= 2.;
  //numSamples /= 2.;
  //bg_reset = true;
  //cout << "qua" << endl;

  if(!bg_reset) {
    numSamples /= 3.;
    samplingPeriod /= 2.;
    bg_frame_counter = 0;
    bg_reset = true;
  }
}

void BackgroundSubtractorIMBS::getBgModel(BgModel bgModel_copy[], int size) {
  if(size != numPixels) {
    return;
  }
  for(unsigned int i = 0; i < numPixels; ++i){
    bgModel_copy[i].values = new Vec3b[maxBgBins];
    bgModel_copy[i].isValid = new bool[maxBgBins];
    bgModel_copy[i].isValid[0] = false;
    bgModel_copy[i].isFg = new bool[maxBgBins];
    bgModel_copy[i].counter = new uchar[maxBgBins];
  }
  for(unsigned int p = 0; p < numPixels; ++p) {
    for(unsigned int n = 0; n < maxBgBins; ++n) {
      if(!bgModel[p].isValid[n]) {
        break;
      }
      bgModel_copy[p].values[n] = bgModel[p].values[n];
      bgModel_copy[p].isValid[n] = bgModel[p].isValid[n];
      bgModel_copy[p].isFg[n] = bgModel[p].isFg[n];
      bgModel_copy[p].counter[n] = bgModel[p].counter[n];
    }
  }
}
