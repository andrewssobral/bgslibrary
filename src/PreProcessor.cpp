#include "PreProcessor.h"

namespace bgslibrary
{
  PreProcessor::PreProcessor() : 
    firstTime(true), equalizeHist(false), gaussianBlur(false)
  {
    debug_construction(PreProcessor);
    initLoadSaveConfig(quote(PreProcessor));
  }

  PreProcessor::~PreProcessor() {
    debug_destruction(PreProcessor);
  }

  void PreProcessor::setEqualizeHist(bool value) {
    equalizeHist = value;
  }

  void PreProcessor::setGaussianBlur(bool value) {
    gaussianBlur = value;
  }

  cv::Mat PreProcessor::getGrayScale() {
    return img_gray.clone();
  }

  void PreProcessor::process(const cv::Mat &img_input, cv::Mat &img_output)
  {
    if (img_input.empty())
      return;

    img_input.copyTo(img_output);

    // Converts image from one color space to another
    // http://opencv.willowgarage.com/documentation/cpp/miscellaneous_image_transformations.html#cv-cvtcolor
    cv::cvtColor(img_input, img_gray, CV_BGR2GRAY);
    //img_gray.copyTo(img_output);

    // Equalizes the histogram of a grayscale image
    // http://opencv.willowgarage.com/documentation/cpp/histograms.html#cv-equalizehist
    if (equalizeHist)
      cv::equalizeHist(img_output, img_output);

    // Smoothes image using a Gaussian filter
    // http://opencv.willowgarage.com/documentation/cpp/imgproc_image_filtering.html#GaussianBlur
    if (gaussianBlur)
      cv::GaussianBlur(img_output, img_output, cv::Size(7, 7), 1.5);

    if (enableShow)
      cv::imshow("PreProcessor", img_output);

    firstTime = false;
  }
  /*
  void PreProcessor::rotate(const cv::Mat &img_input, cv::Mat &img_output, float angle)
  {
    IplImage* image = new IplImage(img_input);

    //IplImage *rotatedImage = cvCreateImage(cvSize(480,320), IPL_DEPTH_8U, image->nChannels);
    //IplImage *rotatedImage = cvCreateImage(cvSize(image->width,image->height), IPL_DEPTH_8U, image->nChannels);
    IplImage* rotatedImage = cvCreateImage(cvSize(image->height, image->width), IPL_DEPTH_8U, image->nChannels);

    CvPoint2D32f center;
    //center.x = 160;
    //center.y = 160;
    center.x = (image->height / 2);
    center.y = (image->width / 2);

    CvMat* mapMatrix = cvCreateMat(2, 3, CV_32FC1);

    cv2DRotationMatrix(center, angle, 1.0, mapMatrix);
    cvWarpAffine(image, rotatedImage, mapMatrix, CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS, cvScalarAll(0));

    cv::Mat img_rot = cv::cvarrToMat(rotatedImage);
    img_rot.copyTo(img_output);

    cvReleaseImage(&image);
    cvReleaseImage(&rotatedImage);
    cvReleaseMat(&mapMatrix);
  }
  */
  void PreProcessor::applyCanny(const cv::Mat &img_input, cv::Mat &img_output)
  {
    if (img_input.empty())
      return;

    //------------------------------------------------------------------
    // Canny
    // Finds edges in an image using Canny algorithm.
    // http://opencv.willowgarage.com/documentation/cpp/imgproc_feature_detection.html#cv-canny
    //------------------------------------------------------------------

    cv::Mat img_canny;
    cv::Canny(
      img_input, // image � Single-channel 8-bit input image
      img_canny,  // edges � The output edge map. It will have the same size and the same type as image
      100,       // threshold1 � The first threshold for the hysteresis procedure
      200);      // threshold2 � The second threshold for the hysteresis procedure
    cv::threshold(img_canny, img_canny, 128, 255, cv::THRESH_BINARY_INV);

    img_canny.copyTo(img_output);
  }

  void PreProcessor::save_config(cv::FileStorage &fs) {
    fs << "equalizeHist" << equalizeHist;
    fs << "gaussianBlur" << gaussianBlur;
    fs << "enableShow" << enableShow;
  }

  void PreProcessor::load_config(cv::FileStorage &fs) {
    fs["equalizeHist"] >> equalizeHist;
    fs["gaussianBlur"] >> gaussianBlur;
    fs["enableShow"] >> enableShow;
  }
}
