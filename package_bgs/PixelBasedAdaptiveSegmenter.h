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

#include "IBGS.h"
#include "PBAS/PBAS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class PixelBasedAdaptiveSegmenter : public IBGS
    {
    private:
      PBAS pbas;

      bool enableInputBlur;
      bool enableOutputBlur;

      float alpha;
      float beta;
      int N;
      int Raute_min;
      float R_incdec;
      int R_lower;
      int R_scale;
      float T_dec;
      int T_inc;
      int T_init;
      int T_lower;
      int T_upper;

    public:
      PixelBasedAdaptiveSegmenter();
      ~PixelBasedAdaptiveSegmenter();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };
  }
}
