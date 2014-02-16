#include "MyBGS.h"

MyBGS::MyBGS(){}
MyBGS::~MyBGS(){}

void MyBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  if(img_previous.empty())
    img_input.copyTo(img_previous);

  cv::Mat img_foreground;
  cv::absdiff(img_previous, img_input, img_foreground);

  if(img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  cv::threshold(img_foreground, img_foreground, 15, 255, cv::THRESH_BINARY);

  img_foreground.copyTo(img_output);
  img_previous.copyTo(img_bgmodel);

  img_input.copyTo(img_previous);
}
