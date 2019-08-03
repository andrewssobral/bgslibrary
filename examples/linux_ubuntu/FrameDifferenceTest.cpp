#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "../../package_bgs/FrameDifference.h"

using namespace bgslibrary::algorithms;

int main(int argc, char **argv)
{
  CvCapture *capture = 0;
  capture = cvCaptureFromCAM(0);

  if(!capture){
    std::cerr << "Cannot initialize video!" << std::endl;
    return -1;
  }

  IBGS *bgs;
  bgs = new FrameDifference;

  IplImage *frame;
  while(1)
  {
    frame = cvQueryFrame(capture);
    if(!frame) break;

    cv::Mat img_input(frame);
    cv::imshow("Input", img_input);

    cv::Mat img_mask;
    cv::Mat img_bkgmodel;

    // by default, it shows automatically the foreground mask image
    bgs->process(img_input, img_mask, img_bkgmodel);

    //if(!img_mask.empty())
    //  cv::imshow("Foreground", img_mask);
    //  do something

    if(cvWaitKey(33) >= 0)
		  break;
  }

  delete bgs;

  cvDestroyAllWindows();
  cvReleaseCapture(&capture);

  return 0;
}
