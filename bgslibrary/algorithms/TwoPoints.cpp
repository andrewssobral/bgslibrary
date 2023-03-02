#include "TwoPoints.h"

using namespace bgslibrary::algorithms;

TwoPoints::TwoPoints() :
  IBGS(quote(TwoPoints)),
  matchingThreshold(DEFAULT_MATCH_THRESH),
  updateFactor(DEFAULT_UPDATE_FACTOR), model(nullptr)
{
  debug_construction(TwoPoints);
  initLoadSaveConfig(algorithmName);
  //model = static_cast<twopointsModel_t*>(libtwopointsModel_New());
  model = twopoints::libtwopointsModel_New();
}

TwoPoints::~TwoPoints() {
  debug_destruction(TwoPoints);
  twopoints::libtwopointsModel_Free(model);
}

void TwoPoints::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_input.empty())
    return;

  cv::Mat updatingMask;
  cv::Mat img_input_grayscale;

  // Convert input image to a grayscale image
  cvtColor(img_input, img_input_grayscale, CV_BGR2GRAY);

  if (firstTime) {
    // Create a buffer for the output image.
    //img_output = Mat(img_input.rows, img_input.cols, CV_8UC1);

    // Initialization of the ViBe model.
    twopoints::libtwopointsModel_AllocInit_8u_C1R(model, img_input_grayscale.data, img_input.cols, img_input.rows);

    // Sets default model values.
    // twopoints::libvibeModel_Sequential_SetMatchingThreshold(model, matchingThreshold);
    // twopoints::libvibeModel_Sequential_SetUpdateFactor(model, updateFactor);
  }

  twopoints::libtwopointsModel_Segmentation_8u_C1R(model, img_input_grayscale.data, img_output.data);

  updatingMask = cv::Mat(img_input.rows, img_input.cols, CV_8UC1);
  // Work on the output and define the updating mask
  for (int i = 0; i < img_input.cols * img_input.rows; i++) {
    if (img_output.data[i] == 0) { // Foreground pixel
      updatingMask.data[i] = 0;
      img_output.data[i] = 255;
    }
    else { // Background
      updatingMask.data[i] = 255;
      img_output.data[i] = 0;
    }
  }

  twopoints::libtwopointsModel_Update_8u_C1R(model, img_input_grayscale.data, updatingMask.data);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_output);
#endif

  firstTime = false;
}

void TwoPoints::save_config(cv::FileStorage &fs) {
  fs << "matchingThreshold" << matchingThreshold;
  fs << "updateFactor" << updateFactor;
  fs << "showOutput" << showOutput;
}

void TwoPoints::load_config(cv::FileStorage &fs) {
  fs["matchingThreshold"] >> matchingThreshold;
  fs["updateFactor"] >> updateFactor;
  fs["showOutput"] >> showOutput;
}
