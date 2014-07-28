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
/* --- --- ---
* Copyright (C) 2008--2010 Idiap Research Institute (.....@idiap.ch)
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. The name of the author may not be used to endorse or promote products
*    derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
//////////////////////////////////////////////////////////////////////
//
// BackgroundSubtractionAPI.h: 
//   interface for the BackgroundSubtractionAPI class.
//
// A background subtraction algorithm takes as input
// an RGB image and provide as ouput a Binary mask
// with a value of 0 for points belonging to the 
// background, and non zero for points belonging
// to the foreground.
//
//
//
// To add:
//  - a function indicating the valid input and ouput
//    images 
//     e.g. RGB  image (default) or greylevel image for the input
//          char image for the output
//
//////////////////////////////////////////////////////////////////////


#if !defined(_BACKGROUND_SUBTRACTION_API_H_)
#define _BACKGROUND_SUBTRACTION_API_H_

#include <opencv2/opencv.hpp>

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

#endif // !defined(_BACKGROUND_SUBTRACTION_API_H_)
