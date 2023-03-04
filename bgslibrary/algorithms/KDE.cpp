#include "KDE.h"

using namespace bgslibrary::algorithms;

KDE::KDE() :
  IBGS(quote(KDE)),
  SequenceLength(50), TimeWindowSize(100), 
  SDEstimationFlag(1), lUseColorRatiosFlag(1),
  th(10e-8), alpha(0.3), framesToLearn(10), frameNumber(0)
{
  debug_construction(KDE);
  initLoadSaveConfig(algorithmName);
  p = new kde::NPBGSubtractor;
}

KDE::~KDE() {
  debug_destruction(KDE);
  delete FGImage;
  delete p;
}

void KDE::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (firstTime) {
    rows = img_input.size().height;
    cols = img_input.size().width;
    color_channels = img_input.channels();

    // SequenceLength: number of samples for each pixel.
    // TimeWindowSize: Time window for sampling. for example in the call above, the bg will sample 50 points out of 100 frames.
    // this rate will affect how fast the model adapt.
    // SDEstimationFlag: True means to estimate suitable kernel bandwidth to each pixel, False uses a default value.
    // lUseColorRatiosFlag: True means use normalized RGB for color (recommended.)
    p->Intialize(rows, cols, color_channels, SequenceLength, TimeWindowSize, SDEstimationFlag, lUseColorRatiosFlag);
    // th: 0-1 is the probability threshold for a pixel to be a foregroud. typically make it small as 10e-8. the smaller the value the less false positive and more false negative.
    // alpha: 0-1, for color. typically set to 0.3. this affect shadow suppression.
    p->SetThresholds(th, alpha);

    FGImage = new unsigned char[rows*cols];
    //FilteredFGImage = new unsigned char[rows*cols];
    FilteredFGImage = 0;
    DisplayBuffers = 0;

    img_foreground = cv::Mat::zeros(img_input.size(), CV_8UC1);
    img_background = cv::Mat::zeros(img_input.size(), img_input.type());

    frameNumber = 0;
    firstTime = false;
  }

  // Stores the first N frames to build the background model
  if (frameNumber < framesToLearn) {
    p->AddFrame(img_input.data);
    frameNumber++;
  }
  else {
    // Build the background model with first 10 frames
    if (frameNumber == framesToLearn) {
      p->Estimation();
      frameNumber++;
    }

    // Now, we can subtract the background
    ((kde::NPBGSubtractor*)p)->NBBGSubtraction(img_input.data, FGImage, FilteredFGImage, DisplayBuffers);

    // At each frame also you can call the update function to adapt the bg
    // here you pass a mask where pixels with true value will be masked out of the update.
    ((kde::NPBGSubtractor*)p)->Update(FGImage);

    img_foreground.data = FGImage;
  }

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    cv::imshow(algorithmName + "_FG", img_foreground);
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
}

void KDE::save_config(cv::FileStorage &fs) {
  fs << "framesToLearn" << framesToLearn;
  fs << "SequenceLength" << SequenceLength;
  fs << "TimeWindowSize" << TimeWindowSize;
  fs << "SDEstimationFlag" << SDEstimationFlag;
  fs << "lUseColorRatiosFlag" << lUseColorRatiosFlag;
  fs << "th" << th;
  fs << "alpha" << alpha;
  fs << "showOutput" << showOutput;
}

void KDE::load_config(cv::FileStorage &fs) {
  fs["framesToLearn"] >> framesToLearn;
  fs["SequenceLength"] >> SequenceLength;
  fs["TimeWindowSize"] >> TimeWindowSize;
  fs["SDEstimationFlag"] >> SDEstimationFlag;
  fs["lUseColorRatiosFlag"] >> lUseColorRatiosFlag;
  fs["th"] >> th;
  fs["alpha"] >> alpha;
  fs["showOutput"] >> showOutput;
}
