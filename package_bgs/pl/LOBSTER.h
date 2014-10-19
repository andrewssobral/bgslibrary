#pragma once

#include <opencv2/opencv.hpp>


#include "../IBGS.h"

class BackgroundSubtractorLOBSTER;

class LOBSTERBGS : public IBGS
{
private:
	BackgroundSubtractorLOBSTER* pLOBSTER;
	bool firstTime;
	bool showOutput;

	float fRelLBSPThreshold;
	size_t nLBSPThresholdOffset;
	size_t nDescDistThreshold;
	size_t nColorDistThreshold;
	size_t nBGSamples;
	size_t nRequiredBGSamples;
  
public:
  LOBSTERBGS();
  ~LOBSTERBGS();

  void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

private:
  void saveConfig();
  void loadConfig();
};
