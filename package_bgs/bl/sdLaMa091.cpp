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

#include "sdLaMa091.h"

#define DEFAULT_N 1
#define DEFAULT_VMIN 2
#define DEFAULT_VMAX 255

#define LIB "sdLaMa091 - "
#define RED 0
#define GREEN 1
#define BLUE 2
#define CHANNELS 3

typedef enum {
  UNKNOWN,
  C1R,
  C3R
} image_t;

struct sdLaMa091 {
  image_t  imageType;

  uint32_t width;
  uint32_t rgbWidth;
  uint32_t height;
  uint32_t stride;
  uint32_t numBytes;
  uint32_t unusedBytes;
  uint32_t rgbUnusedBytes;

  uint32_t N;
  uint32_t Vmin;
  uint32_t Vmax;

  uint8_t* Mt;
  uint8_t* Ot;
  uint8_t* Vt;
};

#if defined(DEFENSIVE_ALLOC) || defined(DEFENSIVE_POINTER) || \
  defined(DEFENSIVE_PARAM)
static inline void outputError(char* error);
#endif

static inline uint8_t absVal(int8_t num);
static inline uint8_t min(uint8_t a, uint8_t b);
static inline uint8_t max(uint8_t a, uint8_t b);

#if defined(DEFENSIVE_ALLOC) || defined(DEFENSIVE_POINTER) || \
  defined(DEFENSIVE_PARAM)

static inline void outputError(char* error) {
  fprintf(stderr, "%s%s\n", LIB, error);
}
#endif


static inline uint8_t absVal(int8_t num) {
  return (num < 0) ? (uint8_t)-num : (uint8_t)num;
}

static inline uint8_t min(uint8_t a, uint8_t b) {
  return (a < b) ? a : b;
}

static inline uint8_t max(uint8_t a, uint8_t b) {
  return (a > b) ? a : b;
}

sdLaMa091_t* sdLaMa091New(void) {
  sdLaMa091_t* sdLaMa091 = (sdLaMa091_t*) malloc(sizeof(*sdLaMa091));

#ifdef DEFENSIVE_ALLOC
  if (sdLaMa091 == NULL) {
    outputError("Cannot allocate sdLaMa091 structure");
    return NULL;
  }
#endif

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
#endif

#ifdef DEFENSIVE_PARAM
  if (width == 0 || height == 0 || stride == 0) {
    outputError("Cannot allocate ressources for zero values");
    return EXIT_FAILURE;
  }

  if (stride < width) {
    outputError("Cannot allocate ressources for a stride lower than the width");
    return EXIT_FAILURE;
  }
#endif

  sdLaMa091->imageType = C1R;

  sdLaMa091->width = width;
  sdLaMa091->height = height;
  sdLaMa091->stride = stride;
  sdLaMa091->numBytes = stride * height;
  sdLaMa091->unusedBytes = stride - sdLaMa091->width;

  sdLaMa091->Mt = (uint8_t*) malloc(sdLaMa091->numBytes);
#ifdef DEFENSIVE_ALLOC
  if (sdLaMa091->Mt == NULL) {
    outputError("Cannot allocate sdLaMa091->Mt table");
    return EXIT_FAILURE;
  }
#endif 
  memcpy(sdLaMa091->Mt, image_data, sdLaMa091->numBytes);

  sdLaMa091->Ot = (uint8_t*) malloc(sdLaMa091->numBytes);
#ifdef DEFENSIVE_ALLOC
  if (sdLaMa091->Ot == NULL) {
    outputError("Cannot allocate sdLaMa091->Ot table");
    return EXIT_FAILURE;
  }
#endif 
  uint8_t* workOt = sdLaMa091->Ot;

  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workOt)
      *workOt = 0;

    
    if (sdLaMa091->unusedBytes > 0)
      workOt += sdLaMa091->unusedBytes;
  }

  sdLaMa091->Vt = (uint8_t*) malloc(sdLaMa091->numBytes);
#ifdef DEFENSIVE_ALLOC
  if (sdLaMa091->Vt == NULL) {
    outputError("Cannot allocate sdLaMa091->Vt table");
    return EXIT_FAILURE;
  }
#endif
  uint8_t* workVt = sdLaMa091->Vt;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workVt)
      *workVt = sdLaMa091->Vmin;

    
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

int32_t sdLaMa091SetAmplificationFactor(sdLaMa091_t* sdLaMa091,
  const uint32_t amplificationFactor) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot set a parameter of a NULL structure");
    return EXIT_FAILURE;
  }
#endif 

#ifdef DEFENSIVE_PARAM
  if (amplificationFactor == 0) {
    outputError("Cannot set a parameter with a zero value");
    return EXIT_FAILURE;
  }
#endif 

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
#endif 

  return sdLaMa091->N;
}

int32_t sdLaMa091SetMaximalVariance(sdLaMa091_t* sdLaMa091,
  const uint32_t maximalVariance) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot set a parameter of a NULL structure");
    return EXIT_FAILURE;
  }
#endif 

#ifdef DEFENSIVE_PARAM
  if (maximalVariance == 0) {
    outputError("Cannot set a parameter with a zero value");
    return EXIT_FAILURE;
  }
#endif 

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
#endif

  return sdLaMa091->Vmax;
}

int32_t sdLaMa091SetMinimalVariance(sdLaMa091_t* sdLaMa091,
  const uint32_t minimalVariance) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot set a parameter of a NULL structure");
    return EXIT_FAILURE;
  }
#endif 

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
#endif 

  return sdLaMa091->Vmin;
}


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
#endif 

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
#endif 

  
  const uint8_t* workImage = image_data;
  uint8_t* workMt = sdLaMa091->Mt;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workImage, ++workMt) {
      if (*workMt < *workImage)
        ++(*workMt);
      else if (*workMt > *workImage)
        --(*workMt);
    }

    
    if (sdLaMa091->unusedBytes > 0) {
      workImage += sdLaMa091->unusedBytes;
      workMt += sdLaMa091->unusedBytes;
    }
  }

  workImage = image_data;
  workMt = sdLaMa091->Mt;
  uint8_t* workOt = sdLaMa091->Ot;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workImage, ++workMt,
      ++workOt)
      *workOt = absVal(*workMt - *workImage);

    
    if (sdLaMa091->unusedBytes > 0) {
      workImage += sdLaMa091->unusedBytes;
      workMt += sdLaMa091->unusedBytes;
      workOt += sdLaMa091->unusedBytes;
    }
  }

  
  workOt = sdLaMa091->Ot;
  uint8_t* workVt = sdLaMa091->Vt;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++workOt, ++workVt) {
      uint32_t ampOt = sdLaMa091->N * *workOt;

      if (*workVt < ampOt)
        ++(*workVt);
      else if (*workVt > ampOt)
        --(*workVt);

      *workVt = max(min(*workVt, sdLaMa091->Vmax), sdLaMa091->Vmin);
    }

    
    if (sdLaMa091->unusedBytes > 0) {
      workOt += sdLaMa091->unusedBytes;
      workVt += sdLaMa091->unusedBytes;
    }
  }

  
  workOt = sdLaMa091->Ot;
  workVt = sdLaMa091->Vt;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->width; ++j, ++segmentation_map,
      ++workOt, ++workVt) {
      
      if (*workOt < *workVt)
        *segmentation_map = BACKGROUND;
      else
        *segmentation_map = FOREGROUND;
    }

    
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
#endif

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
#endif 

  
  const uint8_t* workImage = image_data;
  uint8_t* workMt = sdLaMa091->Mt;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->rgbWidth; ++j, ++workImage, ++workMt) {
      if (*workMt < *workImage)
        ++(*workMt);
      else if (*workMt > *workImage)
        --(*workMt);
    }

    
    if (sdLaMa091->rgbUnusedBytes > 0) {
      workImage += sdLaMa091->rgbUnusedBytes;
      workMt += sdLaMa091->rgbUnusedBytes;
    }
  }

  
  workImage = image_data;
  workMt = sdLaMa091->Mt;
  uint8_t* workOt = sdLaMa091->Ot;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->rgbWidth; ++j, ++workImage, ++workMt,
      ++workOt)
      *workOt = absVal(*workMt - *workImage);

    
    if (sdLaMa091->rgbUnusedBytes > 0) {
      workImage += sdLaMa091->rgbUnusedBytes;
      workMt += sdLaMa091->rgbUnusedBytes;
      workOt += sdLaMa091->rgbUnusedBytes;
    }
  }

  workOt = sdLaMa091->Ot;
  uint8_t* workVt = sdLaMa091->Vt;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    for (uint32_t j = 0; j < sdLaMa091->rgbWidth; ++j, ++workOt, ++workVt) {
      uint32_t ampOt = sdLaMa091->N * *workOt;

      if (*workVt < ampOt)
        ++(*workVt);
      else if (*workVt > ampOt)
        --(*workVt);

      *workVt = max(min(*workVt, sdLaMa091->Vmax), sdLaMa091->Vmin);
    }

    
    if (sdLaMa091->rgbUnusedBytes > 0) {
      workOt += sdLaMa091->rgbUnusedBytes;
      workVt += sdLaMa091->rgbUnusedBytes;
    }
  }

  workOt = sdLaMa091->Ot;
  workVt = sdLaMa091->Vt;

  
  for (uint32_t i = 0; i < sdLaMa091->numBytes; i += sdLaMa091->stride) {
    
    uint32_t numColor = 0;
    
    bool isForeground = false;

    
    for (uint32_t j = 0; j < sdLaMa091->rgbWidth; ++j, ++workOt, ++workVt) {
      if (*workOt >= *workVt)
        isForeground = true;

      
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

    
    if (sdLaMa091->rgbUnusedBytes > 0) {
      segmentation_map += sdLaMa091->rgbUnusedBytes;
      workOt += sdLaMa091->rgbUnusedBytes;
      workVt += sdLaMa091->rgbUnusedBytes;
    }
  }

  return EXIT_SUCCESS;
}

int32_t sdLaMa091Free(sdLaMa091_t* sdLaMa091) {
#ifdef DEFENSIVE_POINTER
  if (sdLaMa091 == NULL) {
    outputError("Cannot free a NULL strucutre");
    return EXIT_FAILURE;
  }
#endif

  if (sdLaMa091->Mt != NULL)
    free(sdLaMa091->Mt);
  if (sdLaMa091->Ot != NULL)
    free(sdLaMa091->Ot);
  if (sdLaMa091->Vt != NULL)
    free(sdLaMa091->Vt);

  free(sdLaMa091);

  return EXIT_SUCCESS;
}
