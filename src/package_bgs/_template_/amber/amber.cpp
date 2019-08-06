#include "amber.h"

#define COLOR_BACKGROUND   0
#define COLOR_FOREGROUND 255

amberModel* libamberModelNew()
{
  /* model structure alloc */
  amberModel* model = NULL;
  model = (amberModel*)calloc(1, sizeof(amberModel));
  assert(model != NULL);

  /* default parameters values */
  model->height = 0;

  return(model);
}

// --------------------------------------------------
int32_t libamberModelAllocInit_8u_C3R(amberModel* model,
  const uint8_t *image_data,
  const uint32_t width,
  const uint32_t height)
{

  /* basic checks */
  assert((image_data != NULL) && (model != NULL));
  assert((model->width > 0) && (model->height > 0));

  /* finish model alloc - parameters values cannot be changed anymore */
  model->width = width;
  model->height = height;

  return(0);
}

// --------------------------------------------------
int32_t libamberGetSegmentation_8u_C3R(amberModel* model,
  const uint8_t *image_data,
  uint8_t *segmentation_map)
{
  /* basic checks */
  assert((image_data != NULL) && (model != NULL) && (segmentation_map != NULL));
  assert((model->width > 0) && (model->height > 0));

  return(0);
}

// --------------------------------------------------
int32_t libamberModelFree(amberModel* model)
{
  if (model == NULL)
    return(-1);

  free(model);

  return(0);
}
