#pragma once

#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
// opencv legacy includes
#ifndef MEX_COMPILE_FLAG
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#endif
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>

namespace bgslibrary
{
  namespace algorithms
  {
    namespace imbs
    {
      class BackgroundSubtractorIMBS
      {
      public:
        //! the default constructor
        BackgroundSubtractorIMBS();
        //! the full constructor
        BackgroundSubtractorIMBS(double fps,
          unsigned int fgThreshold = 15,
          unsigned int associationThreshold = 5,
          double samplingPeriod = 50.,
          unsigned int minBinHeight = 2,
          unsigned int numSamples = 10,
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
        void apply(cv::InputArray image, cv::OutputArray fgmask, double learningRate = -1.);

        //! computes a background image which shows only the highest bin for each pixel
        void getBackgroundImage(cv::OutputArray backgroundImage) const;

        //! re-initiaization method
        void initialize(cv::Size frameSize, int frameType);

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
        cv::Mat convertImageRGBtoHSV(const cv::Mat& imageRGB);
        //method for changing the bg in case of sudden changes
        void changeBg();

        //current input RGB frame
        cv::Mat frame;
        std::vector<cv::Mat> frameBGR;
        //frame size
        cv::Size frameSize;
        //frame type
        int frameType;
        //total number of pixels in frame
        unsigned int numPixels;
        //current background sample
        cv::Mat bgSample;
        std::vector<cv::Mat> bgSampleBGR;
        //current background image which shows only the highest bin for each pixel
        //(just for displaying purposes)
        cv::Mat bgImage;
        //current foreground mask
        cv::Mat fgmask;
        cv::Mat fgfiltered;
        //number of fps
        double fps;
        //time stamp in milliseconds (ms)
        double timestamp;
        //previous time stamp in milliseconds (ms)
        double prev_timestamp;
        double initial_tick_count;
        //initial message to be shown until the first bg model is ready
        cv::Mat initialMsgGray;
        cv::Mat initialMsgRGB;

        //struct for modeling the background values for a single pixel
        typedef struct Bins {
          void initialize(unsigned int numSamples) {
            binValues = new cv::Vec3b[numSamples];
            binHeights = new uchar[numSamples];
            isFg = new bool[numSamples];
          }
          ~Bins() {
            if (binValues)  { delete[] binValues; }
            if (binHeights) { delete[] binHeights; }
            if (isFg)       { delete[] isFg; }
          }
          cv::Vec3b* binValues;
          uchar* binHeights;
          bool* isFg;
        } Bins;
        Bins* bgBins;

      public:
        //struct for modeling the background values for the entire frame
        typedef struct BgModel {
          void initialize(unsigned int maxBgBins) {
            values = new cv::Vec3b[maxBgBins];
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
          cv::Vec3b* values;
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
        cv::Mat persistenceImage;

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
    }
  }
}
