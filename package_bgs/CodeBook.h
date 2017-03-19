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

#include <opencv2/opencv.hpp>

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    struct codeword {
      float min;
      float max;
      float f;
      float l;
      int first;
      int last;
      bool isStale;
    };

    class CodeBook : public IBGS
    {
    private:
      static const int Tdel = 200;
      static const int Tadd = 150;
      static const int Th = 200;
      const int DEFAULT_ALPHA = 10;
      const float DEFAULT_BETA = 1.;
      const int DEFAULT_LEARNFRAMES = 10;
      int t = 0;
      int learningFrames = 10;
      int alpha = 10;
      float beta = 1;
      std::vector<codeword> **cbMain;
      std::vector<codeword> **cbCache;

    public:
      CodeBook();
      ~CodeBook();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void initializeCodebook(int w, int h);
      void update_cb(const cv::Mat& frame);
      void fg_cb(const cv::Mat& frame, cv::Mat& fg);

      void saveConfig();
      void loadConfig();
    };
  }
}
