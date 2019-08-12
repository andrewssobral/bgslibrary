#include "SigmaDelta.h"

using namespace bgslibrary::algorithms;

SigmaDelta::SigmaDelta() :
  IBGS(quote(SigmaDelta)),
  ampFactor(1), minVar(15), maxVar(255), 
  algorithm(sdLaMa091New())
{
  debug_construction(SigmaDelta);
  applyParams();
  setup("./config/SigmaDelta.xml");
}

SigmaDelta::~SigmaDelta() {
  debug_destruction(SigmaDelta);
  sdLaMa091Free(algorithm);
}

void SigmaDelta::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
    sdLaMa091AllocInit_8u_C3R(algorithm, img_input.data, img_input.cols, img_input.rows, img_input.step);
    img_foreground = cv::Mat(img_input.size(), CV_8UC1);
    img_background = cv::Mat(img_input.size(), CV_8UC3);
    firstTime = false;
  }
  else {
    cv::Mat img_output_tmp(img_input.rows, img_input.cols, CV_8UC3);
    sdLaMa091Update_8u_C3R(algorithm, img_input.data, img_output_tmp.data);

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

void SigmaDelta::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
  
  fs << "ampFactor" << ampFactor;
  fs << "minVar" << minVar;
  fs << "maxVar" << maxVar;
  fs << "showOutput" << showOutput;
  
  fs.release();
}

void SigmaDelta::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);
  
  fs["ampFactor"] >> ampFactor;
  fs["minVar"] >> minVar;
  fs["maxVar"] >> maxVar;
  fs["showOutput"] >> showOutput;
  
  fs.release();
  
  applyParams();
}

void SigmaDelta::applyParams()
{
  sdLaMa091SetAmplificationFactor(algorithm, ampFactor);
  sdLaMa091SetMinimalVariance(algorithm, minVar);
  sdLaMa091SetMaximalVariance(algorithm, maxVar);
}
