#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7

// opencv legacy includes
#include "OpenCvLegacyIncludes.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace multilayer
    {
      class CBackgroundSubtractionAPI
      {
      public:
        //CBackgroundSubtractionAPI(){};
        //virtual ~CBackgroundSubtractionAPI(){};

        //-------------------------------------------------------------
        // TO CALL AT INITIALISATION: DEFINES THE SIZE OF THE INPUT IMAGES
        // NORMALLY, UNNECESSARY IF A CONFIGURATION FILE IS LOADED
        void   Init(int width, int height);

        //-------------------------------------------------------------
        // PROVIDE A MASK TO DEFINE THE SET OF POINTS WHERE BACKGROUND
        // SUBTRACTION DOES NOT NEED TO BE PERFORMED
        //
        //  mode is useful to specify if the points to remove from
        //  processing are in addition to the ones potentially
        //  removed according to the configuration file,
        //  or if they are the only ones to be removed
        //
        // mode=0 : provided points need to be removed
        //          in addition to those already removed
        // mode=1 : the provided points are the only one to remove
        //          from processing
        // Note:  maskImage(li,co)=0 indicate the points to remove
        //       from background processing
        void   SetValidPointMask(IplImage* maskImage, int mode);

        //-------------------------------------------------------------
        //
        //   set the frame rate, to adjust the update parameters
        //   to the actual frame rate.
        //   Can be called only once at initialisation,
        //   but in online cases, can be used to indicate
        //   the time interval during the last processed frame
        //
        //   frameDuration is in millisecond
        void   SetFrameRate(float    frameDuration);

        //-------------------------------------------------------------
        //   PROVIDE A POINTER TO THE INPUT IMAGE
        //   -> INDICATE WHERE THE NEW IMAGE TO PROCESS IS STORED
        //
        //   Here assumes that the input image will contain RGB images.
        //   The memory of this image is handled by the caller.
        //
        //    The return value indicate whether the actual Background
        //    Subtraction algorithm handles RGB images (1) or not (0).
        //
        int  SetRGBInputImage(IplImage  *  inputImage);

        //-------------------------------------------------------------
        //   PROVIDE A POINTER TO THE RESULT IMAGE
        //   INDICATE WHERE THE BACKGROUND RESULT NEED TO BE STORED
        //
        //   The return value is 1 if correct image format is provided,
        //   otherwise the return value is 0.
        //   e.g. fg_mask_img = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
        int  SetForegroundMaskImage(IplImage *fg_mask_img);

        //   The return value is 1 if the function is implemented
        //   with correct format, otherwise the return value is 0
        //   e.g. fg_prob_img = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
        int  SetForegroundProbImage(IplImage *fg_prob_img);

        //-------------------------------------------------------------
        // This function should be called each time a new image is
        // available in the input image.
        //
        // The return value is 1 if everything goes well,
        // otherwise the return value is 0.
        //
        int   Process();

        //-------------------------------------------------------------
        // this function should save parameters and information of the model
        // (e.g. after a training of the model, or in such a way
        // that the model can be reload to process the next frame
        // type of save:
        void   Save(char   *bg_model_fn);

        //-------------------------------------------------------------
        // this function should load the parameters necessary
        // for the processing of the background subtraction or
        // load background model information
        void   Load(char  *bg_model_fn);
      };
    }
  }
}

#endif
