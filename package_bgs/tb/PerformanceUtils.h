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
/*
Code provided by Thierry BOUWMANS

Maitre de Conférences
Laboratoire MIA
Université de La Rochelle
17000 La Rochelle
France
tbouwman@univ-lr.fr

http://sites.google.com/site/thierrybouwmans/
*/
#include <stdio.h>
#include <fstream>
#include <opencv2/opencv.hpp>


#include "PixelUtils.h"

class PerformanceUtils
{
public:
  PerformanceUtils(void);
  ~PerformanceUtils(void);

  float NrPixels(IplImage *image);
  float NrAllDetectedPixNotNULL(IplImage *image, IplImage *ground_truth);
  float NrTruePositives(IplImage *image, IplImage *ground_truth, bool debug = false);
  float NrTrueNegatives(IplImage *image, IplImage *ground_truth, bool debug = false);
  float NrFalsePositives(IplImage *image, IplImage *ground_truth, bool debug = false);
  float NrFalseNegatives(IplImage *image, IplImage *ground_truth, bool debug = false);
  float SimilarityMeasure(IplImage *image, IplImage *ground_truth, bool debug = false);

  void ImageROC(IplImage *image, IplImage* ground_truth, bool saveResults = false, char* filename = "");
  void PerformanceEvaluation(IplImage *image, IplImage *ground_truth, bool saveResults = false, char* filename = "", bool debug = false);
};

