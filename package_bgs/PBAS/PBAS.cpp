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
#include "PBAS.h"

PBAS::PBAS(void) : N(20), R_lower(18), Raute_min(2), T_lower(2), T_upper(200), R_scale(5), R_incdec(0.05), T_dec(0.05), T_inc(1)
{
  std::cout << "PBAS()" << std::endl;

  //feature vector
  alpha = 7.0;
  beta = 1.0;
  formerMeanNorm = 0;
  width = 0;

  //result image
  foregroundValue = 255;
  backgroundValue = 0;

  //length of random array
  countOfRandomNumb = 1000;

  //the T(x_i) value needs initiation
  T_init = R_lower;

  //check if something is moving in the picture
  isMove = false;

  //for init, count number of runs
  runs = 0;
  newInitialization();
}

void PBAS::newInitialization()
{
  if (!randomN.empty())
    randomN.clear();

  if (!randomX.empty())
    randomX.clear();

  if (!randomY.empty())
    randomY.clear();

  if (!randomMinDist.empty())
    randomMinDist.clear();

  if (!randomT.empty())
    randomT.clear();

  if (!randomTN.empty())
    randomTN.clear();

  for (int l = 0; l < countOfRandomNumb; l++)
  {
    randomN.push_back((int)randomGenerator.uniform((int)0, (int)N));
    randomX.push_back((int)randomGenerator.uniform(-1, +2));
    randomY.push_back((int)randomGenerator.uniform(-1, +2));
    randomMinDist.push_back((int)randomGenerator.uniform((int)0, (int)N));
    randomT.push_back((int)randomGenerator.uniform((int)0, (int)T_upper));
    randomTN.push_back((int)randomGenerator.uniform((int)0, (int)T_upper));
  }
}

PBAS::~PBAS(void)
{
  std::cout << "~PBAS()" << std::endl;

  randomN.clear();
  randomX.clear();
  randomY.clear();
  randomMinDist.clear();
  randomT.clear();
  randomTN.clear();

  for (int k = 0; k < backgroundModel.size(); ++k)
  {
    if (chans == 1)
    {
      backgroundModel.at(k).at(0).release();
      backgroundModel.at(k).at(1).release();
    }
    else
    {
      backgroundModel.at(k).at(0).release();
      backgroundModel.at(k).at(1).release();
      backgroundModel.at(k).at(2).release();

      backgroundModel.at(k).at(3).release();
      backgroundModel.at(k).at(4).release();
      backgroundModel.at(k).at(5).release();
    }
  }

  backgroundModel.clear();
  meanMinDist.release();

  actualR.release();
  actualT.release();

  sobelX.release();
  sobelY.release();
}

bool PBAS::process(cv::Mat* input, cv::Mat* output)
{
  if (width != input->cols)
  {
    width = input->cols;
    chans = input->channels();
    height = input->rows;

    if (input->rows < 1 || input->cols < 1)
    {
      std::cout << "Error: Occurrence of to small (or empty?) image size in PBAS. STOPPING " << std::endl;
      return false;
    }
  }

  //iniate the background model
  init(input);

  resultMap = new cv::Mat(input->rows, input->cols, CV_8UC1);

  //calculate features
  calculateFeatures(&currentFeatures, input);

  //set sumMagnitude to zero at beginning and then sum up in the loop
  sumMagnitude = 0;
  long glCounterFore = 0;
  isMove = false;

  //Here starts the whole processing of each pixel of the image
  // for each pixel
  for (int j = 0; j < resultMap->rows; ++j)
  {
    resultMap_Pt = resultMap->ptr<uchar>(j);
    currentFeaturesM_Pt.clear();
    currentFeaturesC_Pt.clear();
    std::vector<float*> fT;
    std::vector<uchar*> uT;
    B_Mag_Pts.clear();
    B_Col_Pts.clear();

    for (int z = 0; z < chans; ++z)
    {
      currentFeaturesM_Pt.push_back(currentFeatures.at(z).ptr<float>(j));
      currentFeaturesC_Pt.push_back(currentFeatures.at(z + chans).ptr<uchar>(j));

      B_Mag_Pts.push_back(fT);

      B_Col_Pts.push_back(uT);
    }

    meanMinDist_Pt = meanMinDist.ptr<float>(j);
    actualR_Pt = actualR.ptr<float>(j);
    actualT_Pt = actualT.ptr<float>(j);

    for (int k = 0; k < runs; ++k)
    {
      for (int z = 0; z < chans; ++z)
      {
        B_Mag_Pts.at(z).push_back(backgroundModel.at(k).at(z).ptr<float>(j));
        B_Col_Pts.at(z).push_back(backgroundModel.at(k).at(z + chans).ptr<uchar>(j));
      }
    }

    for (int i = 0; i < resultMap->cols; ++i)
    {
      //Compare each pixel to in the worst runtime-case each background model
      int count = 0;
      int index = 0;

      double norm = 0.0;
      double dist = 0.0;
      double minDist = 1000.0;
      int entry = randomGenerator.uniform(3, countOfRandomNumb - 4);

      do
      {
        if (chans == 3)
        {
          norm = sqrt(
            (((double)B_Mag_Pts.at(0).at(index)[i] - ((double)*currentFeaturesM_Pt.at(0)))*((double)B_Mag_Pts.at(0).at(index)[i] - ((double)*currentFeaturesM_Pt.at(0)))) +
            (((double)B_Mag_Pts.at(1).at(index)[i] - ((double)*currentFeaturesM_Pt.at(1)))*((double)B_Mag_Pts.at(1).at(index)[i] - ((double)*currentFeaturesM_Pt.at(1)))) +
            (((double)B_Mag_Pts.at(2).at(index)[i] - ((double)*currentFeaturesM_Pt.at(2)))*((double)B_Mag_Pts.at(2).at(index)[i] - ((double)*currentFeaturesM_Pt.at(2))))
          );

          dist = sqrt(
            (((double)B_Col_Pts.at(0).at(index)[i] - ((double)*currentFeaturesC_Pt.at(0)))*((double)B_Col_Pts.at(0).at(index)[i] - ((double)*currentFeaturesC_Pt.at(0)))) +
            (((double)B_Col_Pts.at(1).at(index)[i] - ((double)*currentFeaturesC_Pt.at(1)))*((double)B_Col_Pts.at(1).at(index)[i] - ((double)*currentFeaturesC_Pt.at(1)))) +
            (((double)B_Col_Pts.at(2).at(index)[i] - ((double)*currentFeaturesC_Pt.at(2)))*((double)B_Col_Pts.at(2).at(index)[i] - ((double)*currentFeaturesC_Pt.at(2))))
          );
        }
        else
        {
          norm = abs((((double)B_Mag_Pts.at(0).at(index)[i] -
            ((double)*currentFeaturesM_Pt.at(0)))*((double)B_Mag_Pts.at(0).at(index)[i] - ((double)*currentFeaturesM_Pt.at(0)))));

          dist = abs((((double)B_Col_Pts.at(0).at(index)[i] -
            ((double)*currentFeaturesC_Pt.at(0)))*((double)B_Col_Pts.at(0).at(index)[i] - ((double)*currentFeaturesC_Pt.at(0))))
          );
        }
        dist = ((double)alpha*(norm / formerMeanMag) + beta*dist);

        if ((dist < *actualR_Pt))
        {
          ++count;
          if (minDist > dist)
            minDist = dist;
        }
        else
        {
          sumMagnitude += (double)(norm);
          ++glCounterFore;
        }
        ++index;
      } while ((count < Raute_min) && (index < runs));


      //#############################################
      //update backgroundmodel
      // is BACKGROUND
      if (count >= Raute_min)
      {
        *resultMap_Pt = 0;
        double ratio = std::ceil((double)T_upper / (double)(*actualT_Pt));
        //in the first run every distance is zero, because there is no background model
        //in the secont run, we have already one image as background model, hence a
        // reasonable minDist could be found -> because of the partly 1/run changing in the running average, we set in the first try meanMinDist to the actual minDist value
        if (runs < N && runs > 2)
        {
          *meanMinDist_Pt = ((((float)(runs - 1)) * (*meanMinDist_Pt)) + (float)minDist) / ((float)runs);
        }
        else if (runs < N && runs == 2)
        {
          *meanMinDist_Pt = (float)minDist;
        }

        //1. update model
        if (runs == N)
        {
          //Update current pixel
          //check if random numer is smaller than ratio
          if (randomT.at(entry) < ratio)
          {
            // replace randomly chosen sample
            int rand = randomN.at(entry + 1); //randomGenerator.uniform((int)0,(int)N-1);
            for (int z = 0; z < chans; ++z)
            {
              B_Mag_Pts.at(z).at(rand)[i] = (float)*currentFeaturesM_Pt.at(z);
              B_Col_Pts.at(z).at(rand)[i] = (uchar)*currentFeaturesC_Pt.at(z);

            }

            *meanMinDist_Pt = ((((float)(N - 1)) * (*meanMinDist_Pt)) + (float)minDist) / ((float)N);
          }

          //Update neighboring pixel model
          if (randomTN.at(entry) < ratio)
          {
            //choose neighboring pixel randomly
            int xNeigh = randomX.at(entry) + i;
            int yNeigh = randomY.at(entry) + j;
            checkValid(&xNeigh, &yNeigh);

            // replace randomly chosen sample
            int rand = randomN.at(entry - 1);
            for (int z = 0; z < chans; ++z)
            {
              (backgroundModel.at(rand)).at(z).at<float>(yNeigh, xNeigh) = currentFeatures.at(z).at<float>(yNeigh, xNeigh);
              (backgroundModel.at(rand)).at(z + chans).at<uchar>(yNeigh, xNeigh) = currentFeatures.at(z + chans).at<uchar>(yNeigh, xNeigh);
            }
          }
        }
      }
      else
      {
        // store pixel as foreground
        *resultMap_Pt = 255;

        //there is some movement
        isMove = true;
      }

      //#######################//#######################//#######################//#######################
      //control loops
      //#######################//#######################//#######################//#######################
      //update R
      decisionThresholdRegulator(actualR_Pt, meanMinDist_Pt);

      //update T
      learningRateRegulator(actualT_Pt, meanMinDist_Pt, resultMap_Pt);

      //#######################//#######################//#######################//#######################
      //#######################//#######################//#######################//#######################

      //jump to next pixel
      ++resultMap_Pt;
      for (int z = 0; z < chans; ++z)
      {
        ++currentFeaturesM_Pt.at(z);
        ++currentFeaturesC_Pt.at(z);
      }

      ++meanMinDist_Pt;
      ++actualR_Pt;
      ++actualT_Pt;
    }
  }

  resultMap->copyTo(*output);

  //if there is no foreground -> no magnitudes fount
  //-> initiate some low value to prevent diving through zero
  double meanMag = sumMagnitude / (double)(glCounterFore + 1); //height*width);

  if (meanMag > 20)
    formerMeanMag = meanMag;
  else
    formerMeanMag = 20;

  delete resultMap;

  for (int z = 0; z < chans; ++z)
  {
    currentFeatures.at(z + chans).release();
    currentFeatures.at(z).release();
  }

  return true;
}

void PBAS::decisionThresholdRegulator(float* pt, float* meanDist)
{
  //update R
  double tempR = *pt;
  double newThresh = (*meanDist)*R_scale;

  if (tempR < newThresh)
  {
    tempR += tempR * R_incdec;
  }
  else
  {
    tempR -= tempR * R_incdec;
  }

  if (tempR >= R_lower)
    *pt = (float)tempR;
  else
    *pt = (float)R_lower;
}

void PBAS::learningRateRegulator(float* pt, float* meanDist, uchar* isFore)
{
  //time update
  double tempT = *pt;

  if ((int)*isFore < 128)
  {
    tempT -= T_inc / (*meanDist + 1.0);
  }
  else
  {
    tempT += T_dec / (*meanDist + 1.0);
  }

  if (tempT > T_lower && tempT < T_upper)
    *pt = (float)tempT;
}

void PBAS::checkValid(int *x, int *y)
{
  if (*x < 0)
  {
    *x = 0;
  }
  else if (*x >= width)
  {
    *x = width - 1;
  }

  if (*y < 0)
  {
    *y = 0;
  }
  else if (*y >= height)
  {
    *y = height - 1;
  }
}

void PBAS::init(cv::Mat* input)
{
  if (runs < N)
  {
    std::vector<cv::Mat> init;
    calculateFeatures(&init, input);
    backgroundModel.push_back(init);

    if (chans == 1)
    {
      init.at(0).release();
      init.at(1).release();
    }
    else
    {
      init.at(0).release();
      init.at(1).release();
      init.at(2).release();
      init.at(3).release();
      init.at(4).release();
      init.at(5).release();
    }

    init.clear();

    if (runs == 0)
    {
      meanMinDist.create(input->size(), CV_32FC1);
      meanMinDist.zeros(input->rows, input->cols, CV_32FC1);

      actualR.create(input->rows, input->cols, CV_32FC1);
      actualT.create(input->rows, input->cols, CV_32FC1);

      float* ptRs, *ptTs; //, *ptM;
      for (int rows = 0; rows < actualR.rows; ++rows)
      {
        ptRs = actualR.ptr<float>(rows);
        ptTs = actualT.ptr<float>(rows);

        for (int cols = 0; cols < actualR.cols; ++cols)
        {
          ptRs[cols] = (float)R_lower;
          ptTs[cols] = (float)T_init;
        }
      }
    }

    ++runs;
  }
}

void PBAS::calculateFeatures(std::vector<cv::Mat>* feature, cv::Mat* inputImage)
{
  if (!feature->empty())
    feature->clear();

  cv::Mat mag[3], dir;

  if (inputImage->channels() == 3)
  {
    std::vector<cv::Mat> rgbChannels(3);
    cv::split(*inputImage, rgbChannels);

    for (int l = 0; l < 3; ++l)
    {
      cv::Sobel(rgbChannels.at(l), sobelX, CV_32F, 1, 0, 3, 1, 0.0);
      cv::Sobel(rgbChannels.at(l), sobelY, CV_32F, 0, 1, 3, 1, 0.0);

      // Compute the L2 norm and direction of the gradient
      cv::cartToPolar(sobelX, sobelY, mag[l], dir, true);
      feature->push_back(mag[l]);
      sobelX.release();
      sobelY.release();
    }

    feature->push_back(rgbChannels.at(0));
    feature->push_back(rgbChannels.at(1));
    feature->push_back(rgbChannels.at(2));
    rgbChannels.at(0).release();
    rgbChannels.at(1).release();
    rgbChannels.at(2).release();
  }
  else
  {
    cv::Sobel(*inputImage, sobelX, CV_32F, 1, 0, 3, 1, 0.0);
    cv::Sobel(*inputImage, sobelY, CV_32F, 0, 1, 3, 1, 0.0);

    // Compute the L2 norm and direction of the gradient
    cv::cartToPolar(sobelX, sobelY, mag[0], dir, true);
    feature->push_back(mag[0]);

    cv::Mat temp;
    inputImage->copyTo(temp);
    feature->push_back(temp);
    temp.release();
  }

  mag[0].release();
  mag[1].release();
  mag[2].release();
  dir.release();
}

void PBAS::setN(int temp)
{
  N = temp;
  newInitialization();
}

void PBAS::setRaute_min(int temp)
{
  Raute_min = temp;
}

void PBAS::setR_lower(double temp)
{
  R_lower = temp;
}

void PBAS::setR_incdec(double temp)
{
  R_incdec = temp;
}

void PBAS::setR_scale(double temp)
{
  R_scale = temp;
}

void PBAS::setT_init(double temp)
{
  T_init = temp;
}

void PBAS::setT_lower(double temp)
{
  T_lower = temp;
}

void PBAS::setT_upper(double temp)
{
  T_upper = temp;
  newInitialization();
}

void PBAS::setT_dec(double temp)
{
  T_dec = temp;
}

void PBAS::setT_inc(double temp)
{
  T_inc = temp;
}

void PBAS::setAlpha(double temp)
{
  alpha = temp;
}

void PBAS::setBeta(double temp)
{
  beta = temp;
}

bool PBAS::isMovement()
{
  return isMove;
}

//cv::Mat* PBAS::getR1_xi()
//{
//	return &actualR;
//}
//
//cv::Mat* PBAS::getT_xi()
//{
//	return &actualT;
//}
