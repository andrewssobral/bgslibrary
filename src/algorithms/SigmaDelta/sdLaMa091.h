#pragma once

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace bgslibrary
{
  namespace algorithms
  {
    namespace sigmadelta
    {
      const int BACKGROUND = 0;
      const int FOREGROUND = 255;
      const int ERROR_OCCURED = 1;

      typedef struct sdLaMa091 sdLaMa091_t;

      sdLaMa091_t* sdLaMa091New(void);

      int32_t sdLaMa091AllocInit_8u_C1R(sdLaMa091_t* sdLaMa091,
        const uint8_t* image_data,
        const uint32_t width,
        const uint32_t height,
        const uint32_t stride);

      int32_t sdLaMa091AllocInit_8u_C3R(sdLaMa091_t* sdLaMa091,
        const uint8_t* image_data,
        const uint32_t width,
        const uint32_t height,
        const uint32_t stride);

      int32_t sdLaMa091SetAmplificationFactor(sdLaMa091_t* sdLaMa091,
        const uint32_t amplificationFactor);

      uint32_t sdLaMa091GetAmplificationFactor(const sdLaMa091_t* sdLaMa091);

      int32_t sdLaMa091SetMaximalVariance(sdLaMa091_t* sdLaMa091,
        const uint32_t maximalVariance);

      uint32_t sdLaMa091GetMaximalVariance(const sdLaMa091_t* sdLaMa091);

      int32_t sdLaMa091SetMinimalVariance(sdLaMa091_t* sdLaMa091,
        const uint32_t minimalVariance);

      uint32_t sdLaMa091GetMinimalVariance(const sdLaMa091_t* sdLaMa091);

      int32_t sdLaMa091Update_8u_C1R(sdLaMa091_t* sdLaMa091,
        const uint8_t* image_data,
        uint8_t* segmentation_map);

      int32_t sdLaMa091Update_8u_C3R(sdLaMa091_t* sdLaMa091,
        const uint8_t* image_data,
        uint8_t* segmentation_map);

      int32_t sdLaMa091Free(sdLaMa091_t* sdLaMa091);
    }
  }
}
