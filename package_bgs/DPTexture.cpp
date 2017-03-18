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
#include "DPTexture.h"

using namespace bgslibrary::algorithms;

DPTexture::DPTexture()
// : enableFiltering(true)
{
  std::cout << "DPTexture()" << std::endl;
  setup("./config/DPTexture.xml");
}

DPTexture::~DPTexture()
{
  delete[] bgModel; // ~10Kb (25.708-15.968)
  delete[] modeArray;
  delete[] curTextureHist; // ~10Kb (16-6.396)
  //cvReleaseStructuringElement(&dilateElement);
  //cvReleaseStructuringElement(&erodeElement);
  image.ReleaseImage();
  fgMask.ReleaseImage();
  tempMask.ReleaseImage();
  texture.ReleaseImage();
  std::cout << "~DPTexture()" << std::endl;
}

void DPTexture::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  frame = new IplImage(img_input);

  if (firstTime)
  {
    width = img_input.size().width;
    height = img_input.size().height;
    size = width * height;

    // input image
    image = cvCreateImage(cvSize(width, height), 8, 3);
    cvCopy(frame, image.Ptr());

    // foreground masks
    fgMask = cvCreateImage(cvSize(width, height), 8, 1);
    tempMask = cvCreateImage(cvSize(width, height), 8, 1);
    cvZero(fgMask.Ptr());
    cvZero(tempMask.Ptr());

    // create background model
    bgModel = new TextureArray[size];
    texture = cvCreateImage(cvSize(width, height), 8, 3);
    cvZero(texture.Ptr());
    modeArray = new unsigned char[size];
    curTextureHist = new TextureHistogram[size];

    // initialize background model
    bgs.LBP(image, texture);
    bgs.Histogram(texture, curTextureHist);
    for (int y = REGION_R + TEXTURE_R; y < height - REGION_R - TEXTURE_R; ++y)
    {
      for (int x = REGION_R + TEXTURE_R; x < width - REGION_R - TEXTURE_R; ++x)
      {
        int index = x + y*width;

        for (int m = 0; m < NUM_MODES; ++m)
        {
          for (int i = 0; i < NUM_BINS; ++i)
          {
            bgModel[index].mode[m].r[i] = curTextureHist[index].r[i];
            bgModel[index].mode[m].g[i] = curTextureHist[index].g[i];
            bgModel[index].mode[m].b[i] = curTextureHist[index].b[i];
          }
        }
      }
    }

    //dilateElement = cvCreateStructuringElementEx(7, 7, 3, 3,	CV_SHAPE_RECT);
    //erodeElement = cvCreateStructuringElementEx(3, 3, 1, 1,	CV_SHAPE_RECT);
    firstTime = false;
  }

  cvCopy(frame, image.Ptr());

  // perform background subtraction
  bgs.LBP(image, texture);
  bgs.Histogram(texture, curTextureHist);
  bgs.BgsCompare(bgModel, curTextureHist, modeArray, THRESHOLD, fgMask);

  //if(enableFiltering)
  //{
  //  // size filtering
  //  ConnectedComponents cc;
  //  CBlobResult largeBlobs;
  //  cc.SetImage(&fgMask);
  //  cc.Find(127);
  //  cc.FilterMinArea(size/30, largeBlobs);
  //  fgMask.Clear();
  //  cc.ColorBlobs(fgMask.Ptr(), largeBlobs, CV_RGB(255,255,255));
  //  largeBlobs.ClearBlobs();

  //  // morphological operators
  //  cvDilate(fgMask.Ptr(), fgMask.Ptr(), dilateElement, 1);
  //  cvErode(fgMask.Ptr(), fgMask.Ptr(), erodeElement, 1);
  //}

  img_foreground = cv::cvarrToMat(fgMask.Ptr());
  //img_background = cv::cvarrToMat(bgs.Background()->Ptr());
  img_background = cv::Mat::zeros(img_input.size(), img_input.type());

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow("Texture BGS (Donovan Parks)", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  // update background subtraction
  bgs.UpdateModel(fgMask, bgModel, curTextureHist, modeArray);

  delete frame;
}

void DPTexture::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  //cvWriteReal(fs, "alpha", alpha);
  //cvWriteInt(fs, "enableFiltering", enableFiltering);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void DPTexture::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  //alpha = cvReadRealByName(fs, nullptr, "alpha", 1e-6f);
  //enableFiltering = cvReadIntByName(fs, nullptr, "enableFiltering", true);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
