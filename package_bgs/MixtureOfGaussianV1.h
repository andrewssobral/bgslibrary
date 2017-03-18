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

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/video/background_segm.hpp>

#include "IBGS.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class MixtureOfGaussianV1 : public IBGS
    {
    private:
      cv::BackgroundSubtractorMOG mog;
      double alpha;
      bool enableThreshold;
      int threshold;

    public:
      MixtureOfGaussianV1();
      ~MixtureOfGaussianV1();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };
  }
}

#endif
