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
/**
  @file vibe-background-sequential.h
  @brief Interface for the ViBe library
  @author Marc Van Droogenbroeck
  @date July 2014
  @details

  Full documentation is available online at:
     http://www.ulg.ac.be/telecom/research/vibe/doc

  All technical details are available in the following paper:
<em>O. Barnich and M. Van Droogenbroeck. ViBe: A universal background subtraction algorithm for video sequences. IEEE Transactions on Image Processing, 20(6):1709-1724, June 2011.</em>

\verbatim
BiBTeX information

  @article{Barnich2011ViBe,
  title = {{ViBe}: A universal background subtraction algorithm for video sequences},
  author = {O. Barnich and M. {Van Droogenbroeck}},
  journal = {IEEE Transactions on Image Processing},
  volume = {20},
  number = {6},
  pages = {1709-1724},
  month = {June},
  year = {2011},
  keywords = {ViBe, Background, Background subtraction, Segmentation, Motion, Motion detection},
  pdf = {http://orbi.ulg.ac.be/bitstream/2268/145853/1/Barnich2011ViBe.pdf},
  doi = {10.1109/TIP.2010.2101613},
  url = {http://hdl.handle.net/2268/145853}
  }
\endverbatim
See
\cite Barnich2011ViBe
*/
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define COLOR_BACKGROUND   0 /*!< Default label for background pixels */
#define COLOR_FOREGROUND 255 /*!< Default label for foreground pixels. Note that some authors chose any value different from 0 instead */

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
