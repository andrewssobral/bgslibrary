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

#include "TwoPoints.h"

using namespace bgslibrary::algorithms;

TwoPoints::TwoPoints() :
  matchingThreshold(DEFAULT_MATCH_THRESH),
  updateFactor(DEFAULT_UPDATE_FACTOR), model(nullptr)
{
  std::cout << "TwoPoints()" << std::endl;
  //model = static_cast<twopointsModel_t*>(libtwopointsModel_New());
  model = libtwopointsModel_New();
  setup("./config/TwoPoints.xml");
}

TwoPoints::~TwoPoints()
{
  std::cout << "~TwoPoints()" << std::endl;
  libtwopointsModel_Free(model);
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

  if (firstTime)
  {
    // Create a buffer for the output image.
    //img_output = Mat(img_input.rows, img_input.cols, CV_8UC1);

    // Initialization of the ViBe model.
    libtwopointsModel_AllocInit_8u_C1R(model, img_input_grayscale.data, img_input.cols, img_input.rows);

    // Sets default model values.
    // libvibeModel_Sequential_SetMatchingThreshold(model, matchingThreshold);
    // libvibeModel_Sequential_SetUpdateFactor(model, updateFactor);
  }

  libtwopointsModel_Segmentation_8u_C1R(model, img_input_grayscale.data, img_output.data);

  updatingMask = cv::Mat(img_input.rows, img_input.cols, CV_8UC1);
  // Work on the output and define the updating mask
  for (int i = 0; i < img_input.cols * img_input.rows; i++)
  {
    if (img_output.data[i] == 0) // Foreground pixel
    {
      updatingMask.data[i] = 0;
      img_output.data[i] = 255;
    }
    else // Background
    {
      updatingMask.data[i] = 255;
      img_output.data[i] = 0;
    }
  }

  libtwopointsModel_Update_8u_C1R(model, img_input_grayscale.data, updatingMask.data);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    imshow("Two points", img_output);
#endif

  firstTime = false;
}

void TwoPoints::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "matchingThreshold", matchingThreshold);
  cvWriteInt(fs, "updateFactor", updateFactor);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void TwoPoints::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  matchingThreshold = cvReadRealByName(fs, nullptr, "matchingThreshold", DEFAULT_MATCH_THRESH);
  updateFactor = cvReadRealByName(fs, nullptr, "updateFactor", DEFAULT_UPDATE_FACTOR);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", false);

  cvReleaseFileStorage(&fs);
}
