#include "KNN.h"

#if CV_MAJOR_VERSION >= 3

using namespace bgslibrary::algorithms;

KNN::KNN() :
  IBGS(quote(KNN)),
  history(10), dist2Threshold(20.0f * 20.0f), shadowThreshold(0.5f)
{
  debug_construction(KNN);
  initLoadSaveConfig(algorithmName);
}

KNN::~KNN() {
  debug_destruction(KNN);
}

void KNN::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  //------------------------------------------------------------------
  // BackgroundSubtractorKNN
  // http://docs.opencv.org/trunk/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractorknn
  //
  //  The class implements the K nearest neigbours algorithm from:
  //  "Efficient Adaptive Density Estimation per Image Pixel for the Task of Background Subtraction"
  //  Z.Zivkovic, F. van der Heijden
  //  Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006
  //  http:  //www.zoranz.net/Publications/zivkovicPRL2006.pdf
  //
  //  Fast for small foreground object. Results on the benchmark data is at http://www.changedetection.net.
  //------------------------------------------------------------------

  if (firstTime)
    knn = cv::createBackgroundSubtractorKNN(); // history, dist2Threshold, doShadowDetection

  knn->setHistory(history);
  knn->setDist2Threshold(dist2Threshold);
  knn->setShadowThreshold(shadowThreshold);

  //knn->apply(img_input, img_foreground, nSamples >= knnSamples ? 0.f : 1.f);
  knn->apply(img_input, img_foreground);
  knn->getBackgroundImage(img_background);

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_FG", img_foreground);
    cv::imshow(algorithmName + "_BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void KNN::save_config(cv::FileStorage &fs) {
  fs << "history" << history;
  fs << "dist2Threshold" << dist2Threshold;
  fs << "shadowThreshold" << shadowThreshold;
  fs << "showOutput" << showOutput;
}

void KNN::load_config(cv::FileStorage &fs) {
  fs["history"] >> history;
  fs["dist2Threshold"] >> dist2Threshold;
  fs["shadowThreshold"] >> shadowThreshold;
  fs["showOutput"] >> showOutput;
}

#endif
