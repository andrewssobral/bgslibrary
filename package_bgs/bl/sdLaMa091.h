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
#ifndef SD_LA_MA_091_H_
#define SD_LA_MA_091_H_

#include <errno.h>
#include "stdbool.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BACKGROUND 0
#define FOREGROUND 255
#define ERROR_OCCURED 1

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

#endif
