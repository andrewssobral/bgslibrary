#include "MyBGS.h"

using namespace bgslibrary::algorithms;

MyBGS::MyBGS() :
  IBGS(quote(MyBGS))
{
  debug_construction(MyBGS);
}
MyBGS::~MyBGS() {
  debug_destruction(MyBGS);
}

void MyBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if (img_input.empty())
    return;

  if (img_previous.empty())
    img_input.copyTo(img_previous);

  cv::Mat img_foreground;
  cv::absdiff(img_previous, img_input, img_foreground);

  if (img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  cv::threshold(img_foreground, img_foreground, 15, 255, cv::THRESH_BINARY);
  
#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_previous.copyTo(img_bgmodel);

  img_input.copyTo(img_previous);
}

void MyBGS::save_config(cv::FileStorage &fs) {
  fs << "showOutput" << showOutput;
}

void MyBGS::load_config(cv::FileStorage &fs) {
  fs["showOutput"] >> showOutput;
}
