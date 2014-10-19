#include "SuBSENSE.h"
#include "BackgroundSubtractorSuBSENSE.h"


SuBSENSEBGS::SuBSENSEBGS() :
pSubsense(0), firstTime(true), showOutput(true),
fRelLBSPThreshold 			(BGSSUBSENSE_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
nDescDistThresholdOffset 	(BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET),
nMinColorDistThreshold 		(BGSSUBSENSE_DEFAULT_MIN_COLOR_DIST_THRESHOLD),
nBGSamples 					(BGSSUBSENSE_DEFAULT_NB_BG_SAMPLES),
nRequiredBGSamples 			(BGSSUBSENSE_DEFAULT_REQUIRED_NB_BG_SAMPLES),
nSamplesForMovingAvgs 		(BGSSUBSENSE_DEFAULT_N_SAMPLES_FOR_MV_AVGS)
{
}

SuBSENSEBGS::~SuBSENSEBGS() {
	if (pSubsense)
		delete pSubsense;
}

void SuBSENSEBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();

  if (firstTime) {
    saveConfig();
    pSubsense = new BackgroundSubtractorSuBSENSE(
    		fRelLBSPThreshold, nDescDistThresholdOffset, nMinColorDistThreshold,
    		nBGSamples, nRequiredBGSamples, nSamplesForMovingAvgs);

    pSubsense->initialize(img_input, cv::Mat (img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
    firstTime = false;
  }

  (*pSubsense)(img_input, img_output);
  pSubsense->getBackgroundImage(img_bgmodel);

  if(showOutput) {
	  imshow("SuBSENSE FG", img_output);
	  imshow("SuBSENSE BG", img_bgmodel);
  }
}

void SuBSENSEBGS::saveConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("./config/SuBSENSEBGS.xml", 0, CV_STORAGE_WRITE);

	cvWriteReal(fs, "fRelLBSPThreshold", fRelLBSPThreshold);
	cvWriteInt(fs, "nDescDistThresholdOffset", nDescDistThresholdOffset);
	cvWriteInt(fs, "nMinColorDistThreshold", nMinColorDistThreshold);
	cvWriteInt(fs, "nBGSamples", nBGSamples);
	cvWriteInt(fs, "nRequiredBGSamples", nRequiredBGSamples);
	cvWriteInt(fs, "nSamplesForMovingAvgs", nSamplesForMovingAvgs);
  cvWriteInt(fs, "showOutput", showOutput);

	cvReleaseFileStorage(&fs);
}

void SuBSENSEBGS::loadConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("./config/SuBSENSEBGS.xml", 0, CV_STORAGE_READ);

	fRelLBSPThreshold = cvReadRealByName(fs, 0, "fRelLBSPThreshold", BGSSUBSENSE_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD);
	nDescDistThresholdOffset = cvReadIntByName(fs, 0, "nDescDistThresholdOffset", BGSSUBSENSE_DEFAULT_DESC_DIST_THRESHOLD_OFFSET);
	nMinColorDistThreshold = cvReadIntByName(fs, 0, "nMinColorDistThreshold", BGSSUBSENSE_DEFAULT_MIN_COLOR_DIST_THRESHOLD);
	nBGSamples = cvReadIntByName(fs, 0, "nBGSamples", BGSSUBSENSE_DEFAULT_NB_BG_SAMPLES);
	nRequiredBGSamples = cvReadIntByName(fs, 0, "nRequiredBGSamples", BGSSUBSENSE_DEFAULT_REQUIRED_NB_BG_SAMPLES);
	nSamplesForMovingAvgs = cvReadIntByName(fs, 0, "nSamplesForMovingAvgs", BGSSUBSENSE_DEFAULT_N_SAMPLES_FOR_MV_AVGS);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

	cvReleaseFileStorage(&fs);
}
