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
#include "DPTextureBGS.h"

DPTextureBGS::DPTextureBGS() : firstTime(true), showOutput(true)
  //, enableFiltering(true)
{
  std::cout << "DPTextureBGS()" << std::endl;
}

DPTextureBGS::~DPTextureBGS()
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
  std::cout << "~DPTextureBGS()" << std::endl;
}

void DPTextureBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();

  frame = new IplImage(img_input);
  
  if(firstTime)
  {
    width	= img_input.size().width;
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
    for(int y = REGION_R+TEXTURE_R; y < height-REGION_R-TEXTURE_R; ++y)
    {
      for(int x = REGION_R+TEXTURE_R; x < width-REGION_R-TEXTURE_R; ++x)
      {
        int index = x+y*width;
      
        for(int m = 0; m < NUM_MODES; ++m)
        {
          for(int i = 0; i < NUM_BINS; ++i)
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

    saveConfig();
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

  cv::Mat foreground(fgMask.Ptr());
  if(!foreground.empty())
    foreground.copyTo(img_output);
  
  if(showOutput)
    cv::imshow("Texture BGS (Donovan Parks)", foreground);

  // update background subtraction		
  bgs.UpdateModel(fgMask, bgModel, curTextureHist, modeArray);
  
  delete frame;
}

void DPTextureBGS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/DPTextureBGS.xml", 0, CV_STORAGE_WRITE);

  //cvWriteReal(fs, "alpha", alpha);
  //cvWriteInt(fs, "enableFiltering", enableFiltering);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void DPTextureBGS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/DPTextureBGS.xml", 0, CV_STORAGE_READ);
  
  //alpha = cvReadRealByName(fs, 0, "alpha", 1e-6f);
  //enableFiltering = cvReadIntByName(fs, 0, "enableFiltering", true);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
