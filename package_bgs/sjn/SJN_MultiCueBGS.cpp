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
//------------------------------------------------------------------------------------------------------------------------------------//
//									A BGS Method Using Multiple Color, Texture, Appearance Cues in the Scene						  //
//																																	  //
//  - Paper: A New Framework for Background Subtraction Using Multiple Cues (ACCV2012)												  //
//  - Code by: SeungJon Noh																											  //
//------------------------------------------------------------------------------------------------------------------------------------//
//#include "StdAfx.h"
#include "SJN_MultiCueBGS.h"
SJN_MultiCueBGS::SJN_MultiCueBGS() : firstTime(true), showOutput(true)
{
  //----------------------------------
  //	User adjustable parameters
  //----------------------------------			
  g_iTrainingPeriod = 20;											//the training period								(The parameter t in the paper)
  g_iT_ModelThreshold = 1;										//the threshold for texture-model based BGS.		(The parameter tau_T in the paper)
  g_iC_ModelThreshold = 10;										//the threshold for appearance based verification.  (The parameter tau_A in the paper)

  g_fLearningRate = 0.05f;											//the learning rate for background models.			(The parameter alpha in the paper)

  g_nTextureTrainVolRange = 15;									//the codebook size factor for texture models.		(The parameter k in the paper)
  g_nColorTrainVolRange = 20;										//the codebook size factor for color models.		(The parameter eta_1 in the paper)

  g_bAbsorptionEnable = TRUE;										//If TRUE, cache-book is also modeled for ghost region removal.
  g_iAbsortionPeriod = 200;										//the period to absorb static ghost regions 

  g_iRWidth = 160, g_iRHeight = 120;								//Frames are precessed after reduced in this size .

  //------------------------------------
  //	For codebook maintenance
  //------------------------------------
  g_iBackClearPeriod = 300;										//the period to clear background models	
  g_iCacheClearPeriod = 30;										//the period to clear cache-book models	

  //------------------------------------
  //	Initialization of other parameters
  //------------------------------------
  g_nNeighborNum = 6, g_nRadius = 2;
  g_fConfidenceThre = g_iT_ModelThreshold / (float)g_nNeighborNum;	//the final decision threshold

  g_iFrameCount = 0;
  g_bForegroundMapEnable = FALSE;									//TRUE only when BGS is successful
  g_bModelMemAllocated = FALSE;									//To handle memory..
  g_bNonModelMemAllocated = FALSE;								//To handle memory..
}

SJN_MultiCueBGS::~SJN_MultiCueBGS(void){
  Destroy();
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//											the main function to background modeling and subtraction									   //																   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel){

  if (img_input.empty())
    return;

  loadConfig();

  if (firstTime)
    saveConfig();

  //--STep1: Background Modeling--//
  //IplImage* frame = &IplImage(img_input);
  IplImage* frame = new IplImage(img_input);
  IplImage* result_image = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 3);
  cvSetZero(result_image);

  if (g_iFrameCount <= g_iTrainingPeriod){
    BackgroundModeling_Par(frame);
    g_iFrameCount++;
  }

  //--Step2: Background Subtraction--//
  else{
    g_bForegroundMapEnable = FALSE;

    ForegroundExtraction(frame);
    UpdateModel_Par();

    //Get BGS Results
    GetForegroundMap(result_image, NULL);
  }
  delete frame;

  cv::Mat temp(result_image, TRUE);
  temp.copyTo(img_output);

  cvReleaseImage(&result_image);

  if (showOutput)
  {
    cv::imshow("MultiCueBGS FG", img_output);
  }

  firstTime = false;
}

void SJN_MultiCueBGS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/MultiCueBGS.xml", 0, CV_STORAGE_WRITE);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void SJN_MultiCueBGS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/MultiCueBGS.xml", 0, CV_STORAGE_READ);

  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//													the system initialization function													   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::Initialize(IplImage* frame)
{
  int i, j;

  g_iHeight = frame->height;
  g_iWidth = frame->width;

  Destroy();

  //--------------------------------------------------------
  // memory initialization
  //--------------------------------------------------------
  g_ResizedFrame = cvCreateImage(cvSize(g_iRWidth, g_iRHeight), IPL_DEPTH_8U, 3);

  g_aGaussFilteredFrame = (uchar***)malloc(sizeof(uchar**)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++){
    g_aGaussFilteredFrame[i] = (uchar**)malloc(sizeof(uchar*)*g_iRWidth);
    for (j = 0; j < g_iRWidth; j++) g_aGaussFilteredFrame[i][j] = (uchar*)malloc(sizeof(uchar)* 3);
  }

  g_aXYZFrame = (uchar***)malloc(sizeof(uchar**)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++){
    g_aXYZFrame[i] = (uchar**)malloc(sizeof(uchar*)*g_iRWidth);
    for (j = 0; j < g_iRWidth; j++) g_aXYZFrame[i][j] = (uchar*)malloc(sizeof(uchar)* 3);
  }

  g_aLandmarkArray = (uchar**)malloc(sizeof(uchar*)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++) g_aLandmarkArray[i] = (uchar*)malloc(sizeof(uchar)*g_iRWidth);

  g_aResizedForeMap = (uchar**)malloc(sizeof(uchar*)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++) g_aResizedForeMap[i] = (uchar*)malloc(sizeof(uchar)*g_iRWidth);

  g_aForegroundMap = (uchar**)malloc(sizeof(uchar*)*g_iHeight);
  for (i = 0; i < g_iHeight; i++) g_aForegroundMap[i] = (uchar*)malloc(sizeof(uchar)*g_iWidth);

  g_aUpdateMap = (BOOL**)malloc(sizeof(BOOL*)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++) g_aUpdateMap[i] = (BOOL*)malloc(sizeof(BOOL)*g_iRWidth);

  //Bound Box Related..
  int iElementNum = 300;
  g_BoundBoxInfo = (BoundingBoxInfo*)malloc(sizeof(BoundingBoxInfo));
  g_BoundBoxInfo->m_iArraySize = iElementNum;
  g_BoundBoxInfo->m_iBoundBoxNum = iElementNum;

  g_BoundBoxInfo->m_aLeft = (short*)malloc(sizeof(short)* iElementNum); g_BoundBoxInfo->m_aRight = (short*)malloc(sizeof(short)* iElementNum);
  g_BoundBoxInfo->m_aBottom = (short*)malloc(sizeof(short)* iElementNum); g_BoundBoxInfo->m_aUpper = (short*)malloc(sizeof(short)* iElementNum);

  g_BoundBoxInfo->m_aRLeft = (short*)malloc(sizeof(short)* iElementNum); g_BoundBoxInfo->m_aRRight = (short*)malloc(sizeof(short)* iElementNum);
  g_BoundBoxInfo->m_aRBottom = (short*)malloc(sizeof(short)* iElementNum); g_BoundBoxInfo->m_aRUpper = (short*)malloc(sizeof(short)* iElementNum);

  g_BoundBoxInfo->m_ValidBox = (BOOL*)malloc(sizeof(BOOL)* iElementNum);

  //--------------------------------------------------------
  // texture model related
  //--------------------------------------------------------
  T_AllocateTextureModelRelatedMemory();

  //--------------------------------------------------------
  // color moddel related
  //--------------------------------------------------------
  C_AllocateColorModelRelatedMemory();

  g_bModelMemAllocated = TRUE;
  g_bNonModelMemAllocated = TRUE;

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//												the function to release allocated memories											       //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::Destroy()
{
  if (g_bModelMemAllocated == FALSE && g_bNonModelMemAllocated == FALSE) return;

  short nNeighborNum = g_nNeighborNum;

  if (g_bModelMemAllocated == TRUE){
    T_ReleaseTextureModelRelatedMemory();
    C_ReleaseColorModelRelatedMemory();

    g_bModelMemAllocated = FALSE;
  }

  if (g_bNonModelMemAllocated == TRUE){

    cvReleaseImage(&g_ResizedFrame);

    for (int i = 0; i < g_iRHeight; i++){
      for (int j = 0; j < g_iRWidth; j++) free(g_aGaussFilteredFrame[i][j]);
      free(g_aGaussFilteredFrame[i]);
    }
    free(g_aGaussFilteredFrame);

    for (int i = 0; i < g_iRHeight; i++){
      for (int j = 0; j < g_iRWidth; j++) free(g_aXYZFrame[i][j]);
      free(g_aXYZFrame[i]);
    }
    free(g_aXYZFrame);

    for (int i = 0; i < g_iRHeight; i++) free(g_aLandmarkArray[i]);
    free(g_aLandmarkArray);

    for (int i = 0; i < g_iRHeight; i++) free(g_aResizedForeMap[i]);
    free(g_aResizedForeMap);

    for (int i = 0; i < g_iHeight; i++) free(g_aForegroundMap[i]);
    free(g_aForegroundMap);

    for (int i = 0; i < g_iRHeight; i++) free(g_aUpdateMap[i]);
    free(g_aUpdateMap);

    free(g_BoundBoxInfo->m_aLeft); free(g_BoundBoxInfo->m_aRight); free(g_BoundBoxInfo->m_aBottom); free(g_BoundBoxInfo->m_aUpper);
    free(g_BoundBoxInfo->m_aRLeft); free(g_BoundBoxInfo->m_aRRight); free(g_BoundBoxInfo->m_aRBottom); free(g_BoundBoxInfo->m_aRUpper);
    free(g_BoundBoxInfo->m_ValidBox);
    free(g_BoundBoxInfo);

    g_bNonModelMemAllocated = FALSE;
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the preprocessing function		    											   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::PreProcessing(IplImage* frame){

  //image resize
  ReduceImageSize(frame, g_ResizedFrame);

  //Gaussian filtering
  GaussianFiltering(g_ResizedFrame, g_aGaussFilteredFrame);

  //color space conversion
  BGR2HSVxyz_Par(g_aGaussFilteredFrame, g_aXYZFrame);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the background modeling function												   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::BackgroundModeling_Par(IplImage* frame){

  //initialization
  if (g_iFrameCount == 0) Initialize(frame);

  //Step1: pre-processing
  PreProcessing(frame);

  int iH_Start = g_nRadius, iH_end = g_iRHeight - g_nRadius;
  int iW_Start = g_nRadius, iW_end = g_iRWidth - g_nRadius;

  float fLearningRate = g_fLearningRate * 4;

  //Step2: background modeling
  for (int i = iH_Start; i < iH_end; i++){
    for (int j = iW_Start; j < iW_end; j++){
      point center;
      center.m_nX = j;
      center.m_nY = i;

      T_ModelConstruction(g_nTextureTrainVolRange, fLearningRate, g_aXYZFrame, center, g_aNeighborDirection[i][j], g_TextureModel[i][j]);
      C_CodebookConstruction(g_aXYZFrame[i][j], j, i, g_nColorTrainVolRange, fLearningRate, g_ColorModel[i][j]);
    }
  }

  //Step3: Clear non-essential codewords
  if (g_iFrameCount == g_iTrainingPeriod){
    for (int i = 0; i < g_iRHeight; i++){
      for (int j = 0; j < g_iRWidth; j++){
        T_ClearNonEssentialEntries(g_iTrainingPeriod, g_TextureModel[i][j]);

        C_ClearNonEssentialEntries(g_iTrainingPeriod, g_ColorModel[i][j]);
      }
    }
    g_iFrameCount++;
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the background subtraction function							                       //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::ForegroundExtraction(IplImage* frame){

  //Step1:pre-processing
  PreProcessing(frame);

  //Step3: texture-model based process
  T_GetConfidenceMap_Par(g_aXYZFrame, g_aTextureConfMap, g_aNeighborDirection, g_TextureModel);

  //Step2: color-model based verification
  CreateLandmarkArray_Par(g_fConfidenceThre, g_nColorTrainVolRange, g_aTextureConfMap, g_nNeighborNum, g_aXYZFrame, g_aNeighborDirection,
    g_TextureModel, g_ColorModel, g_aLandmarkArray);

  //Step3: verification procedures
  PostProcessing(frame);

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the post-processing function													   //												
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::PostProcessing(IplImage* frame){

  //Step1: morphological operation	
  MorphologicalOpearions(g_aLandmarkArray, g_aResizedForeMap, 0.5, 5, g_iRWidth, g_iRHeight);
  g_bForegroundMapEnable = TRUE;

  //Step2: labeling
  int** aLabelTable = (int**)malloc(sizeof(int*)*g_iRHeight);
  for (int i = 0; i < g_iRHeight; i++)  aLabelTable[i] = (int*)malloc(sizeof(int)*g_iRWidth);

  int iLabelCount;
  Labeling(g_aResizedForeMap, &iLabelCount, aLabelTable);

  //Step3: getting bounding boxes for each candidate fore-blob
  SetBoundingBox(iLabelCount, aLabelTable);

  //Step4: size  and appearance based verification
  BoundBoxVerification(frame, g_aResizedForeMap, g_BoundBoxInfo);

  //Step5: Foreground Region
  RemovingInvalidForeRegions(g_aResizedForeMap, g_BoundBoxInfo);

  for (int i = 0; i < g_iRHeight; i++) free(aLabelTable[i]);
  free(aLabelTable);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the background-model update function			                                   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::UpdateModel_Par(){
  short nNeighborNum = g_nNeighborNum;

  //Step1: update map construction
  for (int i = 0; i < g_iRHeight; i++){
    for (int j = 0; j < g_iRWidth; j++){
      g_aUpdateMap[i][j] = TRUE;
    }
  }

  for (int k = 0; k < g_BoundBoxInfo->m_iBoundBoxNum; k++){
    if (g_BoundBoxInfo->m_ValidBox[k] == TRUE){
      for (int i = g_BoundBoxInfo->m_aRUpper[k]; i <= g_BoundBoxInfo->m_aRBottom[k]; i++){
        for (int j = g_BoundBoxInfo->m_aRLeft[k]; j <= g_BoundBoxInfo->m_aRRight[k]; j++){
          g_aUpdateMap[i][j] = FALSE;
        }
      }
    }
  }

  //Step2: update
  int iH_Start = g_nRadius, iH_End = g_iRHeight - g_nRadius;
  int iW_Start = g_nRadius, iW_End = g_iRWidth - g_nRadius;

  float fLearningRate = (float)g_fLearningRate;

  for (int i = iH_Start; i < iH_End; i++){
    for (int j = iW_Start; j < iW_End; j++){

      point center;

      center.m_nX = j;
      center.m_nY = i;

      if (g_aUpdateMap[i][j] == TRUE){
        //model update		
        T_ModelConstruction(g_nTextureTrainVolRange, fLearningRate, g_aXYZFrame, center, g_aNeighborDirection[i][j], g_TextureModel[i][j]);
        C_CodebookConstruction(g_aXYZFrame[i][j], j, i, g_nColorTrainVolRange, fLearningRate, g_ColorModel[i][j]);

        //clearing non-essential codewords
        T_ClearNonEssentialEntries(g_iBackClearPeriod, g_TextureModel[i][j]);
        C_ClearNonEssentialEntries(g_iBackClearPeriod, g_ColorModel[i][j]);

      }
      else {
        if (g_bAbsorptionEnable == TRUE){
          //model update
          T_ModelConstruction(g_nTextureTrainVolRange, fLearningRate, g_aXYZFrame, center, g_aNeighborDirection[i][j], g_TCacheBook[i][j]);
          C_CodebookConstruction(g_aXYZFrame[i][j], j, i, g_nColorTrainVolRange, fLearningRate, g_CCacheBook[i][j]);

          //clearing non-essential codewords
          T_Absorption(g_iAbsortionPeriod, center, g_aTContinuousCnt, g_aTReferredIndex, g_TextureModel[i][j], g_TCacheBook[i][j]);
          C_Absorption(g_iAbsortionPeriod, center, g_aCContinuousCnt, g_aCReferredIndex, g_ColorModel[i][j], g_CCacheBook[i][j]);

        }
      }

      //clearing non-essential codewords for cache-books
      if (g_bAbsorptionEnable == TRUE){
        T_ClearNonEssentialEntriesForCachebook(g_aLandmarkArray[i][j], g_aTReferredIndex[i][j], 10, g_TCacheBook[i][j]);
        C_ClearNonEssentialEntriesForCachebook(g_aLandmarkArray[i][j], g_aCReferredIndex[i][j], 10, g_CCacheBook[i][j]);
      }
    }
  }

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the color based verification function											   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::CreateLandmarkArray_Par(float fConfThre, short nTrainVolRange, float**aConfMap, int iNehborNum, uchar*** aXYZ,
  point*** aNeiDir, TextureModel**** TModel, ColorModel*** CModel, uchar**aLandmarkArr){

  int iBound_w = g_iRWidth - g_nRadius;
  int iBound_h = g_iRHeight - g_nRadius;

  for (int i = 0; i < g_iRHeight; i++){
    for (int j = 0; j < g_iRWidth; j++){

      if (i < g_nRadius || i >= iBound_h || j<g_nRadius || j >= iBound_w) {
        aLandmarkArr[i][j] = 0;
        continue;
      }

      double tmp = aConfMap[i][j];

      if (tmp > fConfThre) aLandmarkArr[i][j] = 255;
      else{
        aLandmarkArr[i][j] = 0;
        //Calculating texture amount in the background
        double dBackAmt, dCnt;
        dBackAmt = dCnt = 0;

        for (int m = 0; m < iNehborNum; m++){
          for (int n = 0; n < TModel[i][j][m]->m_iNumEntries; n++){
            dBackAmt += TModel[i][j][m]->m_Codewords[n]->m_fMean;
            dCnt++;
          }
        }
        dBackAmt /= dCnt;

        //Calculating texture amount in the input image
        double dTemp, dInputAmt = 0;
        for (int m = 0; m < iNehborNum; m++){
          dTemp = aXYZ[i][j][2] - aXYZ[aNeiDir[i][j][m].m_nY][aNeiDir[i][j][m].m_nX][2];

          if (dTemp >= 0) dInputAmt += dTemp;
          else dInputAmt -= dTemp;

        }

        //If there are only few textures in both background and input image
        if (dBackAmt < 50 && dInputAmt < 50){
          //Conduct color codebook matching
          BOOL bMatched = FALSE;
          for (int m = 0; m < CModel[i][j]->m_iNumEntries; m++){

            int iMatchedCount = 0;
            for (int n = 0; n < 3; n++){
              double dLowThre = CModel[i][j]->m_Codewords[m]->m_dMean[n] - nTrainVolRange - 10;
              double dHighThre = CModel[i][j]->m_Codewords[m]->m_dMean[n] + nTrainVolRange + 10;

              if (dLowThre <= aXYZ[i][j][n] && aXYZ[i][j][n] <= dHighThre) iMatchedCount++;
            }

            if (iMatchedCount == 3) {
              bMatched = TRUE;
              break;
            }

          }
          if (bMatched == TRUE) aLandmarkArr[i][j] = 125;
          else aLandmarkArr[i][j] = 255;

        }

      }
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//													the Gaussian filtering function								                           //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::GaussianFiltering(IplImage* frame, uchar*** aFilteredFrame){

  double dSigma = 0.7;

  if (dSigma == 0){
    for (int i = 0; i < g_iRHeight; i++){
      for (int j = 0; j < g_iRWidth; j++){
        aFilteredFrame[i][j][0] = frame->imageData[i*frame->widthStep + j * 3];
        aFilteredFrame[i][j][1] = frame->imageData[i*frame->widthStep + j * 3 + 1];
        aFilteredFrame[i][j][2] = frame->imageData[i*frame->widthStep + j * 3 + 2];
      }
    }
  }
  else
  {
    cv::Mat temp_img(frame, TRUE);
    cv::GaussianBlur(temp_img, temp_img, cv::Size(7, 7), dSigma);

    //Store results into aFilteredFrame[][][]
    //IplImage* img = &IplImage(temp_img);
    IplImage* img = new IplImage(temp_img);
    int iWidthStep = img->widthStep;

    for (int i = 0; i < g_iRHeight; i++){
      for (int j = 0; j < g_iRWidth; j++){
        aFilteredFrame[i][j][0] = img->imageData[i*img->widthStep + j * 3];
        aFilteredFrame[i][j][1] = img->imageData[i*img->widthStep + j * 3 + 1];
        aFilteredFrame[i][j][2] = img->imageData[i*img->widthStep + j * 3 + 2];
      }
    }
    delete img;
  }
}

//------------------------------------------------------------------------------------------------------------------------------------//
//														the image resize function									                 //
//------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::ReduceImageSize(IplImage* SrcImage, IplImage* DstImage){

  int iChannel = 3;

  double dResizeFactor_w = (double)g_iWidth / (double)g_iRWidth;
  double dResizeFactor_h = (double)g_iHeight / (double)g_iRHeight;


  for (int i = 0; i < g_iRHeight; i++){
    for (int j = 0; j < g_iRWidth; j++){
      int iSrcY = (int)(i*dResizeFactor_h);
      int iSrcX = (int)(j*dResizeFactor_w);

      for (int k = 0; k < iChannel; k++) DstImage->imageData[i*DstImage->widthStep + j * 3 + k]
        = SrcImage->imageData[iSrcY*SrcImage->widthStep + iSrcX * 3 + k];
    }
  }

}

//------------------------------------------------------------------------------------------------------------------------------------//
//											    the color space conversion function                                                   //
//------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::BGR2HSVxyz_Par(uchar*** aBGR, uchar*** aXYZ){

  double dH_ratio = (2 * PI) / 360;

  for (int i = 0; i < g_iRHeight; i++){

    double dR, dG, dB;
    double dMax, dMin;

    double dH, dS, dV;

    for (int j = 0; j < g_iRWidth; j++){

      dB = (double)(aBGR[i][j][0]) / 255;
      dG = (double)(aBGR[i][j][1]) / 255;
      dR = (double)(aBGR[i][j][2]) / 255;


      //Find max, min
      dMin = MIN3(dR, dG, dB);
      dMax = MAX3(dR, dG, dB);


      //Get V
      dV = dMax;

      //Get S, H
      if (dV == 0) dS = dH = 0;
      else{

        //S value
        dS = (dMax - dMin) / dMax;

        if (dS == 0) dH = 0;
        else{
          //H value
          if (dMax == dR) {
            dH = 60 * (dG - dB) / dS;
            if (dH < 0) dH = 360 + dH;
          }
          else if (dMax == dG) dH = 120 + 60 * (dB - dR) / dS;
          else dH = 240 + 60 * (dR - dG) / dS;
        }
      }
      dH = dH * dH_ratio;

      aXYZ[i][j][0] = (uchar)((dV * dS * cos(dH) * 127.5) + 127.5);		//X  --> 0~255
      aXYZ[i][j][1] = (uchar)((dV * dS * sin(dH) * 127.5) + 127.5);		//Y  --> 0~255
      aXYZ[i][j][2] = (uchar)(dV * 255);							    //Z  --> 0~255

    }
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//												the function to get enlarged confidence map												   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::GetEnlargedMap(float** aOriginMap, float** aEnlargedMap){
  int i, j;

  short nSrcX;
  short nSrcY;

  double dEWweight, dNSweight;
  double dEWtop, dEWbottom;

  double dNW; //north-west
  double dNE; //north-east
  double dSW; //south-west
  double dSE; //south-east

  double dScaleFactor_w = ((double)g_iWidth) / ((double)g_iRWidth);
  double dScaleFactor_h = ((double)g_iHeight) / ((double)g_iRHeight);

  for (i = 0; i < g_iHeight; i++){
    for (j = 0; j < g_iWidth; j++){
      //backward mapping
      nSrcY = (int)(i / dScaleFactor_h);
      nSrcX = (int)(j / dScaleFactor_w);

      if (nSrcY == (g_iRHeight - 1)) nSrcY -= 1;
      if (nSrcX == (g_iRWidth - 1)) nSrcX -= 1;

      dEWweight = i / dScaleFactor_h - nSrcY;
      dNSweight = j / dScaleFactor_w - nSrcX;

      dNW = (double)aOriginMap[nSrcY][nSrcX];
      dNE = (double)aOriginMap[nSrcY][nSrcX + 1];
      dSW = (double)aOriginMap[nSrcY + 1][nSrcX];
      dSE = (double)aOriginMap[nSrcY + 1][nSrcX + 1];

      // interpolation
      dEWtop = dNW + dEWweight * (dNE - dNW);
      dEWbottom = dSW + dEWweight * (dSE - dSW);

      aEnlargedMap[i][j] = (float)(dEWtop + dNSweight * (dEWbottom - dEWtop));
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//										   				 the morphological operation function				    				   		   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::MorphologicalOpearions(uchar** aInput, uchar** aOutput, double dThresholdRatio, int iMaskSize, int iWidth, int iHeight){

  int iOffset = (int)(iMaskSize / 2);

  int iBound_w = iWidth - iOffset;
  int iBound_h = iHeight - iOffset;

  uchar** aTemp = (uchar**)malloc(sizeof(uchar*)*iHeight);
  for (int i = 0; i < iHeight; i++){
    aTemp[i] = (uchar*)malloc(sizeof(uchar)*iWidth);
  }

  for (int i = 0; i < iHeight; i++){
    for (int j = 0; j < iWidth; j++){
      aTemp[i][j] = aInput[i][j];
    }
  }

  int iThreshold = (int)(iMaskSize*iMaskSize*dThresholdRatio);
  for (int i = 0; i < iHeight; i++){
    for (int j = 0; j < iWidth; j++){

      if (i < iOffset || i >= iBound_h || j < iOffset || j >= iBound_w){
        aOutput[i][j] = 0;
        continue;
      }

      int iCnt = 0;
      for (int m = -iOffset; m <= iOffset; m++){
        for (int n = -iOffset; n <= iOffset; n++){
          if (aTemp[i + m][j + n] == 255) iCnt++;
        }
      }

      if (iCnt >= iThreshold) aOutput[i][j] = 255;
      else aOutput[i][j] = 0;
    }
  }


  for (int i = 0; i < iHeight; i++){
    free(aTemp[i]);
  }
  free(aTemp);

}
//-----------------------------------------------------------------------------------------------------------------------------------------//
//													2-raster scan pass based labeling function											   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::Labeling(uchar** aBinaryArray, int* pLabelCount, int** aLabelTable){
  int x, y, i;		// pass 1,2
  int cnt = 0;		// pass 1
  int label = 0;	// pass 2

  int iSize = g_iRWidth * g_iRHeight;
  int iTableSize = iSize / 2;

  // initialize , table1 table1
  int* aPass1 = (int*)malloc(iSize * sizeof(int));
  int* aTable1 = (int*)malloc(iSize / 2 * sizeof(int));
  int* aTable2 = (int*)malloc(iSize / 2 * sizeof(int));

  memset(aPass1, 0, (iSize)* sizeof(int));
  for (y = 1; y < (g_iRHeight); y++){
    for (x = 1; x < (g_iRWidth); x++){
      aLabelTable[y][x] = 0;
    }
  }

  for (i = 0; i < iTableSize; i++){
    aTable1[i] = i;
  }
  memset(aTable2, 0, iTableSize * sizeof(int));

  // pass 1
  for (y = 1; y < (g_iRHeight); y++){
    for (x = 1; x < (g_iRWidth); x++){

      if (aBinaryArray[y][x] == 255){ // fore ground??
        int up, le;

        up = aPass1[(y - 1)*(g_iRWidth)+(x)]; // up  index
        le = aPass1[(y)*(g_iRWidth)+(x - 1)]; // left index

        // case
        if (up == 0 && le == 0){
          ++cnt;
          aPass1[y * g_iRWidth + x] = cnt;

        }
        else if (up != 0 && le != 0){
          if (up > le){
            aPass1[y *g_iRWidth + x] = le;
            aTable1[up] = aTable1[le]; // update table1 table1
          }
          else{
            aPass1[y * g_iRWidth + x] = up;
            aTable1[le] = aTable1[up]; // update table1 table1
          }
        }
        else{
          aPass1[y * g_iRWidth + x] = up + le;
        }

      }

    }
  }

  // pass 2
  for (y = 1; y < (g_iRHeight); y++){
    for (x = 1; x < (g_iRWidth); x++){

      if (aPass1[y * g_iRWidth + x]){
        int v = aTable1[aPass1[y * g_iRWidth + x]];

        if (aTable2[v] == 0){
          ++label;
          aTable2[v] = label;
        }

        aLabelTable[y][x] = aTable2[v];
      }
    }
  }

  *pLabelCount = label;

  free(aPass1);
  free(aTable1);
  free(aTable2);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//									the function to set bounding boxes for each candidate foreground regions					    	   //																									   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::SetBoundingBox(int iLabelCount, int** aLabelTable){
  int iBoundBoxIndex;

  g_BoundBoxInfo->m_iBoundBoxNum = iLabelCount;

  for (int i = 0; i < g_BoundBoxInfo->m_iBoundBoxNum; i++){
    g_BoundBoxInfo->m_aRLeft[i] = 9999;		//left
    g_BoundBoxInfo->m_aRUpper[i] = 9999;	//top
    g_BoundBoxInfo->m_aRRight[i] = 0;		//right
    g_BoundBoxInfo->m_aRBottom[i] = 0;		//bottom
  }

  //Step1: Set tight bounding boxes
  for (int i = 1; i<g_iRHeight; i++){
    for (int j = 1; j<g_iRWidth; j++){

      if ((aLabelTable[i][j] == 0)) continue;

      iBoundBoxIndex = aLabelTable[i][j] - 1;

      if (g_BoundBoxInfo->m_aRLeft[iBoundBoxIndex] > j) g_BoundBoxInfo->m_aRLeft[iBoundBoxIndex] = j;		//left
      if (g_BoundBoxInfo->m_aRUpper[iBoundBoxIndex] > i) g_BoundBoxInfo->m_aRUpper[iBoundBoxIndex] = i;		//top
      if (g_BoundBoxInfo->m_aRRight[iBoundBoxIndex] < j) g_BoundBoxInfo->m_aRRight[iBoundBoxIndex] = j;		//right
      if (g_BoundBoxInfo->m_aRBottom[iBoundBoxIndex] < i) g_BoundBoxInfo->m_aRBottom[iBoundBoxIndex] = i;	//bottom

    }
  }

  //Step2: Add margins.
  int iBoundary_w = (int)(g_iRWidth / 80), iBoundary_h = (int)(g_iRHeight / 60);

  for (int i = 0; i < g_BoundBoxInfo->m_iBoundBoxNum; i++){

    g_BoundBoxInfo->m_aRLeft[i] -= iBoundary_w;
    if (g_BoundBoxInfo->m_aRLeft[i] < g_nRadius) g_BoundBoxInfo->m_aRLeft[i] = g_nRadius;									//left

    g_BoundBoxInfo->m_aRRight[i] += iBoundary_w;
    if (g_BoundBoxInfo->m_aRRight[i] >= g_iRWidth - g_nRadius) g_BoundBoxInfo->m_aRRight[i] = g_iRWidth - g_nRadius - 1;		    //Right

    g_BoundBoxInfo->m_aRUpper[i] -= iBoundary_h;
    if (g_BoundBoxInfo->m_aRUpper[i] < g_nRadius) g_BoundBoxInfo->m_aRUpper[i] = g_nRadius;									//Top

    g_BoundBoxInfo->m_aRBottom[i] += iBoundary_h;
    if (g_BoundBoxInfo->m_aRBottom[i] >= g_iRHeight - g_nRadius) g_BoundBoxInfo->m_aRBottom[i] = g_iRHeight - g_nRadius - 1;
  }

  double dH_ratio = (double)g_iHeight / (double)g_iRHeight;
  double dW_ratio = (double)g_iWidth / (double)g_iRWidth;

  for (int i = 0; i < g_BoundBoxInfo->m_iBoundBoxNum; i++){
    g_BoundBoxInfo->m_aLeft[i] = (int)(g_BoundBoxInfo->m_aRLeft[i] * dW_ratio);
    g_BoundBoxInfo->m_aUpper[i] = (int)(g_BoundBoxInfo->m_aRUpper[i] * dH_ratio);
    g_BoundBoxInfo->m_aRight[i] = (int)(g_BoundBoxInfo->m_aRRight[i] * dW_ratio);
    g_BoundBoxInfo->m_aBottom[i] = (int)(g_BoundBoxInfo->m_aRBottom[i] * dH_ratio);
  }

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the box verification function													   //																							   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::BoundBoxVerification(IplImage* frame, uchar** aResForeMap, BoundingBoxInfo* BoundBoxInfo){

  //Step1: Verification by the bounding box size
  EvaluateBoxSize(BoundBoxInfo);

  //Step2: Verification by checking whether the region is ghost
  EvaluateGhostRegion(frame, aResForeMap, BoundBoxInfo);

  //Step3: Counting the # of valid box
  g_iForegroundNum = 0;
  for (int i = 0; i < BoundBoxInfo->m_iBoundBoxNum; i++){
    if (BoundBoxInfo->m_ValidBox[i] == TRUE) g_iForegroundNum++;
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//																the size based verification												   //																							   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::EvaluateBoxSize(BoundingBoxInfo* BoundBoxInfo){

  //Set thresholds
  int iLowThreshold_w, iHighThreshold_w;
  iLowThreshold_w = g_iRWidth / 32; if (iLowThreshold_w < 5) iLowThreshold_w = 5;
  iHighThreshold_w = g_iRWidth;

  int iLowThreshold_h, iHighThreshold_h;
  iLowThreshold_h = g_iRHeight / 24; if (iLowThreshold_h < 5) iLowThreshold_h = 5;
  iHighThreshold_h = g_iRHeight;

  int iBoxWidth, iBoxHeight;

  //Perform verification.
  for (int i = 0; i < BoundBoxInfo->m_iBoundBoxNum; i++){

    iBoxWidth = BoundBoxInfo->m_aRRight[i] - BoundBoxInfo->m_aRLeft[i];
    iBoxHeight = BoundBoxInfo->m_aRBottom[i] - BoundBoxInfo->m_aRUpper[i];

    if (iLowThreshold_w <= iBoxWidth && iBoxWidth <= iHighThreshold_w && iLowThreshold_h <= iBoxHeight && iBoxHeight <= iHighThreshold_h) {
      BoundBoxInfo->m_ValidBox[i] = TRUE;
    }
    else BoundBoxInfo->m_ValidBox[i] = FALSE;
  }
}

//------------------------------------------------------------------------------------------------------------------------------------//
//														overlapped region removal													  //
//------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::EvaluateOverlapRegionSize(BoundingBoxInfo* SrcBoxInfo){

  BOOL *aValidBoxFlag = new BOOL[SrcBoxInfo->m_iBoundBoxNum];
  for (int i = 0; i < SrcBoxInfo->m_iBoundBoxNum; i++) aValidBoxFlag[i] = TRUE;

  int size1, size2;
  short *aLeft = SrcBoxInfo->m_aRLeft, *aRight = SrcBoxInfo->m_aRRight;
  short *aTop = SrcBoxInfo->m_aRUpper, *aBottom = SrcBoxInfo->m_aRBottom;

  int iThreshold, iCount, iSmall_Idx, iLarge_Idx;
  double dThreRatio = 0.7;

  for (int i = 0; i < SrcBoxInfo->m_iBoundBoxNum; i++){

    if (SrcBoxInfo->m_ValidBox[i] == FALSE) {
      aValidBoxFlag[i] = FALSE;
      continue;
    }

    size1 = (aRight[i] - aLeft[i]) * (aBottom[i] - aTop[i]);

    for (int j = i; j < SrcBoxInfo->m_iBoundBoxNum; j++){
      if ((i == j) || (SrcBoxInfo->m_ValidBox[j] == FALSE)) continue;

      //Setting threshold for checking overlapped region size
      size2 = (aRight[j] - aLeft[j]) * (aBottom[j] - aTop[j]);

      if (size1 >= size2) {
        iThreshold = (int)(size2 * dThreRatio);
        iSmall_Idx = j; iLarge_Idx = i;
      }
      else {
        iThreshold = (int)(size1 * dThreRatio);
        iSmall_Idx = i; iLarge_Idx = j;
      }

      //Calculating overlapped region size
      iCount = 0;
      for (int m = aLeft[iSmall_Idx]; m < aRight[iSmall_Idx]; m++){
        for (int n = aTop[iSmall_Idx]; n<aBottom[iSmall_Idx]; n++){
          if (aLeft[iLarge_Idx] <= m && m <= aRight[iLarge_Idx] && aTop[iLarge_Idx] <= n && n <= aBottom[iLarge_Idx]) iCount++;
        }
      }

      //Evaluating overlapped region size
      if (iCount > iThreshold) aValidBoxFlag[iSmall_Idx] = FALSE;
    }
  }

  for (int i = 0; i < SrcBoxInfo->m_iBoundBoxNum; i++) SrcBoxInfo->m_ValidBox[i] = aValidBoxFlag[i];
  delete[] aValidBoxFlag;
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														appearance based verification													   //																							   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::EvaluateGhostRegion(IplImage* frame, uchar** aResForeMap, BoundingBoxInfo* BoundBoxInfo){

  double dThreshold = 10;

  BOOL** aUpdateMap = (BOOL**)malloc(sizeof(BOOL*)*g_iRHeight);
  for (int i = 0; i < g_iRHeight; i++){
    aUpdateMap[i] = (BOOL*)malloc(sizeof(BOOL)*g_iRWidth);
    for (int j = 0; j < g_iRWidth; j++) aUpdateMap[i][j] = FALSE;
  }

  //Step1: Conduct fore-region evaluation to identify ghost regions

  for (int i = 0; i < BoundBoxInfo->m_iBoundBoxNum; i++){
    if (BoundBoxInfo->m_ValidBox[i] == TRUE){
      int iWin_w = BoundBoxInfo->m_aRRight[i] - BoundBoxInfo->m_aRLeft[i];
      int iWin_h = BoundBoxInfo->m_aRBottom[i] - BoundBoxInfo->m_aRUpper[i];

      //Generating edge image from bound-boxed frame region
      IplImage* resized_frame = cvCreateImage(cvSize(g_iRWidth, g_iRHeight), IPL_DEPTH_8U, 3);
      cvResize(frame, resized_frame, CV_INTER_NN);

      cvSetImageROI(resized_frame, cvRect(BoundBoxInfo->m_aRLeft[i], BoundBoxInfo->m_aRUpper[i], iWin_w, iWin_h));
      IplImage* edge_frame = cvCreateImage(cvSize(iWin_w, iWin_h), IPL_DEPTH_8U, 1);

      cvCvtColor(resized_frame, edge_frame, CV_BGR2GRAY);
      cvResetImageROI(resized_frame);

      cvCanny(edge_frame, edge_frame, 100, 150);

      //Generating edge image from aResForeMap
      IplImage* edge_fore = cvCreateImage(cvSize(iWin_w, iWin_h), IPL_DEPTH_8U, 1);
      for (int m = BoundBoxInfo->m_aRUpper[i]; m < BoundBoxInfo->m_aRBottom[i]; m++){
        for (int n = BoundBoxInfo->m_aRLeft[i]; n<BoundBoxInfo->m_aRRight[i]; n++){
          edge_fore->imageData[(m - BoundBoxInfo->m_aRUpper[i])*edge_fore->widthStep + (n - BoundBoxInfo->m_aRLeft[i])] = (char)aResForeMap[m][n];
        }
      }
      cvCanny(edge_fore, edge_fore, 100, 150);
      //Calculating partial undirected Hausdorff distance
      double distance = CalculateHausdorffDist(edge_frame, edge_fore);

      //Recording evaluation result
      if (distance > dThreshold) {

        for (int m = BoundBoxInfo->m_aRUpper[i]; m < BoundBoxInfo->m_aRBottom[i]; m++){
          for (int n = BoundBoxInfo->m_aRLeft[i]; n < BoundBoxInfo->m_aRRight[i]; n++){
            aUpdateMap[m][n] = TRUE;
          }
        }

        BoundBoxInfo->m_ValidBox[i] = FALSE;
      }

      cvReleaseImage(&resized_frame);
      cvReleaseImage(&edge_frame);
      cvReleaseImage(&edge_fore);
    }
  }

  //Step2: Adding information fo ghost region pixels to background model
  float fLearningRate = g_fLearningRate;

  for (int i = 0; i < g_iRHeight; i++){
    for (int j = 0; j < g_iRWidth; j++){
      if (aUpdateMap[i][j] == TRUE){
        point center;
        center.m_nX = j; center.m_nY = i;

        T_ModelConstruction(g_nTextureTrainVolRange, fLearningRate, g_aXYZFrame, center, g_aNeighborDirection[i][j], g_TextureModel[i][j]);
        C_CodebookConstruction(g_aXYZFrame[i][j], j, i, g_nColorTrainVolRange, fLearningRate, g_ColorModel[i][j]);

        T_ClearNonEssentialEntries(g_iBackClearPeriod, g_TextureModel[i][j]);
        C_ClearNonEssentialEntries(g_iBackClearPeriod, g_ColorModel[i][j]);

      }
    }
  }

  for (int i = 0; i < g_iRHeight; i++) free(aUpdateMap[i]);
  free(aUpdateMap);
}


//-----------------------------------------------------------------------------------------------------------------------------------------//
//								the function to calculate partial undirected Hausdorff distance(forward distance)						   //																							   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
double SJN_MultiCueBGS::CalculateHausdorffDist(IplImage* input_image, IplImage* model_image){

  //Step1: Generating imag vectors
  //For reduce errors, points at the image boundary are excluded
  vector<point> vInput, vModel;
  point temp;

  //input image --> input vector 
  for (int i = 0; i < input_image->height; i++){
    for (int j = 0; j < input_image->width; j++){

      if ((uchar)input_image->imageData[i*input_image->widthStep + j] == 0) continue;

      temp.m_nX = j; temp.m_nY = i;
      vInput.push_back(temp);
    }
  }
  //model image --> model vector
  for (int i = 0; i < model_image->height; i++){
    for (int j = 0; j < model_image->width; j++){
      if ((uchar)model_image->imageData[i*model_image->widthStep + j] == 0) continue;

      temp.m_nX = j; temp.m_nY = i;
      vModel.push_back(temp);
    }
  }

  if (vInput.empty() && !vModel.empty()) return (double)vModel.size();
  else if (!vInput.empty() && vModel.empty()) return (double)vInput.size();
  else if (vInput.empty() && vModel.empty()) return 0.0;

  //Step2: Calculating forward distance h(Model,Image)
  double dDist, temp1, temp2, dMinDist;
  vector<double> vTempDist;

  for (auto iter_m = vModel.begin(); iter_m < vModel.end(); iter_m++){

    dMinDist = 9999999;
    for (auto iter_i = vInput.begin(); iter_i < vInput.end(); iter_i++){
      temp1 = (*iter_m).m_nX - (*iter_i).m_nX;
      temp2 = (*iter_m).m_nY - (*iter_i).m_nY;
      dDist = temp1*temp1 + temp2*temp2;

      if (dDist < dMinDist) dMinDist = dDist;
    }
    vTempDist.push_back(dMinDist);
  }
  sort(vTempDist.begin(), vTempDist.end()); //in ascending order

  double dQuantileVal = 0.9, dForwardDistance;
  int iDistIndex = (int)(dQuantileVal*vTempDist.size()); if (iDistIndex == vTempDist.size()) iDistIndex -= 1;

  dForwardDistance = sqrt(vTempDist[iDistIndex]);
  return dForwardDistance;

}


//-----------------------------------------------------------------------------------------------------------------------------------------//
//											function to remove non-valid bounding boxes fore fore-candidates							   //												
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::RemovingInvalidForeRegions(uchar** aResForeMap, BoundingBoxInfo* BoundBoxInfo){

  int iBoxNum = BoundBoxInfo->m_iBoundBoxNum;

  for (int k = 0; k < iBoxNum; k++){

    if (BoundBoxInfo->m_ValidBox[k] == FALSE){
      for (int i = BoundBoxInfo->m_aRUpper[k]; i < BoundBoxInfo->m_aRBottom[k]; i++){
        for (int j = BoundBoxInfo->m_aRLeft[k]; j < BoundBoxInfo->m_aRRight[k]; j++){
          if (aResForeMap[i][j] == 255) aResForeMap[i][j] = 0;
        }
      }
    }

  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//													the function returning a foreground binary-map										   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::GetForegroundMap(IplImage* return_image, IplImage* input_frame){

  if (g_bForegroundMapEnable == FALSE) return;

  IplImage* temp_image = cvCreateImage(cvSize(g_iRWidth, g_iRHeight), IPL_DEPTH_8U, 3);

  if (input_frame == NULL){
    for (int i = 0; i < g_iRHeight; i++){
      for (int j = 0; j < g_iRWidth; j++){
        temp_image->imageData[i*temp_image->widthStep + j * 3] = (char)g_aResizedForeMap[i][j];
        temp_image->imageData[i*temp_image->widthStep + j * 3 + 1] = (char)g_aResizedForeMap[i][j];
        temp_image->imageData[i*temp_image->widthStep + j * 3 + 2] = (char)g_aResizedForeMap[i][j];
      }
    }
  }

  else{

    cvResize(input_frame, temp_image);
    CvScalar MixColor;
    MixColor.val[0] = 255;	//B
    MixColor.val[1] = 0;	//G
    MixColor.val[2] = 255;   //R

    for (int i = 0; i < g_iRHeight; i++){
      for (int j = 0; j < g_iRWidth; j++){

        if (g_aResizedForeMap[i][j] == 255) {

          uchar B = (uchar)temp_image->imageData[i*temp_image->widthStep + j * 3];
          uchar G = (uchar)temp_image->imageData[i*temp_image->widthStep + j * 3];
          uchar R = (uchar)temp_image->imageData[i*temp_image->widthStep + j * 3];

          B = (uchar)(B*0.45 + MixColor.val[0] * 0.55);
          G = (uchar)(G*0.45 + MixColor.val[1] * 0.55);
          R = (uchar)(R*0.45 + MixColor.val[2] * 0.55);

          temp_image->imageData[i*temp_image->widthStep + j * 3] = (char)B;
          temp_image->imageData[i*temp_image->widthStep + j * 3 + 1] = (char)G;
          temp_image->imageData[i*temp_image->widthStep + j * 3 + 2] = (char)R;
        }
      }
    }
  }

  cvResize(temp_image, return_image);

  cvReleaseImage(&temp_image);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//												the	initialization function for the texture-models									       //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::T_AllocateTextureModelRelatedMemory(){
  int i, j, k;

  //neighborhood system related
  int iMaxNeighborArraySize = 8;
  g_aNeighborDirection = (point***)malloc(sizeof(point**)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++){
    g_aNeighborDirection[i] = (point**)malloc(sizeof(point*)*g_iRWidth);
    for (j = 0; j < g_iRWidth; j++){
      g_aNeighborDirection[i][j] = (point*)malloc(sizeof(point)*iMaxNeighborArraySize);
    }
  }
  T_SetNeighborDirection(g_aNeighborDirection);

  //texture-model related
  int iElementArraySize = 6;
  g_TextureModel = (TextureModel****)malloc(sizeof(TextureModel***)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++){
    g_TextureModel[i] = (TextureModel***)malloc(sizeof(TextureModel**)*g_iRWidth);
    for (j = 0; j < g_iRWidth; j++){
      g_TextureModel[i][j] = (TextureModel**)malloc(sizeof(TextureModel*)*g_nNeighborNum);
      for (k = 0; k < g_nNeighborNum; k++){
        g_TextureModel[i][j][k] = (TextureModel*)malloc(sizeof(TextureModel));
        g_TextureModel[i][j][k]->m_Codewords = (TextureCodeword**)malloc(sizeof(TextureCodeword*)*iElementArraySize);
        g_TextureModel[i][j][k]->m_iElementArraySize = iElementArraySize;
        g_TextureModel[i][j][k]->m_iNumEntries = 0;
        g_TextureModel[i][j][k]->m_iTotal = 0;
        g_TextureModel[i][j][k]->m_bID = 1;
      }
    }
  }

  g_aTextureConfMap = (float**)malloc(sizeof(float*)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++) g_aTextureConfMap[i] = (float*)malloc(sizeof(float)*g_iRWidth);

  //cache-book related
  if (g_bAbsorptionEnable == TRUE){
    iElementArraySize = iElementArraySize / 2;
    if (iElementArraySize < 3)iElementArraySize = 3;

    g_TCacheBook = (TextureModel****)malloc(sizeof(TextureModel***)*g_iRHeight);
    for (i = 0; i < g_iRHeight; i++){
      g_TCacheBook[i] = (TextureModel***)malloc(sizeof(TextureModel**)*g_iRWidth);
      for (j = 0; j < g_iRWidth; j++){
        g_TCacheBook[i][j] = (TextureModel**)malloc(sizeof(TextureModel*)*g_nNeighborNum);
        for (k = 0; k < g_nNeighborNum; k++){
          g_TCacheBook[i][j][k] = (TextureModel*)malloc(sizeof(TextureModel));
          g_TCacheBook[i][j][k]->m_Codewords = (TextureCodeword**)malloc(sizeof(TextureCodeword*)*iElementArraySize);
          g_TCacheBook[i][j][k]->m_iElementArraySize = iElementArraySize;
          g_TCacheBook[i][j][k]->m_iNumEntries = 0;
          g_TCacheBook[i][j][k]->m_iTotal = 0;
          g_TCacheBook[i][j][k]->m_bID = 0;
        }
      }
    }

    g_aTReferredIndex = (short***)malloc(sizeof(short**)*g_iRHeight);
    g_aTContinuousCnt = (short***)malloc(sizeof(short**)*g_iRHeight);
    for (i = 0; i < g_iRHeight; i++){
      g_aTReferredIndex[i] = (short**)malloc(sizeof(short*)*g_iRWidth);
      g_aTContinuousCnt[i] = (short**)malloc(sizeof(short*)*g_iRWidth);
      for (j = 0; j < g_iRWidth; j++) {
        g_aTReferredIndex[i][j] = (short*)malloc(sizeof(short)*g_nNeighborNum);
        g_aTContinuousCnt[i][j] = (short*)malloc(sizeof(short)*g_nNeighborNum);
        for (k = 0; k < g_nNeighborNum; k++){
          g_aTReferredIndex[i][j][k] = -1;
          g_aTContinuousCnt[i][j][k] = 0;
        }
      }
    }
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
//															the memory release function											           //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::T_ReleaseTextureModelRelatedMemory(){
  int i, j, k, m;
  short nNeighborNum = g_nNeighborNum;

  for (i = 0; i < g_iRHeight; i++){
    for (j = 0; j < g_iRWidth; j++){
      for (k = 0; k < nNeighborNum; k++){
        for (m = 0; m < g_TextureModel[i][j][k]->m_iNumEntries; m++) free(g_TextureModel[i][j][k]->m_Codewords[m]);
        free(g_TextureModel[i][j][k]->m_Codewords);
        free(g_TextureModel[i][j][k]);
      }
      free(g_TextureModel[i][j]);
    }free(g_TextureModel[i]);
  }
  free(g_TextureModel);

  for (i = 0; i < g_iRHeight; i++){
    for (j = 0; j < g_iRWidth; j++) free(g_aNeighborDirection[i][j]);
    free(g_aNeighborDirection[i]);
  }
  free(g_aNeighborDirection);

  for (i = 0; i < g_iRHeight; i++) free(g_aTextureConfMap[i]);
  free(g_aTextureConfMap);

  if (g_bAbsorptionEnable == TRUE){
    for (i = 0; i < g_iRHeight; i++){
      for (j = 0; j < g_iRWidth; j++){
        for (k = 0; k < nNeighborNum; k++){
          for (m = 0; m < g_TCacheBook[i][j][k]->m_iNumEntries; m++) free(g_TCacheBook[i][j][k]->m_Codewords[m]);
          free(g_TCacheBook[i][j][k]->m_Codewords);
          free(g_TCacheBook[i][j][k]);
        }
        free(g_TCacheBook[i][j]);
      }free(g_TCacheBook[i]);
    }
    free(g_TCacheBook);

    for (i = 0; i < g_iRHeight; i++){
      for (j = 0; j < g_iRWidth; j++){
        free(g_aTReferredIndex[i][j]);
        free(g_aTContinuousCnt[i][j]);
      }
      free(g_aTReferredIndex[i]);
      free(g_aTContinuousCnt[i]);
    }
    free(g_aTReferredIndex);
    free(g_aTContinuousCnt);

  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the codebook construction function				                                   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::T_ModelConstruction(short nTrainVolRange, float fLearningRate, uchar*** aXYZ, point center, point* aNei, TextureModel** aModel){

  int i, j;
  int iMatchedIndex;

  short nNeighborNum = g_nNeighborNum;

  float fDifference;
  float fDiffMean;

  float fNegLearningRate = 1 - fLearningRate;

  //for all neighboring pairs 
  for (i = 0; i < nNeighborNum; i++){

    fDifference = (float)(aXYZ[center.m_nY][center.m_nX][2] - aXYZ[aNei[i].m_nY][aNei[i].m_nX][2]);

    //Step1: matching
    iMatchedIndex = -1;
    for (j = 0; j < aModel[i]->m_iNumEntries; j++){
      if (aModel[i]->m_Codewords[j]->m_fLowThre <= fDifference && fDifference <= aModel[i]->m_Codewords[j]->m_fHighThre){
        iMatchedIndex = j;
        break;
      }
    }

    aModel[i]->m_iTotal++;
    //Step2: adding a new element
    if (iMatchedIndex == -1){
      //element array
      if (aModel[i]->m_iElementArraySize == aModel[i]->m_iNumEntries){
        aModel[i]->m_iElementArraySize += 5;
        TextureCodeword **temp = (TextureCodeword**)malloc(sizeof(TextureCodeword*)*aModel[i]->m_iElementArraySize);
        for (j = 0; j < aModel[i]->m_iNumEntries; j++){
          temp[j] = aModel[i]->m_Codewords[j];
          aModel[i]->m_Codewords[j] = NULL;
        }
        free(aModel[i]->m_Codewords);
        aModel[i]->m_Codewords = temp;
      }
      aModel[i]->m_Codewords[aModel[i]->m_iNumEntries] = (TextureCodeword*)malloc(sizeof(TextureCodeword));

      aModel[i]->m_Codewords[aModel[i]->m_iNumEntries]->m_fMean = fDifference;
      aModel[i]->m_Codewords[aModel[i]->m_iNumEntries]->m_fLowThre = aModel[i]->m_Codewords[aModel[i]->m_iNumEntries]->m_fMean - nTrainVolRange;
      aModel[i]->m_Codewords[aModel[i]->m_iNumEntries]->m_fHighThre = aModel[i]->m_Codewords[aModel[i]->m_iNumEntries]->m_fMean + nTrainVolRange;

      aModel[i]->m_Codewords[aModel[i]->m_iNumEntries]->m_iT_first_time = aModel[i]->m_iTotal;
      aModel[i]->m_Codewords[aModel[i]->m_iNumEntries]->m_iT_last_time = aModel[i]->m_iTotal;
      aModel[i]->m_Codewords[aModel[i]->m_iNumEntries]->m_iMNRL = aModel[i]->m_iTotal - 1;
      aModel[i]->m_iNumEntries++;
    }

    //Step3: update
    else{
      fDiffMean = aModel[i]->m_Codewords[iMatchedIndex]->m_fMean;
      aModel[i]->m_Codewords[iMatchedIndex]->m_fMean = fLearningRate*fDifference + fNegLearningRate*fDiffMean;

      aModel[i]->m_Codewords[iMatchedIndex]->m_fLowThre = aModel[i]->m_Codewords[iMatchedIndex]->m_fMean - nTrainVolRange;
      aModel[i]->m_Codewords[iMatchedIndex]->m_fHighThre = aModel[i]->m_Codewords[iMatchedIndex]->m_fMean + nTrainVolRange;

      aModel[i]->m_Codewords[iMatchedIndex]->m_iT_last_time = aModel[i]->m_iTotal;
    }

    //cache-book handling
    if (aModel[i]->m_bID == 1){
      //1. m_iMNRL update
      int negTime;
      for (j = 0; j < aModel[i]->m_iNumEntries; j++){
        //m_iMNRL update
        negTime = aModel[i]->m_iTotal - aModel[i]->m_Codewords[j]->m_iT_last_time + aModel[i]->m_Codewords[j]->m_iT_first_time - 1;
        if (aModel[i]->m_Codewords[j]->m_iMNRL < negTime) aModel[i]->m_Codewords[j]->m_iMNRL = negTime;
      }


      //2. g_aTReferredIndex[center.m_nY][center.m_nX][i] update
      if (g_bAbsorptionEnable == TRUE) g_aTReferredIndex[center.m_nY][center.m_nX][i] = -1;
    }

    else{
      //1. m_iMNRL update
      if (iMatchedIndex == -1) aModel[i]->m_Codewords[aModel[i]->m_iNumEntries - 1]->m_iMNRL = 0;

      //2. g_aTReferredIndex[center.m_nY][center.m_nX][i] update
      if (iMatchedIndex == -1){
        g_aTReferredIndex[center.m_nY][center.m_nX][i] = aModel[i]->m_iNumEntries - 1;
        g_aTContinuousCnt[center.m_nY][center.m_nX][i] = 1;
      }
      else{
        if (iMatchedIndex == g_aTReferredIndex[center.m_nY][center.m_nX][i]) g_aTContinuousCnt[center.m_nY][center.m_nX][i]++;
        else{
          g_aTReferredIndex[center.m_nY][center.m_nX][i] = iMatchedIndex;
          g_aTContinuousCnt[center.m_nY][center.m_nX][i] = 1;
        }
      }
    }

  }

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//												Clear non-essential codewords of the given codebook						                   //																									   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::T_ClearNonEssentialEntries(short nClearNum, TextureModel** aModel){
  int i, n;
  int iStaleThresh = (int)(nClearNum*0.5);
  int iKeepCnt;
  int* aKeep;

  short nNeighborNum = g_nNeighborNum;

  TextureModel* c;

  for (n = 0; n < nNeighborNum; n++){
    c = aModel[n];

    if (c->m_iTotal < nClearNum) continue; //(being operated only when c[w][h]->m_iTotal == nClearNum)

    //Step1: initialization
    aKeep = (int*)malloc(sizeof(int)*c->m_iNumEntries);

    iKeepCnt = 0;

    //Step2: Find non-essential code-words
    for (i = 0; i<c->m_iNumEntries; i++){
      if (c->m_Codewords[i]->m_iMNRL > iStaleThresh) {
        aKeep[i] = 0; //removal candidate
      }
      else {
        aKeep[i] = 1;
        iKeepCnt++;
      }
    }

    //Step3: Perform removal
    if (iKeepCnt == 0 || iKeepCnt == c->m_iNumEntries){
      for (i = 0; i < c->m_iNumEntries; i++){
        c->m_Codewords[i]->m_iT_first_time = 1;
        c->m_Codewords[i]->m_iT_last_time = 1;
        c->m_Codewords[i]->m_iMNRL = 0;
      }
    }

    else{
      iKeepCnt = 0;
      TextureCodeword** temp = (TextureCodeword**)malloc(sizeof(TextureCodeword*)*c->m_iNumEntries);

      for (i = 0; i < c->m_iNumEntries; i++){
        if (aKeep[i] == 1){
          temp[iKeepCnt] = c->m_Codewords[i];
          temp[iKeepCnt]->m_iT_first_time = 1;
          temp[iKeepCnt]->m_iT_last_time = 1;
          temp[iKeepCnt]->m_iMNRL = 0;
          iKeepCnt++;
        }
        else free(c->m_Codewords[i]);
      }

      //ending..
      free(c->m_Codewords);
      c->m_Codewords = temp;
      c->m_iElementArraySize = c->m_iNumEntries;
      c->m_iNumEntries = iKeepCnt;
    }
    c->m_iTotal = 0;
    free(aKeep);

  }

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//								Clear non-essential codewords of the given codebook (only for the cache-book)			                   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::T_ClearNonEssentialEntriesForCachebook(uchar bLandmark, short* nReferredIdxArr, short nClearNum, TextureModel** pCachebook){
  int i, n;
  short nNeighborNum = g_nNeighborNum;

  TextureModel* c;
  short nReferredIdx;

  for (n = 0; n < nNeighborNum; n++){

    c = pCachebook[n];
    nReferredIdx = nReferredIdxArr[n];

    //pCachebook->m_iTotal < nClearNum? --> MNRL update
    if (c->m_iTotal < nClearNum) {
      for (i = 0; i < c->m_iNumEntries; i++){
        if (bLandmark == 255 && i == nReferredIdx) c->m_Codewords[i]->m_iMNRL = 0;
        else c->m_Codewords[i]->m_iMNRL++;
      }

      c->m_iTotal++;
    }

    //Perform clearing
    else{
      int iStaleThreshold = 5;

      int* aKeep;
      short nKeepCnt;

      aKeep = (int*)malloc(sizeof(int)*c->m_iNumEntries);
      nKeepCnt = 0;

      for (i = 0; i < c->m_iNumEntries; i++){
        if (c->m_Codewords[i]->m_iMNRL < iStaleThreshold){
          aKeep[i] = 1;
          nKeepCnt++;
        }
        else aKeep[i] = 0;
      }

      c->m_iElementArraySize = nKeepCnt + 2;
      if (c->m_iElementArraySize < 3) c->m_iElementArraySize = 3;

      TextureCodeword** temp = (TextureCodeword**)malloc(sizeof(TextureCodeword*)*c->m_iElementArraySize);
      nKeepCnt = 0;

      for (i = 0; i < c->m_iNumEntries; i++){
        if (aKeep[i] == 1){
          temp[nKeepCnt] = c->m_Codewords[i];
          temp[nKeepCnt]->m_iMNRL = 0;
          nKeepCnt++;
        }
        else {
          free(c->m_Codewords[i]);
        }

      }

      //ending..
      free(c->m_Codewords);
      c->m_Codewords = temp;
      c->m_iNumEntries = nKeepCnt;
      c->m_iTotal = 0;

      free(aKeep);
    }
  }

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//											the function to generate texture confidence maps				                               //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::T_GetConfidenceMap_Par(uchar*** aXYZ, float** aTextureMap, point*** aNeiDirArr, TextureModel**** aModel){

  int iBound_w = g_iRWidth - g_nRadius;
  int iBound_h = g_iRHeight - g_nRadius;

  short nNeighborNum = g_nNeighborNum;
  float fPadding = 5;

  for (int h = 0; h < g_iRHeight; h++){
    for (int w = 0; w < g_iRWidth; w++){

      if (h < g_nRadius || h >= iBound_h || w < g_nRadius || w >= iBound_w){
        aTextureMap[h][w] = 0;
        continue;
      }

      int nMatchedCount = 0;
      float fDiffSum = 0;
      float fDifference;
      point nei;

      for (int i = 0; i < nNeighborNum; i++){

        nei.m_nX = aNeiDirArr[h][w][i].m_nX;
        nei.m_nY = aNeiDirArr[h][w][i].m_nY;

        fDifference = (float)(aXYZ[h][w][2] - aXYZ[nei.m_nY][nei.m_nX][2]);
        if (fDifference < 0) fDiffSum -= fDifference;
        else fDiffSum += fDifference;

        for (int j = 0; j < aModel[h][w][i]->m_iNumEntries; j++){
          if (aModel[h][w][i]->m_Codewords[j]->m_fLowThre - fPadding <= fDifference && fDifference <= aModel[h][w][i]->m_Codewords[j]->m_fHighThre + fPadding){
            nMatchedCount++;
            break;
          }
        }

      }
      aTextureMap[h][w] = 1 - (float)nMatchedCount / nNeighborNum;
    }
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------------//
//											Absorbing Ghost Non-background Region Update					                               //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::T_Absorption(int iAbsorbCnt, point pos, short*** aContinuCnt, short*** aRefferedIndex, TextureModel** pModel, TextureModel** pCache){
  int i, j, k;
  int iLeavingIndex;

  short g_nRadius = 2;
  short nNeighborNum = g_nNeighborNum;

  for (i = 0; i < nNeighborNum; i++){
    //set iLeavingIndex
    if (aContinuCnt[pos.m_nY][pos.m_nX][i] < iAbsorbCnt) continue;

    iLeavingIndex = aRefferedIndex[pos.m_nY][pos.m_nX][i];

    //array expansion
    if (pModel[i]->m_iElementArraySize == pModel[i]->m_iNumEntries){
      pModel[i]->m_iElementArraySize = pModel[i]->m_iElementArraySize + 5;
      TextureCodeword** temp = (TextureCodeword**)malloc(sizeof(TextureCodeword*)*pModel[i]->m_iElementArraySize);
      for (j = 0; j < pModel[i]->m_iNumEntries; j++) temp[j] = pModel[i]->m_Codewords[j];
      free(pModel[i]->m_Codewords);
      pModel[i]->m_Codewords = temp;
    }

    //movement from the cache-book to the codebook
    pModel[i]->m_Codewords[pModel[i]->m_iNumEntries] = pCache[i]->m_Codewords[iLeavingIndex];

    pModel[i]->m_iTotal = pModel[i]->m_iTotal + 1;

    pModel[i]->m_Codewords[pModel[i]->m_iNumEntries]->m_iT_first_time = pModel[i]->m_iTotal;
    pModel[i]->m_Codewords[pModel[i]->m_iNumEntries]->m_iT_last_time = pModel[i]->m_iTotal;
    pModel[i]->m_Codewords[pModel[i]->m_iNumEntries]->m_iMNRL = pModel[i]->m_iTotal - 1;
    pModel[i]->m_iNumEntries = pModel[i]->m_iNumEntries + 1;

    k = 0;
    TextureCodeword **temp_Cache = (TextureCodeword**)malloc(sizeof(TextureCodeword*)*pCache[i]->m_iElementArraySize);
    for (j = 0; j < pCache[i]->m_iNumEntries; j++){
      if (j == iLeavingIndex) continue;
      else{
        temp_Cache[k] = pCache[i]->m_Codewords[j];
        k++;
      }
    }
    free(pCache[i]->m_Codewords);
    pCache[i]->m_Codewords = temp_Cache;
    pCache[i]->m_iNumEntries = k;
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//													the function to set neighborhood system												   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::T_SetNeighborDirection(point*** aNeighborPos){
  int i, j, k;
  point* aSearchDirection = (point*)malloc(sizeof(point)*g_nNeighborNum);

  aSearchDirection[0].m_nX = -2;//180 degree
  aSearchDirection[0].m_nY = 0;

  aSearchDirection[1].m_nX = -1;//123 degree
  aSearchDirection[1].m_nY = -2;

  aSearchDirection[2].m_nX = 1;//45 degree
  aSearchDirection[2].m_nY = -2;

  aSearchDirection[3].m_nX = 2;//0 degree
  aSearchDirection[3].m_nY = 0;

  aSearchDirection[4].m_nX = 1;//-45 degree
  aSearchDirection[4].m_nY = 2;

  aSearchDirection[5].m_nX = -1;//-135 degree
  aSearchDirection[5].m_nY = 2;

  point temp_pos;

  for (i = 0; i < g_iRHeight; i++){
    for (j = 0; j < g_iRWidth; j++){
      for (k = 0; k < g_nNeighborNum; k++){
        temp_pos.m_nX = j + aSearchDirection[k].m_nX;
        temp_pos.m_nY = i + aSearchDirection[k].m_nY;

        if (temp_pos.m_nX < 0 || temp_pos.m_nX >= g_iRWidth || temp_pos.m_nY < 0 || temp_pos.m_nY >= g_iRHeight) {
          aNeighborPos[i][j][k].m_nX = -1;
          aNeighborPos[i][j][k].m_nY = -1;
        }

        else {
          aNeighborPos[i][j][k].m_nX = temp_pos.m_nX;
          aNeighborPos[i][j][k].m_nY = temp_pos.m_nY;
        }
      }
    }
  }
  free(aSearchDirection);
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//													the color-model initialization function												   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::C_AllocateColorModelRelatedMemory(){
  int i, j;

  int iElementArraySize = 10;

  //codebook initialization
  g_ColorModel = (ColorModel***)malloc(sizeof(ColorModel**)*g_iRHeight);
  for (i = 0; i < g_iRHeight; i++){
    g_ColorModel[i] = (ColorModel**)malloc(sizeof(ColorModel*)*g_iRWidth);
    for (j = 0; j < g_iRWidth; j++){
      //initialization of each CodeBookArray.
      g_ColorModel[i][j] = (ColorModel*)malloc(sizeof(ColorModel));
      g_ColorModel[i][j]->m_Codewords = (ColorCodeword**)malloc(sizeof(ColorCodeword*)*iElementArraySize);
      g_ColorModel[i][j]->m_iNumEntries = 0;
      g_ColorModel[i][j]->m_iElementArraySize = iElementArraySize;
      g_ColorModel[i][j]->m_iTotal = 0;
      g_ColorModel[i][j]->m_bID = 1;
    }
  }

  //cache-book initialization
  if (g_bAbsorptionEnable == TRUE){
    iElementArraySize = 3;

    g_CCacheBook = (ColorModel***)malloc(sizeof(ColorModel**)*g_iRHeight);
    for (i = 0; i < g_iRHeight; i++){
      g_CCacheBook[i] = (ColorModel**)malloc(sizeof(ColorModel*)*g_iRWidth);
      for (j = 0; j < g_iRWidth; j++){
        //initialization of each CodeBookArray.
        g_CCacheBook[i][j] = (ColorModel*)malloc(sizeof(ColorModel));
        g_CCacheBook[i][j]->m_Codewords = (ColorCodeword**)malloc(sizeof(ColorCodeword*)*iElementArraySize);
        g_CCacheBook[i][j]->m_iNumEntries = 0;
        g_CCacheBook[i][j]->m_iElementArraySize = iElementArraySize;
        g_CCacheBook[i][j]->m_iTotal = 0;
        g_CCacheBook[i][j]->m_bID = 0;
      }
    }

    g_aCReferredIndex = (short**)malloc(sizeof(short*)*g_iRHeight);
    g_aCContinuousCnt = (short**)malloc(sizeof(short*)*g_iRHeight);
    for (i = 0; i < g_iRHeight; i++){
      g_aCReferredIndex[i] = (short*)malloc(sizeof(short)*g_iRWidth);
      g_aCContinuousCnt[i] = (short*)malloc(sizeof(short)*g_iRWidth);
      for (j = 0; j < g_iRWidth; j++){
        g_aCReferredIndex[i][j] = -1;
        g_aCContinuousCnt[i][j] = 0;
      }
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//															the memory release function											           //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::C_ReleaseColorModelRelatedMemory(){
  int i, j, k;

  for (i = 0; i < g_iRHeight; i++){
    for (j = 0; j < g_iRWidth; j++){
      for (k = 0; k < g_ColorModel[i][j]->m_iNumEntries; k++){
        free(g_ColorModel[i][j]->m_Codewords[k]);
      }
      free(g_ColorModel[i][j]->m_Codewords);
      free(g_ColorModel[i][j]);
    }
    free(g_ColorModel[i]);
  }
  free(g_ColorModel);

  if (g_bAbsorptionEnable == TRUE){
    for (i = 0; i < g_iRHeight; i++){
      for (j = 0; j < g_iRWidth; j++){
        for (k = 0; k < g_CCacheBook[i][j]->m_iNumEntries; k++){
          free(g_CCacheBook[i][j]->m_Codewords[k]);
        }
        free(g_CCacheBook[i][j]->m_Codewords);
        free(g_CCacheBook[i][j]);
      }
      free(g_CCacheBook[i]);
    }
    free(g_CCacheBook);

    for (i = 0; i < g_iRHeight; i++){
      free(g_aCReferredIndex[i]);
      free(g_aCContinuousCnt[i]);
    }
    free(g_aCReferredIndex);
    free(g_aCContinuousCnt);
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the codebook construction function								                   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::C_CodebookConstruction(uchar* aP, int iPosX, int iPosY, short nTrainVolRange, float fLearningRate, ColorModel* pC){

  //Step1: matching
  short nMatchedIndex;

  float fNegLearningRate = 1 - fLearningRate;

  nMatchedIndex = -1;

  for (int i = 0; i < pC->m_iNumEntries; i++){

    //Checking X
    if (pC->m_Codewords[i]->m_dMean[0] - nTrainVolRange <= aP[0] && aP[0] <= pC->m_Codewords[i]->m_dMean[0] + nTrainVolRange){
      //Checking Y
      if (pC->m_Codewords[i]->m_dMean[1] - nTrainVolRange <= aP[1] && aP[1] <= pC->m_Codewords[i]->m_dMean[1] + nTrainVolRange){
        //Checking Z
        if (pC->m_Codewords[i]->m_dMean[2] - nTrainVolRange <= aP[2] && aP[2] <= pC->m_Codewords[i]->m_dMean[2] + nTrainVolRange){
          nMatchedIndex = i;
          break;
        }
      }
    }
  }

  pC->m_iTotal = pC->m_iTotal + 1;

  //Step2 : adding a new element
  if (nMatchedIndex == -1){
    if (pC->m_iElementArraySize == pC->m_iNumEntries){
      pC->m_iElementArraySize = pC->m_iElementArraySize + 5;
      ColorCodeword **temp = (ColorCodeword**)malloc(sizeof(ColorCodeword*)*pC->m_iElementArraySize);
      for (int j = 0; j < pC->m_iNumEntries; j++){
        temp[j] = pC->m_Codewords[j];
        pC->m_Codewords[j] = NULL;
      }
      free(pC->m_Codewords);
      pC->m_Codewords = temp;

    }
    pC->m_Codewords[pC->m_iNumEntries] = (ColorCodeword*)malloc(sizeof(ColorCodeword));

    pC->m_Codewords[pC->m_iNumEntries]->m_dMean[0] = aP[0];//X
    pC->m_Codewords[pC->m_iNumEntries]->m_dMean[1] = aP[1];//Y
    pC->m_Codewords[pC->m_iNumEntries]->m_dMean[2] = aP[2];//Z

    pC->m_Codewords[pC->m_iNumEntries]->m_iT_first_time = pC->m_iTotal;
    pC->m_Codewords[pC->m_iNumEntries]->m_iT_last_time = pC->m_iTotal;
    pC->m_Codewords[pC->m_iNumEntries]->m_iMNRL = pC->m_iTotal - 1;
    pC->m_iNumEntries = pC->m_iNumEntries + 1;
  }

  //Step3 : update
  else{
    //m_dMean update
    pC->m_Codewords[nMatchedIndex]->m_dMean[0] = (fLearningRate*aP[0]) + fNegLearningRate*pC->m_Codewords[nMatchedIndex]->m_dMean[0];//X
    pC->m_Codewords[nMatchedIndex]->m_dMean[1] = (fLearningRate*aP[1]) + fNegLearningRate*pC->m_Codewords[nMatchedIndex]->m_dMean[1];//Y
    pC->m_Codewords[nMatchedIndex]->m_dMean[2] = (fLearningRate*aP[2]) + fNegLearningRate*pC->m_Codewords[nMatchedIndex]->m_dMean[2];//Z

    pC->m_Codewords[nMatchedIndex]->m_iT_last_time = pC->m_iTotal;
  }

  //cache-book handling
  if (pC->m_bID == 1){
    //1. m_iMNRL update
    int iNegTime;
    for (int i = 0; i < pC->m_iNumEntries; i++){
      //m_iMNRL update
      iNegTime = pC->m_iTotal - pC->m_Codewords[i]->m_iT_last_time + pC->m_Codewords[i]->m_iT_first_time - 1;
      if (pC->m_Codewords[i]->m_iMNRL < iNegTime) pC->m_Codewords[i]->m_iMNRL = iNegTime;
    }

    //2. g_aCReferredIndex[iPosY][iPosX] update
    if (g_bAbsorptionEnable == TRUE) g_aCReferredIndex[iPosY][iPosX] = -1;
  }

  else{
    //1. m_iMNRL update:
    if (nMatchedIndex == -1) pC->m_Codewords[pC->m_iNumEntries - 1]->m_iMNRL = 0;

    //2. g_aCReferredIndex[iPosY][iPosX] update
    if (nMatchedIndex == -1){
      g_aCReferredIndex[iPosY][iPosX] = pC->m_iNumEntries - 1;
      g_aCContinuousCnt[iPosY][iPosX] = 1;
    }
    else{
      if (nMatchedIndex == g_aCReferredIndex[iPosY][iPosX]) g_aCContinuousCnt[iPosY][iPosX]++;
      else{
        g_aCReferredIndex[iPosY][iPosX] = nMatchedIndex;
        g_aCContinuousCnt[iPosY][iPosX] = 1;
      }
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//												Clear non-essential codewords of the given codebook							               //																													   //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::C_ClearNonEssentialEntries(short nClearNum, ColorModel* pModel){
  int i;
  short nStaleThresh = (int)(nClearNum*0.5);
  short nKeepCnt;
  int* aKeep;

  ColorModel* pC = pModel;

  if (pC->m_iTotal < nClearNum) return; //(Being operated only when pC->t >= nClearNum)

  //Step1:initialization
  aKeep = (int*)malloc(sizeof(int)*pC->m_iNumEntries);

  nKeepCnt = 0;

  //Step2: Find non-essential codewords
  for (i = 0; i<pC->m_iNumEntries; i++){
    if (pC->m_Codewords[i]->m_iMNRL > nStaleThresh) {
      aKeep[i] = 0; //removal
    }
    else {
      aKeep[i] = 1; //keep
      nKeepCnt++;
    }
  }

  //Step3: Perform removal
  if (nKeepCnt == 0 || nKeepCnt == pC->m_iNumEntries){
    for (i = 0; i < pC->m_iNumEntries; i++){
      pC->m_Codewords[i]->m_iT_first_time = 1;
      pC->m_Codewords[i]->m_iT_last_time = 1;
      pC->m_Codewords[i]->m_iMNRL = 0;
    }
  }
  else{
    nKeepCnt = 0;
    ColorCodeword** temp = (ColorCodeword**)malloc(sizeof(ColorCodeword*)*pC->m_iNumEntries);

    for (i = 0; i < pC->m_iNumEntries; i++){
      if (aKeep[i] == 1){
        temp[nKeepCnt] = pC->m_Codewords[i];
        temp[nKeepCnt]->m_iT_first_time = 1;
        temp[nKeepCnt]->m_iT_last_time = 1;
        temp[nKeepCnt]->m_iMNRL = 0;
        nKeepCnt++;
      }
      else free(pC->m_Codewords[i]);
    }

    //ending..
    free(pC->m_Codewords);
    pC->m_Codewords = temp;
    pC->m_iElementArraySize = pC->m_iNumEntries;
    pC->m_iNumEntries = nKeepCnt;
  }

  pC->m_iTotal = 0;
  free(aKeep);

}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//										Clear non-essential codewords of the given codebook (for cache-book)				               //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::C_ClearNonEssentialEntriesForCachebook(uchar bLandmark, short nReferredIdx, short nClearNum, ColorModel* pCachebook){
  int i;

  if (pCachebook->m_iTotal < nClearNum) {
    for (i = 0; i < pCachebook->m_iNumEntries; i++){
      if (bLandmark == 255 && i == nReferredIdx) pCachebook->m_Codewords[i]->m_iMNRL = 0;
      else pCachebook->m_Codewords[i]->m_iMNRL++;
    }

    pCachebook->m_iTotal++;
  }

  else{
    int iStaleThreshold = 5;

    int* aKeep;
    short nKeepCnt;

    aKeep = (int*)malloc(sizeof(int)*pCachebook->m_iNumEntries);
    nKeepCnt = 0;

    for (i = 0; i < pCachebook->m_iNumEntries; i++){
      if (pCachebook->m_Codewords[i]->m_iMNRL < iStaleThreshold){
        aKeep[i] = 1;
        nKeepCnt++;
      }
      else aKeep[i] = 0;
    }

    pCachebook->m_iElementArraySize = nKeepCnt + 2;
    if (pCachebook->m_iElementArraySize < 3) pCachebook->m_iElementArraySize = 3;

    ColorCodeword** temp = (ColorCodeword**)malloc(sizeof(ColorCodeword*)*pCachebook->m_iElementArraySize);
    nKeepCnt = 0;

    for (i = 0; i < pCachebook->m_iNumEntries; i++){
      if (aKeep[i] == 1){
        temp[nKeepCnt] = pCachebook->m_Codewords[i];
        temp[nKeepCnt]->m_iMNRL = 0;
        nKeepCnt++;
      }
      else {
        free(pCachebook->m_Codewords[i]);
      }

    }

    //ending..
    free(pCachebook->m_Codewords);
    pCachebook->m_Codewords = temp;
    pCachebook->m_iNumEntries = nKeepCnt;
    pCachebook->m_iTotal = 0;

    free(aKeep);
  }
}

//-----------------------------------------------------------------------------------------------------------------------------------------//
//														the ghost-region absorption function										       //
//-----------------------------------------------------------------------------------------------------------------------------------------//
void SJN_MultiCueBGS::C_Absorption(int iAbsorbCnt, point pos, short** aContinuCnt, short** aRefferedIndex, ColorModel* pModel, ColorModel* pCache){

  //set iLeavingIndex
  if (aContinuCnt[pos.m_nY][pos.m_nX] < iAbsorbCnt) return;

  int iLeavingIndex = aRefferedIndex[pos.m_nY][pos.m_nX];

  //array expansion
  if (pModel->m_iElementArraySize == pModel->m_iNumEntries){
    pModel->m_iElementArraySize = pModel->m_iElementArraySize + 5;
    ColorCodeword** temp = (ColorCodeword**)malloc(sizeof(ColorCodeword*)*pModel->m_iElementArraySize);
    for (int i = 0; i < pModel->m_iNumEntries; i++) temp[i] = pModel->m_Codewords[i];
    free(pModel->m_Codewords);
    pModel->m_Codewords = temp;
  }

  //movement from the cache-book to the codebook
  pModel->m_Codewords[pModel->m_iNumEntries] = pCache->m_Codewords[iLeavingIndex];

  pModel->m_iTotal = pModel->m_iTotal + 1;

  pModel->m_Codewords[pModel->m_iNumEntries]->m_iT_first_time = pModel->m_iTotal;
  pModel->m_Codewords[pModel->m_iNumEntries]->m_iT_last_time = pModel->m_iTotal;
  pModel->m_Codewords[pModel->m_iNumEntries]->m_iMNRL = pModel->m_iTotal - 1;

  pModel->m_iNumEntries = pModel->m_iNumEntries + 1;

  int k = 0;
  ColorCodeword **pTempCache = (ColorCodeword**)malloc(sizeof(ColorCodeword*)*pCache->m_iElementArraySize);
  for (int i = 0; i < pCache->m_iNumEntries; i++){
    if (i == iLeavingIndex) continue;
    else{
      pTempCache[k] = pCache->m_Codewords[i];
      k++;
    }
  }
  free(pCache->m_Codewords);
  pCache->m_Codewords = pTempCache;
  pCache->m_iNumEntries = k;
}
