#include "DPTexture.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms;

DPTexture::DPTexture() :
  IBGS(quote(DPTexture))
  //, enableFiltering(true)
{
  debug_construction(DPTexture);
  initLoadSaveConfig(algorithmName);
}

DPTexture::~DPTexture() {
  debug_destruction(DPTexture);
  delete[] bgModel; // ~10Kb (25.708-15.968)
  delete[] modeArray;
  delete[] curTextureHist; // ~10Kb (16-6.396)
  //cvReleaseStructuringElement(&dilateElement);
  //cvReleaseStructuringElement(&erodeElement);
  image.ReleaseImage();
  fgMask.ReleaseImage();
  tempMask.ReleaseImage();
  texture.ReleaseImage();
}

void DPTexture::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  IplImage _frame = cvIplImage(img_input);
  frame = cvCloneImage(&_frame);

  width = img_input.size().width;
  height = img_input.size().height;
  size = width * height;

  // input image
  image = cvCreateImage(cvSize(width, height), 8, 3);
  cvCopy(frame, image.Ptr());

  if (firstTime) {
    // foreground masks
    fgMask = cvCreateImage(cvSize(width, height), 8, 1);
    tempMask = cvCreateImage(cvSize(width, height), 8, 1);
    cvZero(fgMask.Ptr());
    cvZero(tempMask.Ptr());

    // create background model
    bgModel = new dp::TextureArray[size];
    texture = cvCreateImage(cvSize(width, height), 8, 3);
    cvZero(texture.Ptr());
    modeArray = new unsigned char[size];
    curTextureHist = new dp::TextureHistogram[size];

    // initialize background model
    bgs.LBP(image, texture);
    bgs.Histogram(texture, curTextureHist);
    for (int y = dp::REGION_R + dp::TEXTURE_R; y < height - dp::REGION_R - dp::TEXTURE_R; ++y) {
      for (int x = dp::REGION_R + dp::TEXTURE_R; x < width - dp::REGION_R - dp::TEXTURE_R; ++x) {
        int index = x + y*width;
        for (int m = 0; m < dp::NUM_MODES; ++m) {
          for (int i = 0; i < dp::NUM_BINS; ++i) {
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
  else {
    // cvCopy(frame, image.Ptr());
    // perform background subtraction
    bgs.LBP(image, texture);
    bgs.Histogram(texture, curTextureHist);
    bgs.BgsCompare(bgModel, curTextureHist, modeArray, dp::THRESHOLD, fgMask);
  }

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
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  // update background subtraction
  bgs.UpdateModel(fgMask, bgModel, curTextureHist, modeArray);

  // free memory
  image.ReleaseImage();
  cvReleaseImage(&frame);
}

void DPTexture::save_config(cv::FileStorage &fs) {
  //fs << "alpha" << alpha;
  //fs << "enableFiltering" << enableFiltering;
  fs << "showOutput" << showOutput;
}

void DPTexture::load_config(cv::FileStorage &fs) {
  //fs["alpha"] >> alpha;
  //fs["enableFiltering"] >> enableFiltering;
  fs["showOutput"] >> showOutput;
}

#endif
