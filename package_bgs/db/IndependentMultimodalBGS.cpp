#include "IndependentMultimodalBGS.h"

IndependentMultimodalBGS::IndependentMultimodalBGS() : fps(10), firstTime(true), showOutput(true){
  pIMBS = new BackgroundSubtractorIMBS(fps);
}
IndependentMultimodalBGS::~IndependentMultimodalBGS(){
  delete pIMBS;
}

void IndependentMultimodalBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();

  if (firstTime)
    saveConfig();

  //get the fgmask and update the background model
  pIMBS->apply(img_input, img_foreground);
  
  //get background image
  pIMBS->getBackgroundImage(img_background);

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  if (showOutput)
  {
    cv::imshow("IMBS FG", img_foreground);
    cv::imshow("IMBS BG", img_background);
  }

  firstTime = false;
}

void IndependentMultimodalBGS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/IndependentMultimodalBGS.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void IndependentMultimodalBGS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/IndependentMultimodalBGS.xml", 0, CV_STORAGE_READ);

  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
