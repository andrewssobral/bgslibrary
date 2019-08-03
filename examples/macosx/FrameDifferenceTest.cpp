#include <iostream>
#include <opencv2/opencv.hpp>

#include "../../package_bgs/FrameDifference.h"

using namespace bgslibrary::algorithms;
//using namespace cv;

int main(int argc, char **argv)
{
  cv::VideoCapture capture(0);

  if (!capture.isOpened())
  {
    std::cerr << "Cannot initialize video!" << std::endl;
    return -1;
  }

  IBGS *bgs;
  bgs = new FrameDifference;

  int key = 0;
  cv::Mat img_input;
  while (key != 'q')
  {
    capture >> img_input;
    if(img_input.empty()) break;

    cv::imshow("Input", img_input);

    cv::Mat img_mask;
    cv::Mat img_bkgmodel;

    // by default, it shows automatically the foreground mask image
    bgs->process(img_input, img_mask, img_bkgmodel);

    key = cvWaitKey(33);

    if(key >= 0)
		  break;
  }

  delete bgs;

  capture.release();
  cvDestroyAllWindows();

  return 0;
}
