/**
 * \file   sdLaMa091.h
 * \brief  Interface of a sigma-delta background subtraction algorithm.
 * \author Benjamin Laugraud
 *
 * This file is an interface for a sigma-delta background subtraction algorithm.
 * The implementation is based on the section 2.1 from the article:
 *
 * MOTION DETECTION: FAST AND ROBUST ALGORITHMS FOR EMBEDDED SYSTEMS
 *
 * written by L. Lacassage and A. Manzanera in 2009.
 *
 * \b Compilation
 *
 * Some symbols can be defined for the compilation of the library. For instance,
 * with gcc, you can define a symbol A with the flag -D A. The following symbols
 * can be used:
 *
 * \li \c DEFENSIVE_ALLOC   To compile additional code to test the success of
 *                          the different memory allocations.
 * \li \c DEFENSIVE_POINTER To compile additional code to test whether the
 *                          pointers are NULL or not.
 * \li \c DEFENSIVE_PARAM   To test the coherance of the parameters given to a
 *                          function (for example, negative sizes, a width
 *                          greater than the stride, etc).
 *
 * It is recommended to use all the symbols during a test phase. After, you
 * should recompile the library without the symbols to optimize the different
 * operations.
 *
 * <b>Example of code</b>
 *
 * The following code snippet gives a brief example of how to use this library
 * for monochromatic images:
 *
 * \code
 * #include <stdint.h>
 * #include <stdlib.h>
 *
 * #include "sdLaMa091.h"
 *
 * // Allocation
 * sdLaMa091_t sdLaMa091 = sdLaMa091New();
 *
 * // Initialization
 * uint8_t* image_data = getFirstFrame();
 * uint32_t width = getWidth();
 * uint32_t height = getHeight();
 * uint32_t stride = getStride();
 * sdLaMa091AllocInit_8u_C1R(sdLaMa091, image_data, width, height, stride);
 *
 * // Create segmentation map
 * uint8_t* segmentation_map = malloc(stride * height);
 *
 * // Update the algorithm
 * while ("We are not at the last frame") {
 *   image_data = getNextFrame();
 *   sdLaMa091Update_8u_C3R(sdLaMa091, image_data, segmentation_map);
 *   "Exploit segmentation map";
 * }
 *
 * // Cleaning
 * free(segmentation_map);
 * sdLaMa091Free(sdLaMa091);
 * \endcode
 *
 * This code snippet is only an illustration and uses fictive functions.
 */
#ifndef SD_LA_MA_091_H_
#define SD_LA_MA_091_H_

#include <errno.h>
#include "stdbool.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************
 * Macro(s) for implementation                                                *
 ******************************************************************************/

#define BACKGROUND 0    /**< Value in the segmentation map for the background */
#define FOREGROUND 255  /**< Value in the segmentation map for the foreground */
#define ERROR_OCCURED 1 /**< Errno value for an error */

/******************************************************************************
 * Structure(s)                                                               *
 ******************************************************************************/

/**
 * \struct sdLaMa091
 * \brief  Structure of the algorithm.
 *
 * sdLaMa091 is a structure which contains the informations and the allocations
 * needed by the algorithm and the implementation.
 */
typedef struct sdLaMa091 sdLaMa091_t;

/******************************************************************************
 * Allocation(s) function(s)                                                  *
 ******************************************************************************/

/**
 * \brief  Allocates an empty sdLaMa091 structure.
 *
 * This function allocates an empty sdLaMa091 structure. The
 * amplification factor (N), maximal variance (Vmax) and the minimal variance
 * (Vmin) will be sets to 1, 255 and 2 by default according to the article.
 *
 * \return A pointer to the structure or NULL if an operation failed.
 */
sdLaMa091_t* sdLaMa091New(void);

/**
 * \brief  Initializes an empty sdLaMa091 structure for C1R images.
 *
 * This function initializes an empty sdLaMa091 structure for monochromatic
 * images. If an error occured, a message will be printed on the standard error
 * output (stderr).
 *
 * \param  sdLaMa091  A sdLaMa091 structure.
 * \param  image_data A pointer to the first frame to initialize the algorithm.
 * \param  width      The width of the images.
 * \param  height     The height of the images.
 * \param  stride     The stride of the images.
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091AllocInit_8u_C1R(sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  const uint32_t width,
  const uint32_t height,
  const uint32_t stride);

/**
 * \brief  Initializes an empty sdLaMa091 structure for C3R images.
 *
 * This function initializes an empty sdLaMa091 structure for trichromatic
 * images. If an error occured, a message will be printed on the standard error
 * output (stderr).
 *
 * \param  sdLaMa091  A sdLaMa091 structure.
 * \param  image_data A pointer to the first frame to initialize the algorithm.
 * \param  width      The width of the images.
 * \param  height     The height of the images.
 * \param  stride     The stride of the images.
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091AllocInit_8u_C3R(sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  const uint32_t width,
  const uint32_t height,
  const uint32_t stride);

/******************************************************************************
 * Parameter(s) function(s)                                                   *
 ******************************************************************************/

/**
 * \brief  Sets the amplification factor.
 *
 * This function sets the amplification factor (N) of a sdLaMa091 structure. The
 * author advices to set this factor between 1 and 4. If an error occured, a
 * message will be printed on the standard error output (stderr).
 *
 * \param  sdLaMa091           A sdLaMa091 structure.
 * \param  amplificationFactor The new amplification factor to set.
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091SetAmplificationFactor(sdLaMa091_t* sdLaMa091,
  const uint32_t amplificationFactor);

/**
 * \brief  Gets the amplification factor.
 *
 * This function gets the amplification factor (N) of a sdLaMa091 structure. If
 * an error occured, a message will be printed on the standard error output
 * (stderr).
 *
 * \param  sdLaMa091 A sdLaMa091 structure.
 * \return The amplification factor (N) of the given sdLaMa091. If an error
 *         occured, EXIT_FAILURE will be returned and errno will be set to
 *         ERROR_OCCURED.
 */
uint32_t sdLaMa091GetAmplificationFactor(const sdLaMa091_t* sdLaMa091);

/**
 * \brief  Sets the maximal variance.
 *
 * This function sets the maximal variance (Vmax) of a sdLaMa091 structure. The
 * author advices to set this factor to 255. If an error occured, a message will
 * be printed on the standard error output (stderr).
 *
 * \param  sdLaMa091       A sdLaMa091 structure.
 * \param  maximalVariance The new maximal variance to set.
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091SetMaximalVariance(sdLaMa091_t* sdLaMa091,
  const uint32_t maximalVariance);

/**
 * \brief  Gets the maximal variance.
 *
 * This function gets the maximal variance (Vmax) of a sdLaMa091 structure. If
 * an error occured, a message will be printed on the standard error output
 * (stderr).
 *
 * \param  sdLaMa091 A sdLaMa091 structure.
 * \return The maximal variance (Vmax) of the given sdLaMa091. If an error
 *         occured, EXIT_FAILURE will be returned and errno will be set to
 *         ERROR_OCCURED.
 */
uint32_t sdLaMa091GetMaximalVariance(const sdLaMa091_t* sdLaMa091);

/**
 * \brief  Sets the minimal variance.
 *
 * This function sets the minimal variance (Vmin) of a sdLaMa091 structure. The
 * author advices to set this factor to 2. If an error occured, a message will
 * be printed on the standard error output (stderr).
 *
 * \param  sdLaMa091       A sdLaMa091 structure.
 * \param  minimalVariance The new minimal variance to set.
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091SetMinimalVariance(sdLaMa091_t* sdLaMa091,
  const uint32_t minimalVariance);

/**
 * \brief  Gets the minimal variance.
 *
 * This function gets the minimal variance (Vmin) of a sdLaMa091 structure. If
 * an error occured, a message will be printed on the standard error output
 * (stderr).
 *
 * \param  sdLaMa091 A sdLaMa091 structure.
 * \return The minimal variance (Vmin) of the given sdLaMa091. If an error
 *         occured, EXIT_FAILURE will be returned and errno will be set to
 *         ERROR_OCCURED.
 */
uint32_t sdLaMa091GetMinimalVariance(const sdLaMa091_t* sdLaMa091);

/******************************************************************************
 * Update function(s)                                                         *
 ******************************************************************************/

/**
 * \brief  Updates the algorithm for C1R images.
 *
 * This function updates the algorithm with the next frame. The result of the
 * algorithm will be sets to the content of the segmentation_map pointer. This
 * pointer has to contain a vector of uint8_t elements which has the same size
 * of the different frames (stride * height, exactly). If a pixel is in the
 * foreground, the associated pixel in the segmentation map will have the
 * FOREGROUND value. In the contrary, it will have the BACKGROUND value. If an
 * error occured, a message will be printed on the standard error output
 * (stderr).
 *
 * This function is designed for monochromatic images.
 *
 * \param  sdLaMa091        A sdLaMa091 structure.
 * \param  image_data       A pointer to the next frame.
 * \param  segmentation_map A pointer to the segmentation map.
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091Update_8u_C1R(sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  uint8_t* segmentation_map);

/**
 * \brief  Updates the algorithm for C3R images.
 *
 * This function updates the algorithm with the next frame. The result of the
 * algorithm will be sets to the content of the segmentation_map pointer. This
 * pointer has to contain a vector of uint8_t elements which has the same size
 * of the different frames (stride * height, exactly). If a pixel is in the
 * foreground, the associated pixel in the segmentation map will have the
 * FOREGROUND value. In the contrary, it will have the BACKGROUND value.
 * Moreover, this value will be in the three channels (R, G and B). If an error
 * occured, a message will be printed on the standard error output (stderr).
 *
 * This function is designed for trichromatic images.
 *
 * \param  sdLaMa091        A sdLaMa091 structure.
 * \param  image_data       A pointer to the next frame.
 * \param  segmentation_map A pointer to the segmentation map.
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091Update_8u_C3R(sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  uint8_t* segmentation_map);

/******************************************************************************
 * Free memory function(s)                                                    *
 ******************************************************************************/

/**
 * \brief  Cleans the different allocations.
 *
 * This function cleans the different allocations performed for a sdLaMa091
 * structure.
 *
 * \param  sdLaMa091 A sdLaMa091 structure.
 * \return EXIT_SUCCESS if everything was ok and EXIT_FAILURE in the contrary.
 */
int32_t sdLaMa091Free(sdLaMa091_t* sdLaMa091);

#endif /* SD_LA_MA_091_H_ */
