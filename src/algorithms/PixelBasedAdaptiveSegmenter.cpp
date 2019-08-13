#include "PixelBasedAdaptiveSegmenter.h"

using namespace bgslibrary::algorithms;

PixelBasedAdaptiveSegmenter::PixelBasedAdaptiveSegmenter() :
  IBGS(quote(PixelBasedAdaptiveSegmenter)),
  enableInputBlur(true), enableOutputBlur(true),
  alpha(7.0), beta(1.0), N(20), Raute_min(2), R_incdec(0.05), R_lower(18),
  R_scale(5), T_dec(0.05), T_inc(1), T_init(18), T_lower(2), T_upper(200)
{
  debug_construction(PixelBasedAdaptiveSegmenter);
  initLoadSaveConfig(algorithmName);
}

PixelBasedAdaptiveSegmenter::~PixelBasedAdaptiveSegmenter() {
  debug_destruction(PixelBasedAdaptiveSegmenter);
}

void PixelBasedAdaptiveSegmenter::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
    pbas.setAlpha(alpha);
    pbas.setBeta(beta);
    pbas.setN(N);
    pbas.setRaute_min(Raute_min);
    pbas.setR_incdec(R_incdec);
    pbas.setR_lower(R_lower);
    pbas.setR_scale(R_scale);
    pbas.setT_dec(T_dec);
    pbas.setT_inc(T_inc);
    pbas.setT_init(T_init);
    pbas.setT_lower(T_lower);
    pbas.setT_upper(T_upper);
  }

  cv::Mat img_input_new;
  if (enableInputBlur)
    cv::GaussianBlur(img_input, img_input_new, cv::Size(5, 5), 1.5);
  else
    img_input.copyTo(img_input_new);

  pbas.process(&img_input_new, &img_foreground);
  img_background = cv::Mat::zeros(img_input.size(), img_input.type());

  if (enableOutputBlur)
    cv::medianBlur(img_foreground, img_foreground, 5);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void PixelBasedAdaptiveSegmenter::save_config(cv::FileStorage &fs) {
  fs << "enableInputBlur" << enableInputBlur;
  fs << "enableOutputBlur" << enableOutputBlur;
  fs << "alpha" << alpha;
  fs << "beta" << beta;
  fs << "N" << N;
  fs << "Raute_min" << Raute_min;
  fs << "R_incdec" << R_incdec;
  fs << "R_lower" << R_lower;
  fs << "R_scale" << R_scale;
  fs << "T_dec" << T_dec;
  fs << "T_inc" << T_inc;
  fs << "T_init" << T_init;
  fs << "T_lower" << T_lower;
  fs << "T_upper" << T_upper;
  fs << "showOutput" << showOutput;
}

void PixelBasedAdaptiveSegmenter::load_config(cv::FileStorage &fs) {
  fs["enableInputBlur"] >> enableInputBlur;
  fs["enableOutputBlur"] >> enableOutputBlur;
  fs["alpha"] >> alpha;
  fs["beta"] >> beta;
  fs["N"] >> N;
  fs["Raute_min"] >> Raute_min;
  fs["R_incdec"] >> R_incdec;
  fs["R_lower"] >> R_lower;
  fs["R_scale"] >> R_scale;
  fs["T_dec"] >> T_dec;
  fs["T_inc"] >> T_inc;
  fs["T_init"] >> T_init;
  fs["T_lower"] >> T_lower;
  fs["T_upper"] >> T_upper;
  fs["showOutput"] >> showOutput;
}
