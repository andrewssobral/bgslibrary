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
#include <assert.h>

#include "two_points.h"

static unsigned int abs_uint(const int i)
{
  return (i >= 0) ? i : -i;
}

struct twopointsModel
{
  /* Parameters. */
  uint32_t width;
  uint32_t height;
  uint32_t numberOfSamples;
  uint32_t matchingThreshold;
  uint32_t matchingNumber;
  uint32_t updateFactor;

  /* Storage for the history. */
  uint8_t *historyImage1;
  uint8_t *historyImage2;

  /* Buffers with random values. */
  uint32_t *jump;
  int *neighbor;
};

// -----------------------------------------------------------------------------
// Creates the data structure
// -----------------------------------------------------------------------------
twopointsModel_t *libtwopointsModel_New()
{
  /* Model structure alloc. */
  twopointsModel_t *model = NULL;
  model = (twopointsModel_t*)calloc(1, sizeof(*model));
  assert(model != NULL);

  /* Default parameters values. */
  model->matchingThreshold = 20;
  model->updateFactor = 16;

  /* Storage for the history. */
  model->historyImage1 = NULL;
  model->historyImage2 = NULL;

  /* Buffers with random values. */
  model->jump = NULL;
  model->neighbor = NULL;

  return(model);
}

// ----------------------------------------------------------------------------
// Frees the structure
// ----------------------------------------------------------------------------
int32_t libtwopointsModel_Free(twopointsModel_t *model)
{
  if (model == NULL)
    return(-1);

  if (model->historyImage1 != NULL) {
    free(model->historyImage1);
    model->historyImage1 = NULL;
  }
  if (model->historyImage2 != NULL) {
    free(model->historyImage2);
    model->historyImage2 = NULL;
  }
  if (model->jump != NULL) {
    free(model->jump);
    model->jump = NULL;
  }
  if (model->neighbor != NULL) {
    free(model->neighbor);
    model->neighbor = NULL;
  }
  free(model);

  return(0);
}

// -----------------------------------------------------------------------------
// Allocates and initializes a C1R model structure
// -----------------------------------------------------------------------------
int32_t libtwopointsModel_AllocInit_8u_C1R(
  twopointsModel_t *model,
  const uint8_t *image_data,
  const uint32_t width,
  const uint32_t height
)
{
  // Some basic checks. */
  assert((image_data != NULL) && (model != NULL));
  assert((width > 0) && (height > 0));

  /* Finish model alloc - parameters values cannot be changed anymore. */
  model->width = width;
  model->height = height;

  /* Creates the historyImage structure. */
  model->historyImage1 = NULL;
  model->historyImage1 = (uint8_t*)malloc(width * height * sizeof(uint8_t));
  model->historyImage2 = NULL;
  model->historyImage2 = (uint8_t*)malloc(width * height * sizeof(uint8_t));

  assert(model->historyImage1 != NULL);
  assert(model->historyImage2 != NULL);

  for (int index = width * height - 1; index >= 0; --index) {
    uint8_t value = image_data[index];

    int value_plus_noise = value - 10;
    if (value_plus_noise < 0) { value_plus_noise = 0; }
    if (value_plus_noise > 255) { value_plus_noise = 255; }
    model->historyImage1[index] = value_plus_noise;

    value_plus_noise = value + 10;
    if (value_plus_noise < 0) { value_plus_noise = 0; }
    if (value_plus_noise > 255) { value_plus_noise = 255; }
    model->historyImage2[index] = value_plus_noise;

    // Swaps the two values if needed
    if (model->historyImage1[index] > model->historyImage2[index]) {
      uint8_t val = model->historyImage1[index];
      model->historyImage1[index] = model->historyImage2[index];
      model->historyImage2[index] = val;
    }
  }

  /* Fills the buffers with random values. */
  int size = (width > height) ? 2 * width + 1 : 2 * height + 1;

  model->jump = (uint32_t*)malloc(size * sizeof(*(model->jump)));
  assert(model->jump != NULL);

  model->neighbor = (int*)malloc(size * sizeof(*(model->neighbor)));
  assert(model->neighbor != NULL);


  for (int i = 0; i < size; ++i) {
    model->jump[i] = (rand() % (2 * model->updateFactor)) + 1;            // Values between 1 and 2 * updateFactor.
    model->neighbor[i] = ((rand() % 3) - 1) + ((rand() % 3) - 1) * width; // Values between { -width - 1, ... , width + 1 }.
  }

  return(0);
}

// -----------------------------------------------------------------------------
// Segmentation of a C1R model
// -----------------------------------------------------------------------------
int32_t libtwopointsModel_Segmentation_8u_C1R(
  twopointsModel_t *model,
  const uint8_t *image_data,
  uint8_t *segmentation_map
)
{
  assert((image_data != NULL) && (model != NULL) && (segmentation_map != NULL));
  assert((model->width > 0) && (model->height > 0));
  assert((model->jump != NULL) && (model->neighbor != NULL));

  /* Some variables. */
  uint32_t width = model->width;
  uint32_t height = model->height;
  uint32_t matchingThreshold = model->matchingThreshold;

  uint8_t *historyImage1 = model->historyImage1;
  uint8_t *historyImage2 = model->historyImage2;

  /* Segmentation. */
  memset(segmentation_map, 0, width * height);

  uint8_t *first = historyImage1;
  for (int index = width * height - 1; index >= 0; --index) {
    // We adapt the threshold
    matchingThreshold = model->matchingThreshold;
    if (matchingThreshold < abs_uint(historyImage2[index] - historyImage1[index])) {
      matchingThreshold = abs_uint(historyImage2[index] - historyImage1[index]);
    }
    if (abs_uint(image_data[index] - first[index]) <= matchingThreshold)
      segmentation_map[index]++;
  }

  first = historyImage2;
  for (int index = width * height - 1; index >= 0; --index) {
    // We adapt the threshold
    matchingThreshold = model->matchingThreshold;
    if (matchingThreshold < abs_uint(historyImage2[index] - historyImage1[index])) {
      matchingThreshold = abs_uint(historyImage2[index] - historyImage1[index]);
    }
    if (abs_uint(image_data[index] - first[index]) <= matchingThreshold)
      segmentation_map[index]++;
  }

  return(0);
}

// ----------------------------------------------------------------------------
// Update a C1R model
// ----------------------------------------------------------------------------
int doUpdate(const uint8_t value)
{
  if (value == 0) return 0;
  else return 1;
}


int32_t libtwopointsModel_Update_8u_C1R(
  twopointsModel_t *model,
  const uint8_t *image_data,
  uint8_t *updating_mask
)
{
  assert((image_data != NULL) && (model != NULL) && (updating_mask != NULL));
  assert((model->width > 0) && (model->height > 0));
  assert((model->jump != NULL) && (model->neighbor != NULL));

  // Some variables.
  uint32_t width = model->width;
  uint32_t height = model->height;

  uint8_t *historyImage1 = model->historyImage1;
  uint8_t *historyImage2 = model->historyImage2;

  // Updating.
  uint32_t *jump = model->jump;
  int *neighbor = model->neighbor;

  // All the frame, except the border.
  uint32_t shift, indX, indY;
  unsigned int x, y;

  for (y = 1; y < height - 1; ++y) {
    shift = rand() % width;
    indX = jump[shift]; // index_jump should never be zero (> 1).

    while (indX < width - 1) {
      int index = indX + y * width;

      if (doUpdate(updating_mask[index])) {
        uint8_t value = image_data[index];
        // In-place substitution.
        // if (2*value < (historyImage1[index]+historyImage2[index]) ) {
        if (rand() % 2 == 0) {
          historyImage1[index] = value;
        }
        else {
          historyImage2[index] = value;
        }

        // Propagation
        int index_neighbor = index + neighbor[shift];
        if (2 * value < (historyImage1[index_neighbor] + historyImage2[index_neighbor])) {
          // if (rand()%2 == 0 ) {
          historyImage1[index_neighbor] = value;
        }
        else {
          historyImage2[index_neighbor] = value;
        }
      }

      ++shift;
      indX += jump[shift];
    }
  }

  // First row.
  y = 0;
  shift = rand() % width;
  indX = jump[shift]; // index_jump should never be zero (> 1).

  while (indX <= width - 1) {
    int index = indX + y * width;

    if (doUpdate(updating_mask[index])) {
      uint8_t value = image_data[index];
      // In-place substitution.
      // if (2*value < (historyImage1[index]+historyImage2[index]) ) {
      if (rand() % 2 == 0) {
        historyImage1[index] = value;
      }
      else {
        historyImage2[index] = value;
      }
    }

    ++shift;
    indX += jump[shift];
  }

  // Last row.
  y = height - 1;
  shift = rand() % width;
  indX = jump[shift]; // index_jump should never be zero (> 1).

  while (indX <= width - 1) {
    int index = indX + y * width;

    if (doUpdate(updating_mask[index])) {
      uint8_t value = image_data[index];
      // In-place substitution.
      // if (2*value < (historyImage1[index]+historyImage2[index]) ) {
      if (rand() % 2 == 0) {
        historyImage1[index] = value;
      }
      else {
        historyImage2[index] = value;
      }
    }

    ++shift;
    indX += jump[shift];
  }

  // First column.
  x = 0;
  shift = rand() % height;
  indY = jump[shift]; // index_jump should never be zero (> 1).

  while (indY <= height - 1) {
    int index = x + indY * width;

    if (doUpdate(updating_mask[index])) {
      uint8_t value = image_data[index];
      // In-place substitution.
      // if (2*value < (historyImage1[index]+historyImage2[index]) ) {
      if (rand() % 2 == 0) {
        historyImage1[index] = value;
      }
      else {
        historyImage2[index] = value;
      }
    }

    ++shift;
    indY += jump[shift];
  }

  // Last column.
  x = width - 1;
  shift = rand() % height;
  indY = jump[shift]; // index_jump should never be zero (> 1).

  while (indY <= height - 1) {
    int index = x + indY * width;

    if (doUpdate(updating_mask[index])) {
      uint8_t value = image_data[index];
      // In-place substitution.
      // if (2*value < (historyImage1[index]+historyImage2[index]) ) {
      if (rand() % 2 == 0) {
        historyImage1[index] = value;
      }
      else {
        historyImage2[index] = value;
      }
    }

    ++shift;
    indY += jump[shift];
  }

  // The first pixel!
  if (rand() % model->updateFactor == 0) {
    if (doUpdate(updating_mask[0])) {
      uint8_t value = image_data[0];
      // In-place substitution.
      if (rand() % 2 == 0) {
        historyImage1[0] = value;
      }
      else {
        historyImage2[0] = value;
      }
    }
  }

  return(0);
}
