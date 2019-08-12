#include "ForegroundMaskAnalysis.h"

namespace bgslibrary
{
  ForegroundMaskAnalysis::ForegroundMaskAnalysis() :
    firstTime(true), showOutput(true),
    stopAt(0), img_ref_path("")
  {
    debug_construction(ForegroundMaskAnalysis);
    setup("./config/ForegroundMaskAnalysis.xml");
  }

  ForegroundMaskAnalysis::~ForegroundMaskAnalysis() {
    debug_destruction(ForegroundMaskAnalysis);
  }

  void ForegroundMaskAnalysis::process(const long &frameNumber, const std::string &name, const cv::Mat &img_input)
  {
    if (img_input.empty())
      return;

    if (stopAt == 0) {
      loadConfig();

      if (firstTime)
        saveConfig();
    }

    if (stopAt == frameNumber && img_ref_path.empty() == false)
    {
      cv::Mat img_ref = cv::imread(img_ref_path, 0);

      if (showOutput)
        cv::imshow("ForegroundMaskAnalysis", img_ref);

      int rn = cv::countNonZero(img_ref);
      cv::Mat i;
      cv::Mat u;

      if (rn > 0)
      {
        i = img_input & img_ref;
        u = img_input | img_ref;
      }
      else
      {
        i = (~img_input) & (~img_ref);
        u = (~img_input) | (~img_ref);
      }

      int in = cv::countNonZero(i);
      int un = cv::countNonZero(u);

      double s = (((double)in) / ((double)un));

      if (showOutput) {
        cv::imshow("A^B", i);
        cv::imshow("AvB", u);
      }

      std::cout << name << " - Similarity Measure: " << s << " press ENTER to continue" << std::endl;

      cv::waitKey(0);
    }

    firstTime = false;
  }

  void ForegroundMaskAnalysis::saveConfig()
  {
    cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);
    
    fs << "stopAt" << stopAt;
    fs << "img_ref_path" << img_ref_path;
    
    fs.release();
  }

  void ForegroundMaskAnalysis::loadConfig()
  {
    cv::FileStorage fs;
    fs.open(config_xml, cv::FileStorage::READ);
    
    fs["stopAt"] >> stopAt;
    fs["img_ref_path"] >> img_ref_path;
    
    fs.release();
  }
}
