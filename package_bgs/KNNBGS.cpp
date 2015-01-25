/*
 This file is part of BGSLibrary.

 BGSLibrary is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 BGSLibrary is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "KNNBGS.h"

KNNBGS::KNNBGS() :
		firstTime(true), history(500), nSamples(7),
		dist2Threshold(20.0f * 20.0f), knnSamples(0), doShadowDetection(true),
		shadowValue(127), shadowThreshold(0.5f), showOutput(true)
{
	std::cout << "KNNBGS()" << std::endl;
}

KNNBGS::~KNNBGS()
{
	std::cout << "~KNNBGS()" << std::endl;
}

void KNNBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
	if (img_input.empty())
		return;

//------------------------------------------------------------------
// BackgroundSubtractorKNN
// http://docs.opencv.org/trunk/modules/video/doc/motion_analysis_and_object_tracking.html#backgroundsubtractorknn
//
//  The class implements the K nearest neigbours algorithm from:
//  "Efficient Adaptive Density Estimation per Image Pixel for the Task of Background Subtraction"
//  Z.Zivkovic, F. van der Heijden
//  Pattern Recognition Letters, vol. 27, no. 7, pages 773-780, 2006
//  http:  //www.zoranz.net/Publications/zivkovicPRL2006.pdf
//
//  Fast for small foreground object. Results on the benchmark data is at http://www.changedetection.net.
//------------------------------------------------------------------

	int prevNSamples = nSamples;
	loadConfig();
	if (firstTime) {
		saveConfig();
		knn = cv::createBackgroundSubtractorKNN(history, dist2Threshold, doShadowDetection);
	}

	knn->setNSamples(nSamples);
	knn->setkNNSamples(knnSamples);
	knn->setShadowValue(shadowValue);
	knn->setShadowThreshold(shadowThreshold);

	knn->apply(img_input, img_output, prevNSamples != nSamples ? 0.f : 1.f);
	knn->getBackgroundImage(img_bgmodel);

	if (showOutput) {
		if (!img_output.empty())
			cv::imshow("KNN FG", img_output);

		if (!img_bgmodel.empty())
			cv::imshow("KNN BG", img_bgmodel);
	}

	firstTime = false;
}

void KNNBGS::saveConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("./config/KNNBGS.xml", 0, CV_STORAGE_WRITE);

	cvWriteInt(fs, "history", history);
	cvWriteInt(fs, "nSamples", nSamples);
	cvWriteReal(fs, "dist2Threshold", dist2Threshold);
	cvWriteInt(fs, "knnSamples", knnSamples);
	cvWriteInt(fs, "doShadowDetection", doShadowDetection);
	cvWriteInt(fs, "shadowValue", shadowValue);
	cvWriteReal(fs, "shadowThreshold", shadowThreshold);
	cvWriteInt(fs, "showOutput", showOutput);

	cvReleaseFileStorage(&fs);
}

void KNNBGS::loadConfig()
{
	CvFileStorage* fs = cvOpenFileStorage("./config/KNNBGS.xml", 0, CV_STORAGE_READ);

	history = cvReadIntByName(fs, 0, "history", 500);
	nSamples = cvReadIntByName(fs, 0, "nSamples", 7);
	dist2Threshold = cvReadRealByName(fs, 0, "dist2Threshold", 20.0f * 20.0f);
	knnSamples = cvReadIntByName(fs, 0, "knnSamples", 0);
	doShadowDetection = cvReadIntByName(fs, 0, "doShadowDetection", 1);
	shadowValue = cvReadIntByName(fs, 0, "shadowValue", 127);
	shadowThreshold = cvReadRealByName(fs, 0, "shadowThreshold", 0.5f);
	showOutput = cvReadIntByName(fs, 0, "showOutput", true);

	cvReleaseFileStorage(&fs);
}
