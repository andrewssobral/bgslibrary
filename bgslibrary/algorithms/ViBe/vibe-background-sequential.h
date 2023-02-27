#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace bgslibrary
{
  namespace algorithms
  {
    namespace vibe
    {
      const int COLOR_BACKGROUND = 0; // Default label for background pixels
      const int COLOR_FOREGROUND = 255; // Default label for foreground pixels. Note that some authors chose any value different from 0 instead
      const int NUMBER_OF_HISTORY_IMAGES = 2;

      /**
       * \typedef struct vibeModel_Sequential_t
       * \brief Data structure for the background subtraction model.
       *
       * This data structure contains the background model as well as some paramaters value.
       * The code is designed to hide all the implementation details to the user to ease its use.
       */
      typedef struct vibeModel_Sequential vibeModel_Sequential_t;

      /**
       * Allocation of a new data structure where the background model will be stored.
       * Please note that this function only creates the structure to host the data.
       * This data structures will only be filled with a call to \ref libvibeModel_Sequential_AllocInit_8u_C1R.
       *
       * \result A pointer to a newly allocated \ref vibeModel_Sequential_t
       * structure, or <tt>NULL</tt> in the case of an error.
       */
      vibeModel_Sequential_t *libvibeModel_Sequential_New();

      /**
       * ViBe uses several parameters.
       * You can print and change some of them if you want. However, default
       * value should meet your needs for most videos.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @return
       */
      uint32_t libvibeModel_Sequential_PrintParameters(const vibeModel_Sequential_t *model);

      /**
       * Setter.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @param numberOfSamples
       * @return
       */
      int32_t libvibeModel_Sequential_SetNumberOfSamples(
        vibeModel_Sequential_t *model,
        const uint32_t numberOfSamples
      );

      /**
       * Setter.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @return
       */
      uint32_t libvibeModel_Sequential_GetNumberOfSamples(const vibeModel_Sequential_t *model);

      /**
       * Setter.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @param matchingThreshold
       * @return
       */
      int32_t libvibeModel_Sequential_SetMatchingThreshold(
        vibeModel_Sequential_t *model,
        const uint32_t matchingThreshold
      );

      /**
       * Setter.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @return
       */
      uint32_t libvibeModel_Sequential_GetMatchingThreshold(const vibeModel_Sequential_t *model);

      /**
       * Setter.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @param matchingNumber
       * @return
       */
      int32_t libvibeModel_Sequential_SetMatchingNumber(
        vibeModel_Sequential_t *model,
        const uint32_t matchingNumber
      );

      /**
       * Setter.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @param updateFactor New value for the update factor. Please note that the update factor is to be understood as a probability of updating. More specifically, an update factor of 16 means that 1 out of every 16 background pixels is updated. Likewise, an update factor of 1 means that every background pixel is updated.
       * @return
       */
      int32_t libvibeModel_Sequential_SetUpdateFactor(
        vibeModel_Sequential_t *model,
        const uint32_t updateFactor
      );

      /**
       * Getter.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @return
       */
      uint32_t libvibeModel_Sequential_GetMatchingNumber(const vibeModel_Sequential_t *model);


      /**
       * Getter.
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @return
       */
      uint32_t libvibeModel_Sequential_GetUpdateFactor(const vibeModel_Sequential_t *model);

      /**
       * \brief Frees all the memory used by the <tt>model</tt> and deallocates the structure.
       *
       * This function frees all the memory allocated by \ref libvibeModel_SequentialNew and
       * \ref libvibeModel_Sequential_AllocInit_8u_C1R or \ref libvibeModel_Sequential_AllocInit_8u_C3R.
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @return
       */
      int32_t libvibeModel_Sequential_Free(vibeModel_Sequential_t *model);

      /**
       * The two following functions allocate the required memory according to the
       * model parameters and the dimensions of the input images.
       * You must use the "C1R" function for grayscale images and the "C3R" for color
       * images.
       * These 2 functions also initialize the background model using the content
       * of *image_data which is the pixel buffer of the first image of your stream.
       */
      // -------------------------  Single channel images ----------------------------
      /**
        *
        * @param model The data structure with ViBe's background subtraction model and parameters.
        * @param image_data
        * @param width
        * @param height
        * @return
        */
      int32_t libvibeModel_Sequential_AllocInit_8u_C1R(
        vibeModel_Sequential_t *model,
        const uint8_t *image_data,
        const uint32_t width,
        const uint32_t height
      );

      /* These 2 functions perform 2 operations:
      *   - they classify the pixels *image_data using the provided model and store
      *     the results in *segmentation_map.
      *   - they update *model according to these results and the content of
      *     *image_data.
      * You must use the "C1R" function for grayscale images and the "C3R" for color
      * images.
      */
      /**
        *
        * @param model The data structure with ViBe's background subtraction model and parameters.
        * @param image_data
        * @param segmentation_map
        * @return
        */
      int32_t libvibeModel_Sequential_Segmentation_8u_C1R(
        vibeModel_Sequential_t *model,
        const uint8_t *image_data,
        uint8_t *segmentation_map
      );

      /**
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @param image_data
       * @param updating_mask
       * @return
       */
      int32_t libvibeModel_Sequential_Update_8u_C1R(
        vibeModel_Sequential_t *model,
        const uint8_t *image_data,
        uint8_t *updating_mask
      );

      // -------------------------  Three channel images -----------------------------
      /**
       * The pixel values of color images are arranged in the following order
       * RGBRGBRGB... (or HSVHSVHSVHSVHSVHSV...)
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @param image_data
       * @param width
       * @param height
       * @return
       */
      int32_t libvibeModel_Sequential_AllocInit_8u_C3R(
        vibeModel_Sequential_t *model,
        const uint8_t *image_data,
        const uint32_t width,
        const uint32_t height
      );

      /* These 2 functions perform 2 operations:
      *   - they classify the pixels *image_data using the provided model and store
      *     the results in *segmentation_map.
      *   - they update *model according to these results and the content of
      *     *image_data.
      * You must use the "C1R" function for grayscale images and the "C3R" for color
      * images.
      */
      /**
        * The pixel values of color images are arranged in the following order
        * RGBRGBRGB... (or HSVHSVHSVHSVHSVHSV...)
        *
        * @param model The data structure with ViBe's background subtraction model and parameters.
        * @param image_data
        * @param segmentation_map
        * @return
        */
      int32_t libvibeModel_Sequential_Segmentation_8u_C3R(
        vibeModel_Sequential_t *model,
        const uint8_t *image_data,
        uint8_t *segmentation_map
      );

      /**
       * The pixel values of color images are arranged in the following order
       * RGBRGBRGB... (or HSVHSVHSVHSVHSVHSV...)
       *
       * @param model The data structure with ViBe's background subtraction model and parameters.
       * @param image_data
       * @param updating_mask
       * @return
       */
      int32_t libvibeModel_Sequential_Update_8u_C3R(
        vibeModel_Sequential_t *model,
        const uint8_t *image_data,
        uint8_t *updating_mask
      );
    }
  }
}
