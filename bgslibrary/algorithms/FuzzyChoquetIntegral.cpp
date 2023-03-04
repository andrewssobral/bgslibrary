#include "FuzzyChoquetIntegral.h"

using namespace bgslibrary::algorithms;

FuzzyChoquetIntegral::FuzzyChoquetIntegral() :
  IBGS(quote(FuzzyChoquetIntegral)),
  frameNumber(0), framesToLearn(10), alphaLearn(0.1),
  alphaUpdate(0.01), colorSpace(1), option(2),
  smooth(true), threshold(0.67)
{
  debug_construction(FuzzyChoquetIntegral);
  initLoadSaveConfig(algorithmName);
}

FuzzyChoquetIntegral::~FuzzyChoquetIntegral() {
  debug_destruction(FuzzyChoquetIntegral);
}

void FuzzyChoquetIntegral::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  cv::Mat img_input_f3(img_input.size(), CV_32F);
  img_input.convertTo(img_input_f3, CV_32F, 1. / 255.);

  if (firstTime) {
    std::cout << algorithmName + " parameters:" << std::endl;

    std::string colorSpaceName = "";
    switch (colorSpace)
    {
    case 1: colorSpaceName = "RGB";  break;
    case 2: colorSpaceName = "OHTA"; break;
    case 3: colorSpaceName = "HSV";  break;
    case 4: colorSpaceName = "YCrCb"; break;
    }
    std::cout << "Color space: " << colorSpaceName << std::endl;

    if (option == 1)
      std::cout << "Fuzzing by 3 color components" << std::endl;
    if (option == 2)
      std::cout << "Fuzzing by 2 color components + 1 texture component" << std::endl;
  }

  if (frameNumber <= framesToLearn) {
    if (frameNumber == 0)
      std::cout << algorithmName + " initializing background model by adaptive learning" << std::endl;

    if (img_background_f3.empty())
      img_input_f3.copyTo(img_background_f3);
    else
      img_background_f3 = alphaLearn*img_input_f3 + (1 - alphaLearn)*img_background_f3;

    double minVal = 0., maxVal = 1.;
    img_background_f3.convertTo(img_background, CV_8U, 255.0 / (maxVal - minVal), -minVal);
    img_background.copyTo(img_bgmodel);

    img_foreground = cv::Mat::zeros(img_input.size(), img_input.type());
    img_foreground.copyTo(img_output);

#ifndef MEX_COMPILE_FLAG
    if (showOutput)
      cv::imshow(algorithmName + "_BG", img_background);
#endif
  }
  else
  {
    cv::Mat img_input_f1;
    cv::cvtColor(img_input_f3, img_input_f1, CV_BGR2GRAY);

    cv::Mat img_background_f1;
    cv::cvtColor(img_background_f3, img_background_f1, CV_BGR2GRAY);

    IplImage _input_f3 = cvIplImage(img_input_f3);
    IplImage* input_f3 = cvCloneImage(&_input_f3);

    IplImage _input_f1 = cvIplImage(img_input_f1);
    IplImage* input_f1 = cvCloneImage(&_input_f1);

    IplImage _background_f3 = cvIplImage(img_background_f3);
    IplImage* background_f3 = cvCloneImage(&_background_f3);

    IplImage _background_f1 = cvIplImage(img_background_f1);
    IplImage* background_f1 = cvCloneImage(&_background_f1);

    IplImage* lbp_input_f1 = cvCreateImage(cvSize(input_f1->width, input_f1->height), IPL_DEPTH_32F, 1);
    cvSetZero(lbp_input_f1);
    fu.LBP(input_f1, lbp_input_f1);

    IplImage* lbp_background_f1 = cvCreateImage(cvSize(background_f1->width, background_f1->height), IPL_DEPTH_32F, 1);
    cvSetZero(lbp_background_f1);
    fu.LBP(background_f1, lbp_background_f1);

    IplImage* sim_texture_f1 = cvCreateImage(cvSize(input_f1->width, input_f1->height), IPL_DEPTH_32F, 1);
    fu.SimilarityDegreesImage(lbp_input_f1, lbp_background_f1, sim_texture_f1, 1, colorSpace);

    IplImage* sim_color_f3 = cvCreateImage(cvSize(input_f3->width, input_f3->height), IPL_DEPTH_32F, 3);
    fu.SimilarityDegreesImage(input_f3, background_f3, sim_color_f3, 3, colorSpace);

    float* measureG = (float*)malloc(3 * (sizeof(float)));
    IplImage* integral_choquet_f1 = cvCreateImage(cvSize(input_f1->width, input_f1->height), IPL_DEPTH_32F, 1);

    // 3 color components
    if (option == 1) {
      fu.FuzzyMeasureG(0.4f, 0.3f, 0.3f, measureG);
      fu.getFuzzyIntegralChoquet(sim_texture_f1, sim_color_f3, option, measureG, integral_choquet_f1);
    }

    // 2 color components + 1 texture component
    if (option == 2) {
      fu.FuzzyMeasureG(0.6f, 0.3f, 0.1f, measureG);
      fu.getFuzzyIntegralChoquet(sim_texture_f1, sim_color_f3, option, measureG, integral_choquet_f1);
    }

    free(measureG);
    cv::Mat img_integral_choquet_f1 = cv::cvarrToMat(integral_choquet_f1);

    if (smooth)
      cv::medianBlur(img_integral_choquet_f1, img_integral_choquet_f1, 3);

    cv::Mat img_foreground_f1(img_input.size(), CV_32F);
    cv::threshold(img_integral_choquet_f1, img_foreground_f1, threshold, 255, cv::THRESH_BINARY_INV);

    //cv::Mat img_foreground_u1(img_input.size(), CV_8U);
    double minVal = 0., maxVal = 1.;
    img_foreground_f1.convertTo(img_foreground, CV_8U, 255.0 / (maxVal - minVal), -minVal);
    img_foreground.copyTo(img_output);

    //cv::Mat img_background_u3(img_input.size(), CV_8U);
    //double minVal = 0., maxVal = 1.;
    img_background_f3.convertTo(img_background, CV_8U, 255.0 / (maxVal - minVal), -minVal);
    img_background.copyTo(img_bgmodel);

#ifndef MEX_COMPILE_FLAG
    if (showOutput) {
      cv::imshow(algorithmName + "_LBP_IN", cv::cvarrToMat(lbp_input_f1));
      cv::imshow(algorithmName + "_LBP_BG", cv::cvarrToMat(lbp_background_f1));
      cv::imshow(algorithmName + "_FG_PROB", cv::cvarrToMat(integral_choquet_f1));
      cv::imshow(algorithmName + "_BG", img_background);
      cv::imshow(algorithmName + "_FG", img_foreground);
    }
#endif

    if (frameNumber == (framesToLearn + 1))
      std::cout << algorithmName + " updating background model by adaptive-selective learning" << std::endl;

    IplImage* updated_background_f3 = cvCreateImage(cvSize(input_f1->width, input_f1->height), IPL_DEPTH_32F, 3);
    cvSetZero(updated_background_f3);
    fu.AdaptativeSelectiveBackgroundModelUpdate(input_f3, background_f3, updated_background_f3, integral_choquet_f1, threshold, alphaUpdate);
    cv::Mat img_updated_background_f3 = cv::cvarrToMat(updated_background_f3);
    img_updated_background_f3.copyTo(img_background_f3);

    cvReleaseImage(&input_f3);
    cvReleaseImage(&input_f1);
    cvReleaseImage(&background_f3);
    cvReleaseImage(&background_f1);
    cvReleaseImage(&lbp_input_f1);
    cvReleaseImage(&lbp_background_f1);
    cvReleaseImage(&sim_texture_f1);
    cvReleaseImage(&sim_color_f3);
    cvReleaseImage(&integral_choquet_f1);
    cvReleaseImage(&updated_background_f3);
  }

  firstTime = false;
  frameNumber++;
}

void FuzzyChoquetIntegral::save_config(cv::FileStorage &fs) {
  fs << "threshold" << threshold;
  fs << "framesToLearn" << framesToLearn;
  fs << "alphaLearn" << alphaLearn;
  fs << "alphaUpdate" << alphaUpdate;
  fs << "colorSpace" << colorSpace;
  fs << "option" << option;
  fs << "smooth" << smooth;
  fs << "showOutput" << showOutput;
}

void FuzzyChoquetIntegral::load_config(cv::FileStorage &fs) {
  fs["threshold"] >> threshold;
  fs["framesToLearn"] >> framesToLearn;
  fs["alphaLearn"] >> alphaLearn;
  fs["alphaUpdate"] >> alphaUpdate;
  fs["colorSpace"] >> colorSpace;
  fs["option"] >> option;
  fs["smooth"] >> smooth;
  fs["showOutput"] >> showOutput;
}
