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
#include <iostream>
#include <opencv2/opencv.hpp>

#include "package_bgs/bgslibrary.h"

int main(int argc, char **argv)
{
  std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

  /* Background Subtraction Methods */
  IBGS *bgs;

  bgs = new FrameDifference;
  //bgs = new StaticFrameDifference;
  //bgs = new WeightedMovingMean;
  //bgs = new WeightedMovingVariance;
  //bgs = new MixtureOfGaussianV1; // only on OpenCV 2.x
  //bgs = new MixtureOfGaussianV2;
  //bgs = new AdaptiveBackgroundLearning;
  //bgs = new AdaptiveSelectiveBackgroundLearning;
  //bgs = new GMG; // only on OpenCV 2.x
  //bgs = new KNN; // only on OpenCV 3.x
  //bgs = new DPAdaptiveMedian;
  //bgs = new DPGrimsonGMM;
  //bgs = new DPZivkovicAGMM;
  //bgs = new DPMean;
  //bgs = new DPWrenGA;
  //bgs = new DPPratiMediod;
  //bgs = new DPEigenbackground;
  //bgs = new DPTexture;
  //bgs = new T2FGMM_UM;
  //bgs = new T2FGMM_UV;
  //bgs = new T2FMRF_UM;
  //bgs = new T2FMRF_UV;
  //bgs = new FuzzySugenoIntegral;
  //bgs = new FuzzyChoquetIntegral;
  //bgs = new MultiLayer;
  //bgs = new PixelBasedAdaptiveSegmenter;
  //bgs = new LBSimpleGaussian;
  //bgs = new LBFuzzyGaussian;
  //bgs = new LBMixtureOfGaussians;
  //bgs = new LBAdaptiveSOM;
  //bgs = new LBFuzzyAdaptiveSOM;
  //bgs = new LBP_MRF;
  //bgs = new VuMeter;
  //bgs = new KDE;
  //bgs = new IndependentMultimodal;
  //bgs = new MultiCue;
  //bgs = new SigmaDelta;
  //bgs = new SuBSENSE;
  //bgs = new LOBSTER;
  //bgs = new PAWCS;
  //bgs = new TwoPoints;
  //bgs = new ViBe;

  int frameNumber = 1;
  int key = 0;
  while (key != 'q')
  {
    std::stringstream ss;
    ss << frameNumber;
    std::string fileName = "dataset/frames/" + ss.str() + ".png";
    std::cout << "reading " << fileName << std::endl;

    cv::Mat img_input = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);

    if (img_input.empty())
      break;

    cv::imshow("input", img_input);

    cv::Mat img_mask;
    cv::Mat img_bkgmodel;
    bgs->process(img_input, img_mask, img_bkgmodel);
    // by default, "bgs->process(.)" automatically shows the foreground mask image
    // or set "bgs->setShowOutput(false)" to disable

    //if(!img_mask.empty())
    //  cv::imshow("Foreground", img_mask);
    //  do something

    key = cvWaitKey(33);
    frameNumber++;
  }
  cvWaitKey(0);
  delete bgs;
  cvDestroyAllWindows();

  return 0;
}
