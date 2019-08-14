#include "SigmaDelta.h"

using namespace bgslibrary::algorithms;

SigmaDelta::SigmaDelta() :
  IBGS(quote(SigmaDelta)),
  ampFactor(1), minVar(15), maxVar(255), 
  algorithm(sigmadelta::sdLaMa091New())
{
  debug_construction(SigmaDelta);
  initLoadSaveConfig(algorithmName);
  applyParams();
}

SigmaDelta::~SigmaDelta() {
  debug_destruction(SigmaDelta);
  sigmadelta::sdLaMa091Free(algorithm);
}

void SigmaDelta::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
    sigmadelta::sdLaMa091AllocInit_8u_C3R(algorithm, img_input.data, img_input.cols, img_input.rows, img_input.step);
    img_foreground = cv::Mat(img_input.size(), CV_8UC1);
    img_background = cv::Mat(img_input.size(), CV_8UC3);
    firstTime = false;
  }
  else {
    cv::Mat img_output_tmp(img_input.rows, img_input.cols, CV_8UC3);
    sigmadelta::sdLaMa091Update_8u_C3R(algorithm, img_input.data, img_output_tmp.data);

    unsigned char* tmpBuffer = (unsigned char*)img_output_tmp.data;
    unsigned char* outBuffer = (unsigned char*)img_foreground.data;

    for (size_t i = 0; i < img_foreground.total(); ++i) {
      *outBuffer = *tmpBuffer;
      ++outBuffer;
      tmpBuffer += img_output_tmp.channels();
    }
  }

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void SigmaDelta::save_config(cv::FileStorage &fs) {
  fs << "ampFactor" << ampFactor;
  fs << "minVar" << minVar;
  fs << "maxVar" << maxVar;
  fs << "showOutput" << showOutput;
}

void SigmaDelta::load_config(cv::FileStorage &fs) {
  fs["ampFactor"] >> ampFactor;
  fs["minVar"] >> minVar;
  fs["maxVar"] >> maxVar;
  fs["showOutput"] >> showOutput;
}

void SigmaDelta::applyParams() {
  sigmadelta::sdLaMa091SetAmplificationFactor(algorithm, ampFactor);
  sigmadelta::sdLaMa091SetMinimalVariance(algorithm, minVar);
  sigmadelta::sdLaMa091SetMaximalVariance(algorithm, maxVar);
}
