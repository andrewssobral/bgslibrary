#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace bgslibrary
{
  namespace algorithms
  {
    namespace twopoints
    {
      const int COLOR_BACKGROUND = 0; /*!< Default label for background pixels */
      const int COLOR_FOREGROUND = 255; /*!< Default label for foreground pixels. Note that some authors chose any value different from 0 instead */

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
    }
  }
}
