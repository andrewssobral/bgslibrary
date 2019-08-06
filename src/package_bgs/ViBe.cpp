#include "ViBe.h"

using namespace bgslibrary::algorithms;

ViBe::ViBe() :
  //numberOfSamples(DEFAULT_NUM_SAMPLES),
  matchingThreshold(DEFAULT_MATCH_THRESH),
  matchingNumber(DEFAULT_MATCH_NUM),
  updateFactor(DEFAULT_UPDATE_FACTOR),
  model(nullptr)
{
  std::cout << "ViBe()" << std::endl;
  model = libvibeModel_Sequential_New();
  setup("./config/ViBe.xml");
}

ViBe::~ViBe()
{
  std::cout << "~ViBe()" << std::endl;
  libvibeModel_Sequential_Free(model);
}

void ViBe::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_input.empty())
    return;

  if (firstTime)
  {
    /* Create a buffer for the output image. */
    //img_output = cv::Mat(img_input.rows, img_input.cols, CV_8UC1);

    /* Initialization of the ViBe model. */
    libvibeModel_Sequential_AllocInit_8u_C3R(model, img_input.data, img_input.cols, img_input.rows);

    /* Sets default model values. */
    //libvibeModel_Sequential_SetNumberOfSamples(model, numberOfSamples);
    libvibeModel_Sequential_SetMatchingThreshold(model, matchingThreshold);
    libvibeModel_Sequential_SetMatchingNumber(model, matchingNumber);
    libvibeModel_Sequential_SetUpdateFactor(model, updateFactor);
  }

  libvibeModel_Sequential_Segmentation_8u_C3R(model, img_input.data, img_output.data);
  //libvibeModel_Sequential_Update_8u_C3R(model, model_img_input.data, img_output.data);
  libvibeModel_Sequential_Update_8u_C3R(model, img_input.data, img_output.data);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    imshow("ViBe", img_output);
#endif

  firstTime = false;
}

void ViBe::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  //fs << "numberOfSamples" << numberOfSamples;
  fs << "matchingThreshold" << matchingThreshold;
  fs << "matchingNumber" << matchingNumber;
  fs << "updateFactor" << updateFactor;
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void ViBe::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  //fs["numberOfSamples"] >> numberOfSamples;
  fs["matchingThreshold"] >> matchingThreshold;
  fs["matchingNumber"] >> matchingNumber;
  fs["updateFactor"] >> updateFactor;
  fs["showOutput"] >> showOutput;
  
  fs.release();
}
