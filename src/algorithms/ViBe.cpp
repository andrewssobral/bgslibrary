#include "ViBe.h"

using namespace bgslibrary::algorithms;
//using namespace bgslibrary::algorithms::vibe;

ViBe::ViBe() :
  IBGS(quote(ViBe)),
  //numberOfSamples(DEFAULT_NUM_SAMPLES),
  matchingThreshold(DEFAULT_MATCH_THRESH),
  matchingNumber(DEFAULT_MATCH_NUM),
  updateFactor(DEFAULT_UPDATE_FACTOR),
  model(nullptr)
{
  debug_construction(ViBe);
  initLoadSaveConfig(algorithmName);
  model = vibe::libvibeModel_Sequential_New();
}

ViBe::~ViBe() {
  debug_destruction(ViBe);
  vibe::libvibeModel_Sequential_Free(model);
}

void ViBe::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_input.empty())
    return;

  if (firstTime) {
    /* Create a buffer for the output image. */
    //img_output = cv::Mat(img_input.rows, img_input.cols, CV_8UC1);

    /* Initialization of the ViBe model. */
    vibe::libvibeModel_Sequential_AllocInit_8u_C3R(model, img_input.data, img_input.cols, img_input.rows);

    /* Sets default model values. */
    //vibe::libvibeModel_Sequential_SetNumberOfSamples(model, numberOfSamples);
    vibe::libvibeModel_Sequential_SetMatchingThreshold(model, matchingThreshold);
    vibe::libvibeModel_Sequential_SetMatchingNumber(model, matchingNumber);
    vibe::libvibeModel_Sequential_SetUpdateFactor(model, updateFactor);
  }

  vibe::libvibeModel_Sequential_Segmentation_8u_C3R(model, img_input.data, img_output.data);
  //vibe::libvibeModel_Sequential_Update_8u_C3R(model, model_img_input.data, img_output.data);
  vibe::libvibeModel_Sequential_Update_8u_C3R(model, img_input.data, img_output.data);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_output);
#endif

  firstTime = false;
}

void ViBe::save_config(cv::FileStorage &fs) {
  //fs << "numberOfSamples" << numberOfSamples;
  fs << "matchingThreshold" << matchingThreshold;
  fs << "matchingNumber" << matchingNumber;
  fs << "updateFactor" << updateFactor;
  fs << "showOutput" << showOutput;
}

void ViBe::load_config(cv::FileStorage &fs) {
  //fs["numberOfSamples"] >> numberOfSamples;
  fs["matchingThreshold"] >> matchingThreshold;
  fs["matchingNumber"] >> matchingNumber;
  fs["updateFactor"] >> updateFactor;
  fs["showOutput"] >> showOutput;
}
