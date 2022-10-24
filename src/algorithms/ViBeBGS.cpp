#include "VibeBGS.h"

using namespace bgslibrary::algorithms;

VibeBGS::VibeBGS() :
  IBGS(quote(VibeBGS)),
  numberOfSamples(DEFAULT_NUM_SAMPLES),
  matchingThreshold(DEFAULT_MATCH_THRESH),
  matchingNumber(DEFAULT_MATCH_NUM),
  updateFactor(DEFAULT_UPDATE_FACTOR),
  model(nullptr)
{
    debug_construction(VibeBGS);
    initLoadSaveConfig(algorithmName);
    model = std::make_unique<sky360::VibeBGS>(matchingThreshold,
                                            numberOfSamples,
                                            matchingNumber,
                                            updateFactor);
}

VibeBGS::~VibeBGS() {
    debug_destruction(VibeBGS);
}

/// Ignoring img_bgmodel for now
void VibeBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
    //init(img_input, img_output, img_bgmodel);
    if (img_input.empty())
        return;

    if (firstTime) {
        model->initialize(img_input);
        firstTime = false;
    }

    model->apply(img_input, img_output);

#ifndef MEX_COMPILE_FLAG
    if (showOutput)
        cv::imshow(algorithmName + "_FG", img_output);
#endif
}

void VibeBGS::save_config(cv::FileStorage &fs) {
    fs << "numberOfSamples" << numberOfSamples;
    fs << "matchingThreshold" << matchingThreshold;
    fs << "matchingNumber" << matchingNumber;
    fs << "updateFactor" << updateFactor;
    fs << "showOutput" << showOutput;
}

void VibeBGS::load_config(cv::FileStorage &fs) {
    fs["numberOfSamples"] >> numberOfSamples;
    fs["matchingThreshold"] >> matchingThreshold;
    fs["matchingNumber"] >> matchingNumber;
    fs["updateFactor"] >> updateFactor;
    fs["showOutput"] >> showOutput;
}
