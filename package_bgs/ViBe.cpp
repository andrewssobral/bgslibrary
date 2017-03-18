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
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  //cvWriteInt(fs, "numberOfSamples", numberOfSamples);
  cvWriteInt(fs, "matchingThreshold", matchingThreshold);
  cvWriteInt(fs, "matchingNumber", matchingNumber);
  cvWriteInt(fs, "updateFactor", updateFactor);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void ViBe::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  //numberOfSamples = cvReadIntByName(fs, nullptr, "numberOfSamples", DEFAULT_NUM_SAMPLES);
  matchingThreshold = cvReadRealByName(fs, nullptr, "matchingThreshold", DEFAULT_MATCH_THRESH);
  matchingNumber = cvReadRealByName(fs, nullptr, "matchingNumber", DEFAULT_MATCH_NUM);
  updateFactor = cvReadRealByName(fs, nullptr, "updateFactor", DEFAULT_UPDATE_FACTOR);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", false);

  cvReleaseFileStorage(&fs);
}
