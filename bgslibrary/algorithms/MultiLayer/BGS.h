#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

// opencv legacy includes
#include "OpenCvLegacyIncludes.h"

#define BINARY_PATTERM_ELEM(c1, c2, offset)	\
  ((float)(c2)-(float)(c1)+offset > 0)

/*
#define BINARY_PATTERM_ELEM(c1, c2, offset)	\
( fabsf((float)(c2)-(float)(c1)) <= offset ? 1 : (float)(c2)-(float)(c1) >=0 )
*/

#ifndef PI
#define PI 3.141592653589793f
#endif

namespace bgslibrary
{
  namespace algorithms
  {
    namespace multilayer
    {
      const int MAX_LBP_MODE_NUM = 5;
      const float ROBUST_COLOR_OFFSET = 6.0f;
      const float LOW_INITIAL_MODE_WEIGHT = 0.01f;
      const float MODE_UPDATING_LEARN_RATE = 0.01f;
      const float WEIGHT_UPDATING_LEARN_RATE = 0.01f;
      const int COLOR_MAX_MIN_OFFSET = 5;
      const float BACKGROUND_MODEL_PERCENT = 0.6f;
      const float PATTERN_COLOR_DIST_BACKGROUND_THRESHOLD = 0.2f;
      const float PATTERN_DIST_SMOOTH_NEIG_HALF_SIZE = 6;
      const float PATTERN_DIST_CONV_GAUSSIAN_SIGMA = 2.5f;
      const float ROBUST_SHADOW_RATE = 0.6f;
      const float ROBUST_HIGHLIGHT_RATE = 1.20f;

      /************************************************************************/
      /* some data structures for multi-level LBP (local binary pattern)      */
      /* texture feature for background subtraction                           */
      /************************************************************************/
      typedef struct _LBP
      {
        float* bg_pattern;			/* the average local binary pattern of background mode */
        float* bg_intensity;			/* the average color intensity of background mode */
        float* max_intensity;			/* the maximal color intensity of background mode */
        float* min_intensity;			/* the minimal color intensity of background mode */
        float weight;				/* the weight of background mode, i.e. probability that the background mode belongs to background */
        float max_weight;			/* the maximal weight of background mode */
        int bg_layer_num;			/* the background layer number of background mode */
        unsigned long first_time;				/* the first time of background mode appearing */
        unsigned long last_time;				/* the last time of background model appearing */
        int freq;				/* the appearing frequency */
        //long mnrl;				/* maximum negative run-length */
        unsigned long layer_time;				/* the first time of background mode becoming a background layer */
      }
      LBPStruct;

      typedef struct _PixelLBP
      {
        LBPStruct* LBPs;			/* the background modes */
        unsigned short* lbp_idxes;		/* the indices of background modes */
        unsigned int cur_bg_layer_no;
        unsigned int num;			/* the total number of background modes */
        unsigned int bg_num;			/* the number of the first background modes for foreground detection */
        unsigned char* cur_intensity;		/* the color intensity of current pixel */
        float* cur_pattern;			/* the local binary pattern of current pixel */
        float matched_mode_first_time;		/* the index of currently matched pixel mode */
      }
      PixelLBPStruct;

      /*********************************************************************************/
      /* should replace the above structure using class in the future (not finished)   */
      /*********************************************************************************/

      class BG_PIXEL_MODE
      {
      public:
        float* bg_lbp_pattern;			/* the average local binary pattern of background mode */
        float* bg_intensity;			/* the average color intensity of background mode */
        float* max_intensity;			/* the maximal color intensity of background mode */
        float* min_intensity;			/* the minimal color intensity of background mode */
        float weight;				/* the weight of background mode, i.e. probability that the background mode belongs to background */
        float max_weight;			/* the maximal weight of background mode */
        int bg_layer_num;			/* the background layer number of background mode */

        int lbp_pattern_length;
        int color_channel;

        BG_PIXEL_MODE(int _lbp_pattern_length, int _color_channel = 3) {
          lbp_pattern_length = _lbp_pattern_length;
          color_channel = _color_channel;

          bg_lbp_pattern = new float[lbp_pattern_length];
          bg_intensity = new float[color_channel];
          max_intensity = new float[color_channel];
          min_intensity = new float[color_channel];
        };

        virtual ~BG_PIXEL_MODE() {
          delete[] bg_lbp_pattern;
          delete[] bg_intensity;
          delete[] max_intensity;
          delete[] min_intensity;
        };
      };

      class BG_PIXEL_PATTERN
      {
      public:
        BG_PIXEL_MODE** pixel_MODEs;		/* the background modes */
        unsigned short* lbp_pattern_idxes;	/* the indices of background modes */
        unsigned int cur_bg_layer_no;
        unsigned int num;			/* the total number of background modes */
        unsigned int bg_num;			/* the number of the first background modes for foreground detection */
        unsigned char* cur_intensity;		/* the color intensity of current pixel */
        float* cur_lbp_pattern;			/* the local binary pattern of current pixel */

        int lbp_pattern_length;
        int color_channel;
        int pixel_mode_num;

        BG_PIXEL_PATTERN(int _pixel_mode_num, int _lbp_pattern_length, int _color_channel = 3) {
          pixel_mode_num = _pixel_mode_num;
          lbp_pattern_length = _lbp_pattern_length;
          color_channel = _color_channel;

          pixel_MODEs = new BG_PIXEL_MODE*[pixel_mode_num];

          for (int i = 0; i < pixel_mode_num; i++) {
            pixel_MODEs[i] = new BG_PIXEL_MODE(_lbp_pattern_length, _color_channel);
          }

          lbp_pattern_idxes = new unsigned short[pixel_mode_num];
          cur_intensity = new unsigned char[color_channel];
          cur_lbp_pattern = new float[lbp_pattern_length];
        };

        virtual ~BG_PIXEL_PATTERN() {
          delete[] lbp_pattern_idxes;
          delete[] cur_intensity;
          delete[] cur_lbp_pattern;

          for (int i = 0; i < pixel_mode_num; i++)
            delete pixel_MODEs[i];
          delete[] pixel_MODEs;
        };
      };

      class IMAGE_BG_MODEL
      {
        int pixel_length;

        BG_PIXEL_PATTERN** pixel_PATTERNs;

        IMAGE_BG_MODEL(int _pixel_length, int _pixel_mode_num, int _lbp_pattern_length, int _color_channel = 3) {
          pixel_length = _pixel_length;

          pixel_PATTERNs = new BG_PIXEL_PATTERN*[pixel_length];
          for (int i = 0; i < pixel_length; i++)
            pixel_PATTERNs[i] = new BG_PIXEL_PATTERN(_pixel_mode_num, _lbp_pattern_length, _color_channel);
        }
        virtual ~IMAGE_BG_MODEL() {
          for (int i = 0; i < pixel_length; i++)
            delete pixel_PATTERNs[i];
          delete[] pixel_PATTERNs;
        }
      };
    }
  }
}

#endif
