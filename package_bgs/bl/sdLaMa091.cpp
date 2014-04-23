/**
 * \file   sdLaMa091.c
 * \brief  Implementation of a sigma-delta background subtraction algorithm.
 * \author Benjamin Laugraud
 *
 * This file implements a sigma-delta background subtraction algorithm. The
 * implementation is based on the section 2.1 from the article:
 *
 * MOTION DETECTION: FAST AND ROBUST ALGORITHMS FOR EMBEDDED SYSTEMS
 *
 * written by L. Lacassage and A. Manzanera in 2009.
 * You have to take a look at the sdLaMa091.h file to have more documentation.
 */

#include "sdLaMa091.h"

/******************************************************************************
 * Macro(s) for the default values of the parameters                          *
 ******************************************************************************/

#define DEFAULT_N 1        /**< Default amplification factor */
#define DEFAULT_VMIN 2     /**< Default minimal variance */
#define DEFAULT_VMAX 255   /**< Default maximal variance */

/******************************************************************************
 * Macro(s) for internal implementation                                       *
 ******************************************************************************/

#define LIB "sdLaMa091 - " /**< Message header */
#define RED 0              /**< Value for red channel */
#define GREEN 1            /**< Value for green channel */
#define BLUE 2             /**< Value for blue channel */
#define CHANNELS 3         /**< Number of channels (RGB) */

/******************************************************************************
 * Enumeration(s)                                                             *
 ******************************************************************************/

/**
 * \enum  image_t
 * \brief Types of images.
 *
 * image_t defines some constants to represent easily the different types of
 * images.
 */
typedef enum {
  UNKNOWN,                 /**< Monochromatic */
  C1R,                     /**< Unknown */
  C3R                      /**< Trichromatic */
} image_t;

/******************************************************************************
 * Structure(s)                                                               *
 ******************************************************************************/

struct sdLaMa091 {
  image_t  imageType;      /**< Type of images */

  uint32_t width;          /**< Width of images */
  uint32_t rgbWidth;       /**< Width of images using the 3 channels */
  uint32_t height;         /**< Height of images */
  uint32_t stride;         /**< Stride of images */
  uint32_t numBytes;       /**< Total number of bytes in images */
  uint32_t unusedBytes;    /**< Unused bytes in a row of images */
  uint32_t rgbUnusedBytes; /**< Unused bytes in a row of images using the 3
                                channels */

  uint32_t N;              /**< Amplification factor */
  uint32_t Vmin;           /**< Minimal variance */
  uint32_t Vmax;           /**< Maximum variance */

  uint8_t* Mt;             /**< Background estimator */
  uint8_t* Ot;             /**< Sigma-Delta estimator */
  uint8_t* Vt;             /**< Variance (dispersion parameter) */
};

/******************************************************************************
 * Prototype(s)                                                               *
 ******************************************************************************
 * The inline functions are documented before the functions themselves.       *
 ******************************************************************************/

#if defined(DEFENSIVE_ALLOC) || defined(DEFENSIVE_POINTER) || \
  defined(DEFENSIVE_PARAM)
static inline void outputError(char* error);
#endif /* DEFENSIVE_ALLOC || DEFENSIVE_POINTER || DEFENSIVE_PARAM */

static inline uint8_t absVal(int8_t num);
static inline uint8_t min(uint8_t a, uint8_t b);
static inline uint8_t max(uint8_t a, uint8_t b);

/******************************************************************************
 * Inline function(s)                                                         *
 ******************************************************************************/

#if defined(DEFENSIVE_ALLOC) || defined(DEFENSIVE_POINTER) || \
  defined(DEFENSIVE_PARAM)
/**
 * \brief Prints an error message.
 *
 * Function to print an error on the standard error output (stderr). The error
 * message will has the following shape:
 *
 * LIB error
 *
 * LIB is a constant defined in this file.
 *
 * \param error The error message.
 */
static inline void outputError(char* error) {
  fprintf(stderr, "%s%s\n", LIB, error);
}
#endif /* DEFENSIVE_ALLOC || DEFENSIVE_POINTER || DEFENSIVE_PARAM */

/**
 * \brief  Computes an absolute value.
 *
 * Function to compute the absolute value of the argument.
 *
 * \param  num A given number in int8_t format.
 * \return The absolute value of num in uint8_t format.
 */
static inline uint8_t absVal(int8_t num) {
  return (num < 0) ? (uint8_t)-num : (uint8_t)num;
}

/**
 * \brief  Finds the minimum.
 *
 * Function to find the minimum of two arguments.
 *
 * \param  a First given number in int8_t format.
 * \param  b Second given number in int8_t format.
 * \return The minimum between a and b in int8_t format.
 */
static inline uint8_t min(uint8_t a, uint8_t b) {
  return (a < b) ? a : b;
}

/**
 * \brief  Finds the maximum.
 *
 * Function to find the maximum of two arguments.
 *
 * \param  a First given number in int8_t format.
 * \param  b Second given number in int8_t format.
 * \return The maximum between a and b in int8_t format.
 */
static inline uint8_t max(uint8_t a, uint8_t b) {
  return (a > b) ? a : b;
}

/******************************************************************************
 * Allocation(s) function(s)                                                  *
 ******************************************************************************/

sdLaMa091_t* sdLaMa091New(void) {
  sdLaMa091_t* sdLaMa091 = (sdLaMa091_t*) malloc(sizeof(*sdLaMa091));

#ifdef DEFENSIVE_ALLOC
  if (sdLaMa091 == NULL) {
    outputError("Cannot allocate sdLaMa091 structure");
    return NULL;
  }
#endif /* DEFENSIVE_ALLOC */

  sdLaMa091->imageType = UNKNOWN;

  sdLaMa091->width = 0;
  sdLaMa091->rgbWidth = 0;
  sdLaMa091->height = 0;
  sdLaMa091->stride = 0;
  sdLaMa091->numBytes = 0;
  sdLaMa091->unusedBytes = 0;
  sdLaMa091->rgbUnusedBytes = 0;

  sdLaMa091->N = DEFAULT_N;
  sdLaMa091->Vmin = DEFAULT_VMIN;
  sdLaMa091->Vmax = DEFAULT_VMAX;

  sdLaMa091->Mt = NULL;
  sdLaMa091->Ot = NULL;
  sdLaMa091->Vt = NULL;

  return sdLaMa091;
}

int32_t sdLaMa091AllocInit_8u_C1R(sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  const uint32_t width,
  const uint32_t height,
  const uint32_t stride) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot initialize a NULL structure");
    return EXIT_FAILURE;
  }

  if (image_data == NULL) {
    outputError("Cannot allocate ressources for a NULL image");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

#ifdef DEFENSIVE_PARAM
  if (width == 0 || height == 0 || stride == 0) {
    outputError("Cannot allocate ressources for zero values");
    return EXIT_FAILURE;
  }

  if (stride < width) {
    outputError("Cannot allocate ressources for a stride lower than the width");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_PARAM */

  sdLaMa091->imageType = C1R;

  sdLaMa091->width = width;
  sdLaMa091->height = height;
  sdLaMa091->stride = stride;
  sdLaMa091->numBytes = stride * height;
  sdLaMa091->unusedBytes = stride - sdLaMa091->width;

  /* Initialisation of Mt
   * Initialized with a copy of the first frame
   */
  sdLaMa091->Mt = (uint8_t*) malloc(sdLaMa091->numBytes);
#ifdef DEFENSIVE_ALLOC
  if (sdLaMa091->Mt == NULL) {
    outputError("Cannot allocate sdLaMa091->Mt table");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_ALLOC */
  memcpy(sdLaMa091->Mt, image_data, sdLaMa091->numBytes);

  /* Initialisation of Ot
   * Initialized with zero values
   */
  sdLaMa091->Ot = (uint8_t*) malloc(sdLaMa091->numBytes);
#ifdef DEFENSIVE_ALLOC
  if (sdLaMa091->Ot == NULL) {
    outputError("Cannot allocate sdLaMa091->Ot table");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_ALLOC */
  uint8_t* workOt = sdLaMa091->Ot;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workOt)
      *workOt = 0;

    /* Skip end of stride */
    if (sdLaMa091->unusedBytes > 0)
      workOt += sdLaMa091->unusedBytes;
  }

  /* Initialisation of Vt
   * Initialized with sdLaMa091->Vmin values
   */
  sdLaMa091->Vt = (uint8_t*) malloc(sdLaMa091->numBytes);
#ifdef DEFENSIVE_ALLOC
  if (sdLaMa091->Vt == NULL) {
    outputError("Cannot allocate sdLaMa091->Vt table");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_ALLOC */
  uint8_t* workVt = sdLaMa091->Vt;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workVt)
      *workVt = sdLaMa091->Vmin;

    /* Skip end of stride */
    if (sdLaMa091->unusedBytes > 0)
      workVt += sdLaMa091->unusedBytes;
  }

  return EXIT_SUCCESS;
}

int32_t sdLaMa091AllocInit_8u_C3R(sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  const uint32_t width,
  const uint32_t height,
  const uint32_t stride) {
  int32_t success = sdLaMa091AllocInit_8u_C1R(sdLaMa091, image_data, width,
    height, stride);

  if (success == EXIT_SUCCESS) {
    sdLaMa091->imageType = C3R;
    sdLaMa091->rgbWidth = sdLaMa091->width * CHANNELS;
    sdLaMa091->rgbUnusedBytes = stride - sdLaMa091->rgbWidth;
    sdLaMa091->width = 0;
    sdLaMa091->unusedBytes = 0;
  }

  return success;
}

/******************************************************************************
 * Parameter(s) function(s)                                                   *
 ******************************************************************************/

int32_t sdLaMa091SetAmplificationFactor(sdLaMa091_t* sdLaMa091,
  const uint32_t amplificationFactor) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot set a parameter of a NULL structure");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

#ifdef DEFENSIVE_PARAM
  if (amplificationFactor == 0) {
    outputError("Cannot set a parameter with a zero value");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_PARAM */

  sdLaMa091->N = amplificationFactor;

  return EXIT_SUCCESS;
}

uint32_t sdLaMa091GetAmplificationFactor(const sdLaMa091_t* sdLaMa091) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot get a parameter of a NULL structure");
    errno = ERROR_OCCURED;

    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

  return sdLaMa091->N;
}

int32_t sdLaMa091SetMaximalVariance(sdLaMa091_t* sdLaMa091,
  const uint32_t maximalVariance) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot set a parameter of a NULL structure");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

#ifdef DEFENSIVE_PARAM
  if (maximalVariance == 0) {
    outputError("Cannot set a parameter with a zero value");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_PARAM */

  sdLaMa091->Vmax = maximalVariance;

  return EXIT_SUCCESS;
}

uint32_t sdLaMa091GetMaximalVariance(const sdLaMa091_t* sdLaMa091) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot get a parameter of a NULL structure");
    errno = ERROR_OCCURED;

    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

  return sdLaMa091->Vmax;
}

int32_t sdLaMa091SetMinimalVariance(sdLaMa091_t* sdLaMa091,
  const uint32_t minimalVariance) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot set a parameter of a NULL structure");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

  sdLaMa091->Vmin = minimalVariance;

  return EXIT_SUCCESS;
}

uint32_t sdLaMa091GetMinimalVariance(const sdLaMa091_t* sdLaMa091) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot get a parameter of a NULL structure");
    errno = ERROR_OCCURED;

    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

  return sdLaMa091->Vmin;
}

/******************************************************************************
 * Update function(s)                                                         *
 ******************************************************************************/

int32_t sdLaMa091Update_8u_C1R(sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  uint8_t* segmentation_map) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot update a NULL structure");
    return EXIT_FAILURE;
  }

  if (image_data == NULL) {
    outputError("Cannot update a structure with a NULL image");
    return EXIT_FAILURE;
  }

  if (segmentation_map == NULL) {
    outputError("Cannot update a structure with a NULL segmentation map");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Mt == NULL) {
    outputError("Cannot update a structure with a NULL Mt table");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Ot == NULL) {
    outputError("Cannot update a structure with a NULL Ot table");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Vt == NULL) {
    outputError("Cannot update a structure with a NULL Vt table");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

#ifdef DEFENSIVE_PARAM
  if (sdLaMa091->imageType != C1R) {
    outputError("Cannot update a structure which is not C1R");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->width == 0 || sdLaMa091->height == 0 ||
    sdLaMa091->stride == 0) {
    outputError("Cannot update a structure with zero values");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->stride < sdLaMa091->width) {
    outputError("Cannot update a structure with a stride lower than the width");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Vmax < sdLaMa091->Vmin) {
    outputError("Cannot update a structure with Vmax inferior to Vmin");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_PARAM */

  /*
   * Mt estimation (step 1)
   */
  const uint8_t* workImage = image_data;
  uint8_t* workMt = sdLaMa091->Mt;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workImage, ++workMt) {
      if (*workMt < *workImage)
        ++(*workMt);
      else if (*workMt > *workImage)
        --(*workMt);
    }

    /* Skip end of stride */
    if (sdLaMa091->unusedBytes > 0) {
      workImage += sdLaMa091->unusedBytes;
      workMt += sdLaMa091->unusedBytes;
    }
  }

  /*
   * Ot computation (step 2)
   */
  workImage = image_data;
  workMt = sdLaMa091->Mt;
  uint8_t* workOt = sdLaMa091->Ot;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workImage, ++workMt,
      ++workOt)
      *workOt = absVal(*workMt - *workImage);

    /* Skip end of stride */
    if (sdLaMa091->unusedBytes > 0) {
      workImage += sdLaMa091->unusedBytes;
      workMt += sdLaMa091->unusedBytes;
      workOt += sdLaMa091->unusedBytes;
    }
  }

  /*
   * Vt update (step 3)
   */
  workOt = sdLaMa091->Ot;
  uint8_t* workVt = sdLaMa091->Vt;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workOt, ++workVt) {
      uint32_t ampOt = sdLaMa091->N * *workOt;

      if (*workVt < ampOt)
        ++(*workVt);
      else if (*workVt > ampOt)
        --(*workVt);

      *workVt = max(min(*workVt, sdLaMa091->Vmax), sdLaMa091->Vmin);
    }

    /* Skip end of stride */
    if (sdLaMa091->unusedBytes > 0) {
      workOt += sdLaMa091->unusedBytes;
      workVt += sdLaMa091->unusedBytes;
    }
  }

  /*
   * Et estimation (step 4)
   */
  workOt = sdLaMa091->Ot;
  workVt = sdLaMa091->Vt;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++segmentation_map,
      ++workOt, ++workVt) {
      /* Et estimation */
      if (*workOt < *workVt)
        *segmentation_map = BACKGROUND;
      else
        *segmentation_map = FOREGROUND;
    }

    /* Skip end of stride */
    if (sdLaMa091->unusedBytes > 0) {
      segmentation_map += sdLaMa091->unusedBytes;
      workOt += sdLaMa091->unusedBytes;
      workVt += sdLaMa091->unusedBytes;
    }
  }

  return EXIT_SUCCESS;
}

int32_t sdLaMa091Update_8u_C3R(sdLaMa091_t* sdLaMa091,
  const uint8_t* image_data,
  uint8_t* segmentation_map) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot update a NULL structure");
    return EXIT_FAILURE;
  }

  if (image_data == NULL) {
    outputError("Cannot update a structure with a NULL image");
    return EXIT_FAILURE;
  }

  if (segmentation_map == NULL) {
    outputError("Cannot update a structure with a NULL segmentation map");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Mt == NULL) {
    outputError("Cannot update a structure with a NULL Mt table");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Ot == NULL) {
    outputError("Cannot update a structure with a NULL Ot table");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Vt == NULL) {
    outputError("Cannot update a structure with a NULL Vt table");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

#ifdef DEFENSIVE_PARAM
  if (sdLaMa091->imageType != C3R) {
    outputError("Cannot update a structure which is not C3R");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->rgbWidth == 0 || sdLaMa091->height == 0 ||
    sdLaMa091->stride == 0) {
    outputError("Cannot update a structure with zero values");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->stride < sdLaMa091->rgbWidth) {
    outputError("Cannot update a structure with a stride lower than the width");
    return EXIT_FAILURE;
  }

  if (sdLaMa091->Vmax < sdLaMa091->Vmin) {
    outputError("Cannot update a structure with Vmax inferior to Vmin");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_PARAM */

  /*
   * Mt estimation (step 1)
   */
  const uint8_t* workImage = image_data;
  uint8_t* workMt = sdLaMa091->Mt;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->rgbWidth; ++j, ++workImage, ++workMt) {
      if (*workMt < *workImage)
        ++(*workMt);
      else if (*workMt > *workImage)
        --(*workMt);
    }

    /* Skip end of stride */
    if (sdLaMa091->rgbUnusedBytes > 0) {
      workImage += sdLaMa091->rgbUnusedBytes;
      workMt += sdLaMa091->rgbUnusedBytes;
    }
  }

  /*
   * Ot computation (step 2)
   */
  workImage = image_data;
  workMt = sdLaMa091->Mt;
  uint8_t* workOt = sdLaMa091->Ot;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->rgbWidth; ++j, ++workImage, ++workMt,
      ++workOt)
      *workOt = absVal(*workMt - *workImage);

    /* Skip end of stride */
    if (sdLaMa091->rgbUnusedBytes > 0) {
      workImage += sdLaMa091->rgbUnusedBytes;
      workMt += sdLaMa091->rgbUnusedBytes;
      workOt += sdLaMa091->rgbUnusedBytes;
    }
  }

  /*
   * Vt update (step 3)
   */
  workOt = sdLaMa091->Ot;
  uint8_t* workVt = sdLaMa091->Vt;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->rgbWidth; ++j, ++workOt, ++workVt) {
      uint32_t ampOt = sdLaMa091->N * *workOt;

      if (*workVt < ampOt)
        ++(*workVt);
      else if (*workVt > ampOt)
        --(*workVt);

      *workVt = max(min(*workVt, sdLaMa091->Vmax), sdLaMa091->Vmin);
    }

    /* Skip end of stride */
    if (sdLaMa091->rgbUnusedBytes > 0) {
      workOt += sdLaMa091->rgbUnusedBytes;
      workVt += sdLaMa091->rgbUnusedBytes;
    }
  }

  /*
   * Et estimation (step 4)
   */
  workOt = sdLaMa091->Ot;
  workVt = sdLaMa091->Vt;

  /* Row by row */
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    /* 0 if the current value is from the red channel, 1 green channel and 2
     * blue channel
     */
    uint32_t numColor = 0;
    /* To know whether the current pixel is from foreground or not */
    bool isForeground = false;

    /* Significant bytes of a row */
    for (uint32_t j = 0; j < sdLaMa091->rgbWidth; ++j, ++workOt, ++workVt) {
      if (*workOt >= *workVt)
        isForeground = true;

      /* Complete segmentation map */
      if (numColor == BLUE) {
        if (isForeground) {
          *segmentation_map = FOREGROUND;
          *(++segmentation_map) = FOREGROUND;
          *(++segmentation_map) = FOREGROUND;
          ++segmentation_map;
        }
        else {
          *segmentation_map = BACKGROUND;
          *(++segmentation_map) = BACKGROUND;
          *(++segmentation_map) = BACKGROUND;
          ++segmentation_map;
        }

        isForeground = false;
      }

      numColor = (numColor + 1) % CHANNELS;
    }

    /* Skip end of stride */
    if (sdLaMa091->rgbUnusedBytes > 0) {
      segmentation_map += sdLaMa091->rgbUnusedBytes;
      workOt += sdLaMa091->rgbUnusedBytes;
      workVt += sdLaMa091->rgbUnusedBytes;
    }
  }

  return EXIT_SUCCESS;
}

/******************************************************************************
 * Free memory function(s)                                                    *
 ******************************************************************************/

int32_t sdLaMa091Free(sdLaMa091_t* sdLaMa091) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot free a NULL strucutre");
    return EXIT_FAILURE;
  }
#endif /* DEFENSIVE_POINTER */

  if (sdLaMa091->Mt != NULL)
    free(sdLaMa091->Mt);
  if (sdLaMa091->Ot != NULL)
    free(sdLaMa091->Ot);
  if (sdLaMa091->Vt != NULL)
    free(sdLaMa091->Vt);

  free(sdLaMa091);

  return EXIT_SUCCESS;
}
