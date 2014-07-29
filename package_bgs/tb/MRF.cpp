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
#include "MRF.h"

using namespace Algorithms::BackgroundSubtraction;

//init the basic MRF
MRF::MRF()
{
  in_image = out_image = 0;
  width = height = 0;

  //////////////////////////////////////////////////////////////////////////
  no_regions = 2;
  beta = 2.8;// 0.9;
  t = 10;//0.05

  //////////////////////////////////////////////////////////////////////////
  K = 0;
  E = E_old = 0;

  //////////////////////////////////////////////////////////////////////////
  T0 = 4;
  c = 0.98;
  T = 0;

  //////////////////////////////////////////////////////////////////////////
  alpha = 0.1;

  //////////////////////////////////////////////////////////////////////////
  classes = 0;
  in_image_data = 0;
  local_evidence = 0;
}

/************************************************************************/
/* the Markov Random Field with time constraints for T2FGMM    */
/************************************************************************/

MRF_TC::MRF_TC()
{
  beta_time = 0.9;
}

MRF_TC::~MRF_TC()
{
  delete []classes;
  delete []old_labeling;
  delete []in_image_data;
  delete []local_evidence;
}

double MRF_TC::TimeEnergy2(int i, int j, int label)
{
  double energy = 0.0;
  
  if(old_labeling[i][j] == (label*255))
    energy -= beta_time;
  else
    energy += beta_time;

  if(i != height-1) // south
  {
    if(label*255 == old_labeling[i+1][j])
      energy -= beta_time;
    else
      energy += beta_time;

    if((j != width-1) && (label*255 == old_labeling[i+1][j+1]))
      energy -= beta_time;
    else
      energy += beta_time;
    
    if((j != 0) && (label*255 == old_labeling[i+1][j-1]))
      energy -= beta_time;
    else
      energy += beta_time;
  }

  if(j != width-1) // east
  {
    if(label*255 == old_labeling[i][j+1])
      energy -= beta_time;
    else
      energy += beta_time;
  }

  if(i != 0) // nord
  {
    if(label*255 == old_labeling[i-1][j])
      energy -= beta_time;
    else
      energy += beta_time;

    if((j != width-1) && (label*255 == old_labeling[i-1][j+1]))
      energy -= beta_time;
    else
      energy += beta_time;
    
    if((j != 0) && (label*255 == old_labeling[i-1][j-1]))
      energy -= beta_time;
    else
      energy += beta_time;
  }

  if(j != 0) // west
  {
    if(label*255 == old_labeling[i][j-1])
      energy -= beta_time;
    else
      energy += beta_time;
  }

  return energy;
}

double MRF_TC::Doubleton2(int i, int j, int label)
{
  double energy = 0.0;

  if(i != height-1) // south
  {
    if(label == classes[i+1][j])
      energy -= beta;
    else
      energy += beta;

    if((j != width-1) && (label == classes[i+1][j+1]))
      energy -= beta;
    else
      energy += beta;
    
    if((j != 0) && (label == classes[i+1][j-1]))
      energy -= beta;
    else
      energy += beta;
  }

  if(j != width-1) // east
  {
    if(label == classes[i][j+1])
      energy -= beta;
    else
      energy += beta;
  }

  if(i != 0) // nord
  {
    if(label == classes[i-1][j])
      energy -= beta;
    else
      energy += beta;

    if((j != width-1) && (label == classes[i-1][j+1]))
      energy -= beta;
    else
      energy += beta;

    if((j != 0) && (label == classes[i-1][j-1]))
      energy -= beta;
    else
      energy += beta;
  }

  if(j != 0) // west
  {
    if(label == classes[i][j-1])
      energy -= beta;
    else
      energy += beta;
  }

  return energy;
}

void MRF_TC::OnIterationOver2(void)
{
  CreateOutput2();
  //cout<<"\rI="<<K<<", ";
}

void MRF_TC::Build_Classes_OldLabeling_InImage_LocalEnergy()
{
  int i;
  classes = new int* [height];
  old_labeling = new int *[height];
  in_image_data = new int* [height];
  local_evidence = new float* [height];

  for(i = 0; i < height; ++i)
  {
    classes[i] = new int[width];
    old_labeling[i] = new int[width];
    in_image_data[i] = new int[width];
    local_evidence[i] = new float[width*2];
  }
}

void MRF_TC::InitEvidence2(GMM *gmm, HMM *hmm, IplImage *labeling)
{
  int i, j;

  background = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
  cvCopy(background2,background.Ptr());

  unsigned char *in_data = (unsigned char *)(in_image->imageData);
  unsigned char *labeling_data = (unsigned char *)(labeling->imageData);

  for(i = 0; i < height; ++i)
  {
    for(j = 0; j < width; ++j)
    {
      in_image_data[i][j] = in_data[(i*in_image->widthStep)+j];
      old_labeling[i][j] = labeling_data[i*width+j];
      
      if(in_image_data[i][j] == 255)
        classes[i][j] = 1;
      else
        classes[i][j] = 0;
    }

    float variance;
    float muR;
    float muG;
    float muB;

    float pixel;

    int modes = 3;
    float mu;	

    for(i = 0; i < height; ++i)
    {
      for(j = 0; j < width; ++j)
      {
        variance = gmm[(i*width+j) * modes+0].variance;
        muR = gmm[(i*width+j) * modes+0].muR;
        muG = gmm[(i*width+j) * modes+0].muG;
        muB = gmm[(i*width+j) * modes+0].muB;

        mu = (muR + muG + muB)/3;

        pixel = (background(i,j,0) + background(i,j,1) + background(i,j,2))/3;

        if(variance == 0) variance = 1;

        local_evidence[i][j*2+0] = pow((pixel-mu),2)/2/variance;

        if(pixel >= mu)
          local_evidence[i][j*2+1] = pow((pixel - mu - 2.5*sqrt(variance)),2)/2/variance;
        else
          local_evidence[i][j*2+1] = pow((pixel - mu + 2.5*sqrt(variance)),2)/2/variance;
      }
    }
  }
}

void MRF_TC::CreateOutput2()
{
  int i, j;
  unsigned char *out_data;

  out_data = (unsigned char *) out_image->imageData;

  // create output image
  for (i = 0; i < height; ++i)
    for(j = 0; j < width; ++j)
      out_data[(i*width) + j] = (unsigned char)((classes[i][j])*255);
}

//calculate the whole energy
double MRF_TC::CalculateEnergy2()
{
  double sum = 0.0;
  int i, j, k;
  // !FAIL!
  for(i = 0; i < height; ++i)
  {
    for(j = 0; j < width; ++j)
    {
      k = classes[i][j];
      sum = sum + local_evidence[i][j*2+k] + Doubleton2(i,j,k) + TimeEnergy2(i, j, k);//min the value
    }
  }
  //sum = 0.1;
  return sum;
}

// local energy
double MRF_TC::LocalEnergy2(int i, int j, int label)
{
  return local_evidence[i][j*2+label] + Doubleton2(i,j,label) + TimeEnergy2(i,j,label);
}

void MRF_TC::ICM2()
{
  int i, j;
  //double summa_deltaE = 0;
  double localenergy0 = 0, localenergy1 = 0;

  K = 0;
  //E_old = CalculateEnergy2();

  do
  {
    for(i = 0; i < height; ++i)
      for(j = 0; j < width; ++j)
      {
        localenergy0 = LocalEnergy2(i,j,0);
        localenergy1 = LocalEnergy2(i,j,1);
        
        if(localenergy0 < localenergy1)
          classes[i][j] = 0;
        else
          classes[i][j] = 1;
      }

      //E = CalculateEnergy2();
      //summa_deltaE = fabs(E_old-E);
      //E_old = E;
      ++K;
      OnIterationOver2();
  }while(K < 2);
}
