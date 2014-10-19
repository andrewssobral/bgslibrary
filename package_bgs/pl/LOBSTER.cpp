#include "LOBSTER.h"
#include "BackgroundSubtractorLOBSTER.h"


LOBSTERBGS::LOBSTERBGS() :
pLOBSTER(0), firstTime(true), showOutput(true),
fRelLBSPThreshold 			(BGSLOBSTER_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD),
nLBSPThresholdOffset 		(BGSLOBSTER_DEFAULT_LBSP_OFFSET_SIMILARITY_THRESHOLD),
nDescDistThreshold 			(BGSLOBSTER_DEFAULT_DESC_DIST_THRESHOLD),
nColorDistThreshold 		(BGSLOBSTER_DEFAULT_COLOR_DIST_THRESHOLD),
nBGSamples 					(BGSLOBSTER_DEFAULT_NB_BG_SAMPLES),
nRequiredBGSamples 			(BGSLOBSTER_DEFAULT_REQUIRED_NB_BG_SAMPLES)
{
}

LOBSTERBGS::~LOBSTERBGS() {
	if (pLOBSTER)
		delete pLOBSTER;
}

void LOBSTERBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if(img_input.empty())
    return;

  loadConfig();

  if (firstTime) {
    saveConfig();
    pLOBSTER = new BackgroundSubtractorLOBSTER(
    		fRelLBSPThreshold, nLBSPThresholdOffset, nDescDistThreshold,
    		nColorDistThreshold, nBGSamples, nRequiredBGSamples);

    pLOBSTER->initialize(img_input, cv::Mat (img_input.size(), CV_8UC1, cv::Scalar_<uchar>(255)));
    firstTime = false;
  }

  (*pLOBSTER)(img_input, img_output);
  pLOBSTER->getBackgroundImage(img_bgmodel);

  if(showOutput) {
	  imshow("LOBSTER FG", img_output);
	  imshow("LOBSTER BG", img_bgmodel);
  }
}

void LOBSTERBGS::saveConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("./config/LOBSTERBGS.xml", 0, CV_STORAGE_WRITE);

	cvWriteReal(fs, "fRelLBSPThreshold", fRelLBSPThreshold);
	cvWriteInt(fs, "nLBSPThresholdOffset", nLBSPThresholdOffset);
	cvWriteInt(fs, "nDescDistThreshold", nDescDistThreshold);
	cvWriteInt(fs, "nColorDistThreshold", nColorDistThreshold);
	cvWriteInt(fs, "nBGSamples", nBGSamples);
  cvWriteInt(fs, "nRequiredBGSamples", nRequiredBGSamples);
  cvWriteInt(fs, "showOutput", showOutput);

	cvReleaseFileStorage(&fs);
}

void LOBSTERBGS::loadConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("./config/LOBSTERBGS.xml", 0, CV_STORAGE_READ);

	fRelLBSPThreshold = cvReadRealByName(fs, 0, "fRelLBSPThreshold", BGSLOBSTER_DEFAULT_LBSP_REL_SIMILARITY_THRESHOLD);
	nLBSPThresholdOffset = cvReadIntByName(fs, 0, "nLBSPThresholdOffset", BGSLOBSTER_DEFAULT_LBSP_OFFSET_SIMILARITY_THRESHOLD);
	nDescDistThreshold = cvReadIntByName(fs, 0, "nDescDistThreshold", BGSLOBSTER_DEFAULT_DESC_DIST_THRESHOLD);
	nColorDistThreshold = cvReadIntByName(fs, 0, "nColorDistThreshold", BGSLOBSTER_DEFAULT_COLOR_DIST_THRESHOLD);
	nBGSamples = cvReadIntByName(fs, 0, "nBGSamples", BGSLOBSTER_DEFAULT_NB_BG_SAMPLES);
	nRequiredBGSamples = cvReadIntByName(fs, 0, "nRequiredBGSamples", BGSLOBSTER_DEFAULT_REQUIRED_NB_BG_SAMPLES);
  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

	cvReleaseFileStorage(&fs);
}
