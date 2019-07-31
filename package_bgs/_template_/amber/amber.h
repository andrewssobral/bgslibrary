#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define COLOR_BACKGROUND   0
#define COLOR_FOREGROUND 255

/* For compilation with g++ */
#ifdef __cplusplus
extern "C"
{
#endif 
  /* end of addition for compilation with g++ */

#ifndef _STDINT_H  
// This is used to make it compatible for cross-compilation
  typedef unsigned char uint8_t;
  typedef int int32_t;
  typedef unsigned int uint32_t;
#endif // _STDINT_H 

  typedef struct {
    uint32_t width;
    uint32_t height;
  } amberModel;

  /** Allocation of a new data structure where the background model
     will be stored */
  amberModel* libamberModelNew();

  int32_t libamberModelAllocInit_8u_C3R(amberModel* model,
    const uint8_t *image_data,
    const uint32_t width,
    const uint32_t height);

  int32_t libamberGetSegmentation_8u_C3R(amberModel* model,
    const uint8_t *image_data,
    uint8_t *segmentation_map);


  int32_t libamberModelFree(amberModel* model);

  /* For compilation with g++ */
#ifdef __cplusplus
}
#endif 
