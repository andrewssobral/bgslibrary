#include <iostream>
#include <opencv2/opencv.hpp>

#include "../package_bgs/FrameDifferenceBGS.h"

int main(int argc, char **argv)
{
  CvCapture *capture = 0;
  capture = cvCaptureFromCAM(0);

  if(!capture){
    std::cerr << "Cannot initialize video!" << std::endl;
    return -1;
  }

  IBGS *bgs;
  bgs = new FrameDifferenceBGS;

  IplImage *frame;
  while(1)
  {
    frame = cvQueryFrame(capture);
    if(!frame) break;

    // cv::Mat img_input(frame); // don't works with opencv3
		cv::Mat img_input = cv::cvarrToMat(frame);
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