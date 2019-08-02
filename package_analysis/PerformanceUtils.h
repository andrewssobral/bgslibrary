#pragma once

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

  void ImageROC(IplImage *image, IplImage* ground_truth, bool saveResults = false, std::string filename = "");
  void PerformanceEvaluation(IplImage *image, IplImage *ground_truth, bool saveResults = false, std::string filename = "", bool debug = false);
};
