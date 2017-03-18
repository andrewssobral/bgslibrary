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
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define COLOR_BACKGROUND   0 /*!< Default label for background pixels */
#define COLOR_FOREGROUND 255 /*!< Default label for foreground pixels. Note that some authors chose any value different from 0 instead */

typedef struct twopointsModel twopointsModel_t;

twopointsModel_t *libtwopointsModel_New();

int32_t libtwopointsModel_Free(twopointsModel_t *model);

int32_t libtwopointsModel_AllocInit_8u_C1R(
  twopointsModel_t *model,
  const uint8_t *image_data,
  const uint32_t width,
  const uint32_t height
);

int32_t libtwopointsModel_Segmentation_8u_C1R(
  twopointsModel_t *model,
  const uint8_t *image_data,
  uint8_t *segmentation_map
);

int32_t libtwopointsModel_Update_8u_C1R(
  twopointsModel_t *model,
  const uint8_t *image_data,
  uint8_t *updating_mask
);
