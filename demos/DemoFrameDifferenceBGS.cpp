#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "package_bgs/FrameDifferenceBGS.h"

int main(int argc, char **argv)
{
  CvCapture *capture = 0;
  
  capture = cvCaptureFromCAM(0);
  //capture = cvCaptureFromAVI("video.avi");
  
  if(!capture){
    std::cerr << "Cannot open initialize webcam!" << std::endl;
    return 1;
  }
  
  IplImage *frame = cvQueryFrame(capture);
  
  FrameDifferenceBGS* bgs = new FrameDifferenceBGS;

  int key = 0;
  while(key != 'q')
  {
    frame = cvQueryFrame(capture);

    if(!frame) break;

    cv::Mat img_input(frame,true);
    cv::resize(img_input,img_input,cv::Size(320,240));
    cv::imshow("input", img_input);
    
    cv::Mat img_mask;
    bgs->process(img_input, img_mask); // automatically shows the foreground mask image
    
    //if(!img_mask.empty())
    //  do something
    
    key = cvWaitKey(1);
  }

  delete bgs;

  cvDestroyAllWindows();
  cvReleaseCapture(&capture);
  
  return 0;
}
