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
@file vibe-background-sequential.c
@brief Implementation of vibe-background-sequential.h
@author Marc Van Droogenbroeck
@date May 2014
*/
#include <assert.h>
#include <time.h>

#include "vibe-background-sequential.h"

#define NUMBER_OF_HISTORY_IMAGES 2

uint32_t distance_Han2014Improved(uint8_t pixel, uint8_t bg)
{
  uint8_t min, max;

  // Computes R = 0.13 min{ max[bg,26], 230}
  max = 26;
  if (bg > max) { max = bg; }

  min = 230;
  if (min > max) { min = max; }

  return (uint32_t)(0.13*min);
}

static int abs_uint(const int i)
{
  return (i >= 0) ? i : -i;
}

static int32_t distance_is_close_8u_C3R(uint8_t r1, uint8_t g1, uint8_t b1, uint8_t r2, uint8_t g2, uint8_t b2, uint32_t threshold)
{
  return (abs_uint(r1 - r2) + abs_uint(g1 - g2) + abs_uint(b1 - b2) <= 4.5 * threshold);
}

struct vibeModel_Sequential
{
  /* Parameters. */
  uint32_t width;
  uint32_t height;
  uint32_t numberOfSamples;
  uint32_t matchingThreshold;
  uint32_t matchingNumber;
  uint32_t updateFactor;

  /* Storage for the history. */
  uint8_t *historyImage;
  uint8_t *historyBuffer;
  uint32_t lastHistoryImageSwapped;

  /* Buffers with random values. */
  uint32_t *jump;
  int *neighbor;
  uint32_t *position;
};

// -----------------------------------------------------------------------------
// Print parameters
// -----------------------------------------------------------------------------
uint32_t libvibeModel_Sequential_PrintParameters(const vibeModel_Sequential_t *model)
{
  printf(
    "Using ViBe background subtraction algorithm\n"
    "  - Number of samples per pixel:       %03d\n"
    "  - Number of matches needed:          %03d\n"
    "  - Matching threshold:                %03d\n"
    "  - Model update subsampling factor:   %03d\n",
    libvibeModel_Sequential_GetNumberOfSamples(model),
    libvibeModel_Sequential_GetMatchingNumber(model),
    libvibeModel_Sequential_GetMatchingThreshold(model),
    libvibeModel_Sequential_GetUpdateFactor(model)
  );

  return(0);
}

// -----------------------------------------------------------------------------
// Creates the data structure
// -----------------------------------------------------------------------------
vibeModel_Sequential_t *libvibeModel_Sequential_New()
{
  /* Model structure alloc. */
  vibeModel_Sequential_t *model = NULL;
  model = (vibeModel_Sequential_t*)calloc(1, sizeof(*model));
  assert(model != NULL);

  /* Default parameters values. */
  model->numberOfSamples = 20;
  model->matchingThreshold = 20;
  model->matchingNumber = 2;
  model->updateFactor = 16;

  /* Storage for the history. */
  model->historyImage = NULL;
  model->historyBuffer = NULL;
  model->lastHistoryImageSwapped = 0;

  /* Buffers with random values. */
  model->jump = NULL;
  model->neighbor = NULL;
  model->position = NULL;

  return(model);
}

// -----------------------------------------------------------------------------
// Some "Get-ers"
// -----------------------------------------------------------------------------
uint32_t libvibeModel_Sequential_GetNumberOfSamples(const vibeModel_Sequential_t *model)
{
  assert(model != NULL); return(model->numberOfSamples);
}

uint32_t libvibeModel_Sequential_GetMatchingNumber(const vibeModel_Sequential_t *model)
{
  assert(model != NULL); return(model->matchingNumber);
}

uint32_t libvibeModel_Sequential_GetMatchingThreshold(const vibeModel_Sequential_t *model)
{
  assert(model != NULL); return(model->matchingThreshold);
}

uint32_t libvibeModel_Sequential_GetUpdateFactor(const vibeModel_Sequential_t *model)
{
  assert(model != NULL); return(model->updateFactor);
}

// -----------------------------------------------------------------------------
// Some "Set-ers"
// -----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_SetMatchingThreshold(
  vibeModel_Sequential_t *model,
  const uint32_t matchingThreshold
) {
  assert(model != NULL);
  assert(matchingThreshold > 0);

  model->matchingThreshold = matchingThreshold;

  return(0);
}

// -----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_SetMatchingNumber(
  vibeModel_Sequential_t *model,
  const uint32_t matchingNumber
) {
  assert(model != NULL);
  assert(matchingNumber > 0);

  model->matchingNumber = matchingNumber;

  return(0);
}

// -----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_SetUpdateFactor(
  vibeModel_Sequential_t *model,
  const uint32_t updateFactor
) {
  assert(model != NULL);
  assert(updateFactor > 0);

  model->updateFactor = updateFactor;

  /* We also need to change the values of the jump buffer ! */
  assert(model->jump != NULL);

  /* Shifts. */
  int size = (model->width > model->height) ? 2 * model->width + 1 : 2 * model->height + 1;

  for (int i = 0; i < size; ++i)
    model->jump[i] = (updateFactor == 1) ? 1 : (rand() % (2 * model->updateFactor)) + 1; // 1 or values between 1 and 2 * updateFactor.

  return(0);
}

// ----------------------------------------------------------------------------
// Frees the structure
// ----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_Free(vibeModel_Sequential_t *model)
{
  if (model == NULL)
    return(-1);

  if (model->historyBuffer == NULL) {
    free(model);
    return(0);
  }

  free(model->historyImage);
  free(model->historyBuffer);
  free(model->jump);
  free(model->neighbor);
  free(model->position);
  free(model);

  return(0);
}

// -----------------------------------------------------------------------------
// Allocates and initializes a C1R model structure
// -----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_AllocInit_8u_C1R(
  vibeModel_Sequential_t *model,
  const uint8_t *image_data,
  const uint32_t width,
  const uint32_t height
) {
  // Some basic checks. */
  assert((image_data != NULL) && (model != NULL));
  assert((width > 0) && (height > 0));

  /* Finish model alloc - parameters values cannot be changed anymore. */
  model->width = width;
  model->height = height;

  /* Creates the historyImage structure. */
  model->historyImage = NULL;
  model->historyImage = (uint8_t*)malloc(NUMBER_OF_HISTORY_IMAGES * width * height * sizeof(*(model->historyImage)));

  assert(model->historyImage != NULL);

  for (int i = 0; i < NUMBER_OF_HISTORY_IMAGES; ++i) {
    for (int index = width * height - 1; index >= 0; --index)
      model->historyImage[i * width * height + index] = image_data[index];
  }

  /* Now creates and fills the history buffer. */
  model->historyBuffer = (uint8_t*)malloc(width * height * (model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES) * sizeof(uint8_t));
  assert(model->historyBuffer != NULL);

  for (int index = width * height - 1; index >= 0; --index) {
    uint8_t value = image_data[index];

    for (int x = 0; x < model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES; ++x) {
      int value_plus_noise = value + rand() % 20 - 10;

      if (value_plus_noise < 0) { value_plus_noise = 0; }
      if (value_plus_noise > 255) { value_plus_noise = 255; }

      model->historyBuffer[index * (model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES) + x] = value_plus_noise;
    }
  }

  /* Fills the buffers with random values. */
  int size = (width > height) ? 2 * width + 1 : 2 * height + 1;

  model->jump = (uint32_t*)malloc(size * sizeof(*(model->jump)));
  assert(model->jump != NULL);

  model->neighbor = (int*)malloc(size * sizeof(*(model->neighbor)));
  assert(model->neighbor != NULL);

  model->position = (uint32_t*)malloc(size * sizeof(*(model->position)));
  assert(model->position != NULL);

  for (int i = 0; i < size; ++i) {
    model->jump[i] = (rand() % (2 * model->updateFactor)) + 1;            // Values between 1 and 2 * updateFactor.
    model->neighbor[i] = ((rand() % 3) - 1) + ((rand() % 3) - 1) * width; // Values between { -width - 1, ... , width + 1 }.
    model->position[i] = rand() % (model->numberOfSamples);               // Values between 0 and numberOfSamples - 1.
  }

  return(0);
}

// -----------------------------------------------------------------------------
// Segmentation of a C1R model
// -----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_Segmentation_8u_C1R(
  vibeModel_Sequential_t *model,
  const uint8_t *image_data,
  uint8_t *segmentation_map
) {
  /* Basic checks. */
  assert((image_data != NULL) && (model != NULL) && (segmentation_map != NULL));
  assert((model->width > 0) && (model->height > 0));
  assert(model->historyBuffer != NULL);
  assert((model->jump != NULL) && (model->neighbor != NULL) && (model->position != NULL));

  /* Some variables. */
  uint32_t width = model->width;
  uint32_t height = model->height;
  uint32_t matchingNumber = model->matchingNumber;
  //uint32_t matchingThreshold = model->matchingThreshold;

  uint8_t *historyImage = model->historyImage;
  uint8_t *historyBuffer = model->historyBuffer;

  /* Segmentation. */
  memset(segmentation_map, matchingNumber - 1, width * height);

  /* First history Image structure. */
  for (int index = width * height - 1; index >= 0; --index) {
    //if (abs_uint(image_data[index] - historyImage[index]) > matchingThreshold)
    if (abs_uint(image_data[index] - historyImage[index]) > distance_Han2014Improved(image_data[index], historyImage[index]))
      segmentation_map[index] = matchingNumber;
  }

  /* Next historyImages. */
  for (int i = 1; i < NUMBER_OF_HISTORY_IMAGES; ++i) {
    uint8_t *pels = historyImage + i * width * height;

    for (int index = width * height - 1; index >= 0; --index) {
      // if (abs_uint(image_data[index] - pels[index]) <= matchingThreshold)
      if (abs_uint(image_data[index] - pels[index]) <= distance_Han2014Improved(image_data[index], pels[index]))
        --segmentation_map[index];
    }
  }

  /* For swapping. */
  model->lastHistoryImageSwapped = (model->lastHistoryImageSwapped + 1) % NUMBER_OF_HISTORY_IMAGES;
  uint8_t *swappingImageBuffer = historyImage + (model->lastHistoryImageSwapped) * width * height;

  /* Now, we move in the buffer and leave the historyImages. */
  int numberOfTests = (model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES);

  for (int index = width * height - 1; index >= 0; --index) {
    if (segmentation_map[index] > 0) {
      /* We need to check the full border and swap values with the first or second historyImage.
       * We still need to find a match before we can stop our search.
       */
      uint32_t indexHistoryBuffer = index * numberOfTests;
      uint8_t currentValue = image_data[index];

      for (int i = numberOfTests; i > 0; --i, ++indexHistoryBuffer) {
        // if (abs_uint(currentValue - historyBuffer[indexHistoryBuffer]) <= matchingThreshold) {
        if (abs_uint(currentValue - historyBuffer[indexHistoryBuffer]) <= distance_Han2014Improved(currentValue, historyBuffer[indexHistoryBuffer])) {
          --segmentation_map[index];

          /* Swaping: Putting found value in history image buffer. */
          uint8_t temp = swappingImageBuffer[index];
          swappingImageBuffer[index] = historyBuffer[indexHistoryBuffer];
          historyBuffer[indexHistoryBuffer] = temp;

          /* Exit inner loop. */
          if (segmentation_map[index] <= 0) break;
        }
      } // for
    } // if
  } // for

  /* Produces the output. Note that this step is application-dependent. */
  for (uint8_t *mask = segmentation_map; mask < segmentation_map + (width * height); ++mask)
    if (*mask > 0) *mask = COLOR_FOREGROUND;

  return(0);
}

// ----------------------------------------------------------------------------
// Update a C1R model
// ----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_Update_8u_C1R(
  vibeModel_Sequential_t *model,
  const uint8_t *image_data,
  uint8_t *updating_mask
) {
  /* Basic checks . */
  assert((image_data != NULL) && (model != NULL) && (updating_mask != NULL));
  assert((model->width > 0) && (model->height > 0));
  assert(model->historyBuffer != NULL);
  assert((model->jump != NULL) && (model->neighbor != NULL) && (model->position != NULL));

  /* Some variables. */
  uint32_t width = model->width;
  uint32_t height = model->height;

  uint8_t *historyImage = model->historyImage;
  uint8_t *historyBuffer = model->historyBuffer;

  /* Some utility variable. */
  int numberOfTests = (model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES);

  /* Updating. */
  uint32_t *jump = model->jump;
  int *neighbor = model->neighbor;
  uint32_t *position = model->position;

  /* All the frame, except the border. */
  uint32_t shift, indX, indY;
  unsigned int x, y;

  for (y = 1; y < height - 1; ++y) {
    shift = rand() % width;
    indX = jump[shift]; // index_jump should never be zero (> 1).

    while (indX < width - 1) {
      int index = indX + y * width;

      if (updating_mask[index] == COLOR_BACKGROUND) {
        /* In-place substitution. */
        uint8_t value = image_data[index];
        int index_neighbor = index + neighbor[shift];

        if (position[shift] < NUMBER_OF_HISTORY_IMAGES) {
          historyImage[index + position[shift] * width * height] = value;
          historyImage[index_neighbor + position[shift] * width * height] = value;
        }
        else {
          int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;
          historyBuffer[index * numberOfTests + pos] = value;
          historyBuffer[index_neighbor * numberOfTests + pos] = value;
        }
      }

      ++shift;
      indX += jump[shift];
    }
  }

  /* First row. */
  y = 0;
  shift = rand() % width;
  indX = jump[shift]; // index_jump should never be zero (> 1).

  while (indX <= width - 1) {
    int index = indX + y * width;

    if (updating_mask[index] == COLOR_BACKGROUND) {
      if (position[shift] < NUMBER_OF_HISTORY_IMAGES)
        historyImage[index + position[shift] * width * height] = image_data[index];
      else {
        int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;
        historyBuffer[index * numberOfTests + pos] = image_data[index];
      }
    }

    ++shift;
    indX += jump[shift];
  }

  /* Last row. */
  y = height - 1;
  shift = rand() % width;
  indX = jump[shift]; // index_jump should never be zero (> 1).

  while (indX <= width - 1) {
    int index = indX + y * width;

    if (updating_mask[index] == COLOR_BACKGROUND) {
      if (position[shift] < NUMBER_OF_HISTORY_IMAGES)
        historyImage[index + position[shift] * width * height] = image_data[index];
      else {
        int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;
        historyBuffer[index * numberOfTests + pos] = image_data[index];
      }
    }

    ++shift;
    indX += jump[shift];
  }

  /* First column. */
  x = 0;
  shift = rand() % height;
  indY = jump[shift]; // index_jump should never be zero (> 1).

  while (indY <= height - 1) {
    int index = x + indY * width;

    if (updating_mask[index] == COLOR_BACKGROUND) {
      if (position[shift] < NUMBER_OF_HISTORY_IMAGES)
        historyImage[index + position[shift] * width * height] = image_data[index];
      else {
        int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;
        historyBuffer[index * numberOfTests + pos] = image_data[index];
      }
    }

    ++shift;
    indY += jump[shift];
  }

  /* Last column. */
  x = width - 1;
  shift = rand() % height;
  indY = jump[shift]; // index_jump should never be zero (> 1).

  while (indY <= height - 1) {
    int index = x + indY * width;

    if (updating_mask[index] == COLOR_BACKGROUND) {
      if (position[shift] < NUMBER_OF_HISTORY_IMAGES)
        historyImage[index + position[shift] * width * height] = image_data[index];
      else {
        int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;
        historyBuffer[index * numberOfTests + pos] = image_data[index];
      }
    }

    ++shift;
    indY += jump[shift];
  }

  /* The first pixel! */
  if (rand() % model->updateFactor == 0) {
    if (updating_mask[0] == 0) {
      int position = rand() % model->numberOfSamples;

      if (position < NUMBER_OF_HISTORY_IMAGES)
        historyImage[position * width * height] = image_data[0];
      else {
        int pos = position - NUMBER_OF_HISTORY_IMAGES;
        historyBuffer[pos] = image_data[0];
      }
    }
  }

  return(0);
}

// ----------------------------------------------------------------------------
// -------------------------- The same for C3R models -------------------------
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Allocates and initializes a C3R model structure
// -----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_AllocInit_8u_C3R(
  vibeModel_Sequential_t *model,
  const uint8_t *image_data,
  const uint32_t width,
  const uint32_t height
) {
  /* Some basic checks. */
  assert((image_data != NULL) && (model != NULL));
  assert((width > 0) && (height > 0));

  /* Finish model alloc - parameters values cannot be changed anymore. */
  model->width = width;
  model->height = height;

  /* Creates the historyImage structure. */
  model->historyImage = NULL;
  model->historyImage = (uint8_t*)malloc(NUMBER_OF_HISTORY_IMAGES * (3 * width) * height * sizeof(uint8_t));
  assert(model->historyImage != NULL);

  for (int i = 0; i < NUMBER_OF_HISTORY_IMAGES; ++i) {
    for (int index = (3 * width) * height - 1; index >= 0; --index)
      model->historyImage[i * (3 * width) * height + index] = image_data[index];
  }

  assert(model->historyImage != NULL);

  /* Now creates and fills the history buffer. */
  model->historyBuffer = (uint8_t *)malloc((3 * width) * height * (model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES) * sizeof(uint8_t));
  assert(model->historyBuffer != NULL);

  for (int index = (3 * width) * height - 1; index >= 0; --index) {
    uint8_t value = image_data[index];

    for (int x = 0; x < model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES; ++x) {
      int value_plus_noise = value + rand() % 20 - 10;

      if (value_plus_noise < 0) { value_plus_noise = 0; }
      if (value_plus_noise > 255) { value_plus_noise = 255; }

      model->historyBuffer[index * (model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES) + x] = value_plus_noise;
    }
  }

  /* Fills the buffers with random values. */
  int size = (width > height) ? 2 * width + 1 : 2 * height + 1;

  model->jump = (uint32_t*)malloc(size * sizeof(*(model->jump)));
  assert(model->jump != NULL);

  model->neighbor = (int*)malloc(size * sizeof(*(model->neighbor)));
  assert(model->neighbor != NULL);

  model->position = (uint32_t*)malloc(size * sizeof(*(model->position)));
  assert(model->position != NULL);

  for (int i = 0; i < size; ++i) {
    model->jump[i] = (rand() % (2 * model->updateFactor)) + 1;            // Values between 1 and 2 * updateFactor.
    model->neighbor[i] = ((rand() % 3) - 1) + ((rand() % 3) - 1) * width; // Values between { width - 1, ... , width + 1 }.
    model->position[i] = rand() % (model->numberOfSamples);               // Values between 0 and numberOfSamples - 1.
  }

  return(0);
}

// -----------------------------------------------------------------------------
// Segmentation of a C3R model
// -----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_Segmentation_8u_C3R(
  vibeModel_Sequential_t *model,
  const uint8_t *image_data,
  uint8_t *segmentation_map
) {
  /* Basic checks. */
  assert((image_data != NULL) && (model != NULL) && (segmentation_map != NULL));
  assert((model->width > 0) && (model->height > 0));
  assert(model->historyBuffer != NULL);
  assert((model->jump != NULL) && (model->neighbor != NULL) && (model->position != NULL));

  /* Some variables. */
  uint32_t width = model->width;
  uint32_t height = model->height;
  uint32_t matchingNumber = model->matchingNumber;
  uint32_t matchingThreshold = model->matchingThreshold;

  uint8_t *historyImage = model->historyImage;
  uint8_t *historyBuffer = model->historyBuffer;

  /* Segmentation. */
  memset(segmentation_map, matchingNumber - 1, width * height);

  /* First history Image structure. */
  uint8_t *first = historyImage;

  for (int index = width * height - 1; index >= 0; --index) {
    if (
      !distance_is_close_8u_C3R(
        image_data[3 * index], image_data[3 * index + 1], image_data[3 * index + 2],
        first[3 * index], first[3 * index + 1], first[3 * index + 2], matchingThreshold
      )
      )
      segmentation_map[index] = matchingNumber;
  }

  /* Next historyImages. */
  for (int i = 1; i < NUMBER_OF_HISTORY_IMAGES; ++i) {
    uint8_t *pels = historyImage + i * (3 * width) * height;

    for (int index = width * height - 1; index >= 0; --index) {
      if (
        distance_is_close_8u_C3R(
          image_data[3 * index], image_data[3 * index + 1], image_data[3 * index + 2],
          pels[3 * index], pels[3 * index + 1], pels[3 * index + 2], matchingThreshold
        )
        )
        --segmentation_map[index];
    }
  }

  // For swapping
  model->lastHistoryImageSwapped = (model->lastHistoryImageSwapped + 1) % NUMBER_OF_HISTORY_IMAGES;
  uint8_t *swappingImageBuffer = historyImage + (model->lastHistoryImageSwapped) * (3 * width) * height;

  // Now, we move in the buffer and leave the historyImages
  int numberOfTests = (model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES);

  for (int index = width * height - 1; index >= 0; --index) {
    if (segmentation_map[index] > 0) {
      /* We need to check the full border and swap values with the first or second historyImage.
       * We still need to find a match before we can stop our search.
       */
      uint32_t indexHistoryBuffer = (3 * index) * numberOfTests;

      for (int i = numberOfTests; i > 0; --i, indexHistoryBuffer += 3) {
        if (
          distance_is_close_8u_C3R(
            image_data[(3 * index)], image_data[(3 * index) + 1], image_data[(3 * index) + 2],
            historyBuffer[indexHistoryBuffer], historyBuffer[indexHistoryBuffer + 1], historyBuffer[indexHistoryBuffer + 2],
            matchingThreshold
          )
          )
          --segmentation_map[index];

        /* Swaping: Putting found value in history image buffer. */
        uint8_t temp_r = swappingImageBuffer[(3 * index)];
        uint8_t temp_g = swappingImageBuffer[(3 * index) + 1];
        uint8_t temp_b = swappingImageBuffer[(3 * index) + 2];

        swappingImageBuffer[(3 * index)] = historyBuffer[indexHistoryBuffer];
        swappingImageBuffer[(3 * index) + 1] = historyBuffer[indexHistoryBuffer + 1];
        swappingImageBuffer[(3 * index) + 2] = historyBuffer[indexHistoryBuffer + 2];

        historyBuffer[indexHistoryBuffer] = temp_r;
        historyBuffer[indexHistoryBuffer + 1] = temp_g;
        historyBuffer[indexHistoryBuffer + 2] = temp_b;

        /* Exit inner loop. */
        if (segmentation_map[index] <= 0) break;
      } // for
    } // if
  } // for

  /* Produces the output. Note that this step is application-dependent. */
  for (uint8_t *mask = segmentation_map; mask < segmentation_map + (width * height); ++mask)
    if (*mask > 0) *mask = COLOR_FOREGROUND;

  return(0);
}

// ----------------------------------------------------------------------------
// Update a C3R model
// ----------------------------------------------------------------------------
int32_t libvibeModel_Sequential_Update_8u_C3R(
  vibeModel_Sequential_t *model,
  const uint8_t *image_data,
  uint8_t *updating_mask
) {
  /* Basic checks. */
  assert((image_data != NULL) && (model != NULL) && (updating_mask != NULL));
  assert((model->width > 0) && (model->height > 0));
  assert(model->historyBuffer != NULL);
  assert((model->jump != NULL) && (model->neighbor != NULL) && (model->position != NULL));

  /* Some variables. */
  uint32_t width = model->width;
  uint32_t height = model->height;

  uint8_t *historyImage = model->historyImage;
  uint8_t *historyBuffer = model->historyBuffer;

  /* Some utility variable. */
  int numberOfTests = (model->numberOfSamples - NUMBER_OF_HISTORY_IMAGES);

  /* Updating. */
  uint32_t *jump = model->jump;
  int *neighbor = model->neighbor;
  uint32_t *position = model->position;

  /* All the frame, except the border. */
  uint32_t shift, indX, indY;
  int x, y;

  for (y = 1; y < height - 1; ++y) {
    shift = rand() % width;
    indX = jump[shift]; // index_jump should never be zero (> 1).

    while (indX < width - 1) {
      int index = indX + y * width;

      if (updating_mask[index] == COLOR_BACKGROUND) {
        /* In-place substitution. */
        uint8_t r = image_data[3 * index];
        uint8_t g = image_data[3 * index + 1];
        uint8_t b = image_data[3 * index + 2];

        int index_neighbor = 3 * (index + neighbor[shift]);

        if (position[shift] < NUMBER_OF_HISTORY_IMAGES) {
          historyImage[3 * index + position[shift] * (3 * width) * height] = r;
          historyImage[3 * index + position[shift] * (3 * width) * height + 1] = g;
          historyImage[3 * index + position[shift] * (3 * width) * height + 2] = b;

          historyImage[index_neighbor + position[shift] * (3 * width) * height] = r;
          historyImage[index_neighbor + position[shift] * (3 * width) * height + 1] = g;
          historyImage[index_neighbor + position[shift] * (3 * width) * height + 2] = r;
        }
        else {
          int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;

          historyBuffer[(3 * index) * numberOfTests + 3 * pos] = r;
          historyBuffer[(3 * index) * numberOfTests + 3 * pos + 1] = g;
          historyBuffer[(3 * index) * numberOfTests + 3 * pos + 2] = b;

          historyBuffer[index_neighbor * numberOfTests + 3 * pos] = r;
          historyBuffer[index_neighbor * numberOfTests + 3 * pos + 1] = g;
          historyBuffer[index_neighbor * numberOfTests + 3 * pos + 2] = b;
        }
      }

      ++shift;
      indX += jump[shift];
    }
  }

  /* First row. */
  y = 0;
  shift = rand() % width;
  indX = jump[shift]; // index_jump should never be zero (> 1).

  while (indX <= width - 1) {
    int index = indX + y * width;

    uint8_t r = image_data[3 * index];
    uint8_t g = image_data[3 * index + 1];
    uint8_t b = image_data[3 * index + 2];

    if (updating_mask[index] == COLOR_BACKGROUND) {
      if (position[shift] < NUMBER_OF_HISTORY_IMAGES) {
        historyImage[3 * index + position[shift] * (3 * width) * height] = r;
        historyImage[3 * index + position[shift] * (3 * width) * height + 1] = g;
        historyImage[3 * index + position[shift] * (3 * width) * height + 2] = b;
      }
      else {
        int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;

        historyBuffer[(3 * index) * numberOfTests + 3 * pos] = r;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos + 1] = g;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos + 2] = b;
      }
    }

    ++shift;
    indX += jump[shift];
  }

  /* Last row. */
  y = height - 1;
  shift = rand() % width;
  indX = jump[shift]; // index_jump should never be zero (> 1).

  while (indX <= width - 1) {
    int index = indX + y * width;

    uint8_t r = image_data[3 * index];
    uint8_t g = image_data[3 * index + 1];
    uint8_t b = image_data[3 * index + 2];

    if (updating_mask[index] == COLOR_BACKGROUND) {
      if (position[shift] < NUMBER_OF_HISTORY_IMAGES) {
        historyImage[3 * index + position[shift] * (3 * width) * height] = r;
        historyImage[3 * index + position[shift] * (3 * width) * height + 1] = g;
        historyImage[3 * index + position[shift] * (3 * width) * height + 2] = b;
      }
      else {
        int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;

        historyBuffer[(3 * index) * numberOfTests + 3 * pos] = r;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos + 1] = g;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos + 2] = b;
      }
    }

    ++shift;
    indX += jump[shift];
  }

  /* First column. */
  x = 0;
  shift = rand() % height;
  indY = jump[shift]; // index_jump should never be zero (> 1).

  while (indY <= height - 1) {
    int index = x + indY * width;

    uint8_t r = image_data[3 * index];
    uint8_t g = image_data[3 * index + 1];
    uint8_t b = image_data[3 * index + 2];

    if (updating_mask[index] == COLOR_BACKGROUND) {
      if (position[shift] < NUMBER_OF_HISTORY_IMAGES) {
        historyImage[3 * index + position[shift] * (3 * width) * height] = r;
        historyImage[3 * index + position[shift] * (3 * width) * height + 1] = g;
        historyImage[3 * index + position[shift] * (3 * width) * height + 2] = b;
      }
      else {
        int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos] = r;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos + 1] = g;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos + 2] = b;
      }
    }

    ++shift;
    indY += jump[shift];
  }

  /* Last column. */
  x = width - 1;
  shift = rand() % height;
  indY = jump[shift]; // index_jump should never be zero (> 1).

  while (indY <= height - 1) {
    int index = x + indY * width;

    uint8_t r = image_data[3 * index];
    uint8_t g = image_data[3 * index + 1];
    uint8_t b = image_data[3 * index + 2];

    if (updating_mask[index] == COLOR_BACKGROUND) {
      if (position[shift] < NUMBER_OF_HISTORY_IMAGES) {
        historyImage[3 * index + position[shift] * (3 * width) * height] = r;
        historyImage[3 * index + position[shift] * (3 * width) * height + 1] = g;
        historyImage[3 * index + position[shift] * (3 * width) * height + 2] = b;
      }
      else {
        int pos = position[shift] - NUMBER_OF_HISTORY_IMAGES;

        historyBuffer[(3 * index) * numberOfTests + 3 * pos] = r;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos + 1] = g;
        historyBuffer[(3 * index) * numberOfTests + 3 * pos + 2] = b;
      }
    }

    ++shift;
    indY += jump[shift];
  }

  /* The first pixel! */
  if (rand() % model->updateFactor == 0) {
    if (updating_mask[0] == 0) {
      int position = rand() % model->numberOfSamples;

      uint8_t r = image_data[0];
      uint8_t g = image_data[1];
      uint8_t b = image_data[2];

      if (position < NUMBER_OF_HISTORY_IMAGES) {
        historyImage[position * (3 * width) * height] = r;
        historyImage[position * (3 * width) * height + 1] = g;
        historyImage[position * (3 * width) * height + 2] = b;
      }
      else {
        int pos = position - NUMBER_OF_HISTORY_IMAGES;

        historyBuffer[3 * pos] = r;
        historyBuffer[3 * pos + 1] = g;
        historyBuffer[3 * pos + 2] = b;
      }
    }
  }

  return(0);
}
