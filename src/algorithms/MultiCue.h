#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

#if !defined(__APPLE__)
#include <malloc.h>
#endif
#include "math.h"

#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>

#include "IBGS.h"

//------------------------------------Structure Lists-------------------------------------//
namespace bgslibrary
{
  namespace algorithms
  {
    namespace multiCue
    {
      typedef int BOOL;

      struct point {
        short m_nX;
        short m_nY;
      };

      struct neighbor_pos {
        short m_nX;
        short m_nY;
      };
      //1) Bounding Box Structure
      struct BoundingBoxInfo {
        int m_iBoundBoxNum;										//# of bounding boxes for all foreground and false-positive blobs
        int m_iArraySize;										//the size of the below arrays to store bounding box information

        short *m_aLeft, *m_aRight, *m_aUpper, *m_aBottom;		//arrays to store bounding box information for (the original frame size)
        short *m_aRLeft, *m_aRRight, *m_aRUpper, *m_aRBottom;	//arrays to store bounding box information for (the reduced frame size)
        BOOL* m_ValidBox;										//If this value is true, the corresponding bounding box is for a foreground blob.
                                            //Else, it is for a false-positive blob
      };

      //2) Texture Model Structure
      struct TextureCodeword {
        int m_iMNRL;											//the maximum negative run-length
        int m_iT_first_time;									//the first access time
        int m_iT_last_time;										//the last access time

        float m_fLowThre;										//a low threshold for the matching
        float m_fHighThre;										//a high threshold for the matching
        float m_fMean;											//mean of the codeword
      };

      struct TextureModel {
        TextureCodeword** m_Codewords;							//the texture-codeword Array

        int m_iTotal;											//# of learned samples after the last clear process
        int m_iElementArraySize;								//the array size of m_Codewords
        int m_iNumEntries;										//# of codewords

        BOOL m_bID;												//id=1 --> background model, id=0 --> cachebook
      };

      //3) Color Model Structure
      struct ColorCodeword {
        int m_iMNRL;											//the maximum negative run-length
        int m_iT_first_time;									//the first access time
        int m_iT_last_time;										//the last access time

        double m_dMean[3];										//mean vector of the codeword

      };

      struct ColorModel {
        ColorCodeword** m_Codewords;							//the color-codeword Array

        int m_iTotal;											//# of learned samples after the last clear process
        int m_iElementArraySize;								//the array size of m_Codewords
        int m_iNumEntries;										//# of codewords

        BOOL m_bID;												//id=1 --> background model, id=0 --> cachebookk
      };
    }
  }
}

namespace bgslibrary
{
  namespace algorithms
  {
    //using namespace bgslibrary::algorithms::multiCue;

    class MultiCue : public IBGS
    {
    private:
      void save_config(cv::FileStorage &fs);
      void load_config(cv::FileStorage &fs);

    public:
      typedef bgslibrary::algorithms::multiCue::point point;
      typedef bgslibrary::algorithms::multiCue::TextureModel TextureModel;
      typedef bgslibrary::algorithms::multiCue::BoundingBoxInfo BoundingBoxInfo;
      typedef bgslibrary::algorithms::multiCue::ColorModel ColorModel;
      typedef bgslibrary::algorithms::multiCue::BOOL BOOL;

      MultiCue();
      ~MultiCue();
      //----------------------------------------------------
      //		APIs and User-Adjustable Parameters
      //----------------------------------------------------
      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);			//the main function to background modeling and subtraction

      void GetForegroundMap(IplImage* return_image, IplImage* input_frame = NULL);					//the function returning a foreground binary-map
      void Destroy();																				//the function to release allocated memories

      int g_iTrainingPeriod;										//the training period								(The parameter t in the paper)
      int g_iT_ModelThreshold;									//the threshold for texture-model based BGS.		(The parameter tau_T in the paper)
      int g_iC_ModelThreshold;									//the threshold for appearance based verification.  (The parameter tau_A in the paper)

      float g_fLearningRate;										//the learning rate for background models.			(The parameter alpha in the paper)

      short g_nTextureTrainVolRange;								//the codebook size factor for texture models.		(The parameter k in the paper)
      short g_nColorTrainVolRange;								//the codebook size factor for color models.		(The parameter eta_1 in the paper)

      //----------------------------------------------------
      //	Implemented Function Lists
      //----------------------------------------------------

      //--1) General Functions
      void Initialize(IplImage* frame);

      void PreProcessing(IplImage* frame);
      void ReduceImageSize(IplImage* SrcImage, IplImage* DstImage);
      void GaussianFiltering(IplImage* frame, uchar*** aFilteredFrame);
      void BGR2HSVxyz_Par(uchar*** aBGR, uchar*** aXYZ);

      void BackgroundModeling_Par(IplImage* frame);
      void ForegroundExtraction(IplImage* frame);
      void CreateLandmarkArray_Par(float fConfThre, short nTrainVolRange, float**aConfMap, int iNehborNum, uchar*** aXYZ,
        point*** aNeiDir, TextureModel**** TModel, ColorModel*** CModel, uchar**aLandmarkArr);

      void PostProcessing(IplImage* frame);
      void MorphologicalOpearions(uchar** aInput, uchar** aOutput, double dThresholdRatio, int iMaskSize, int iWidth, int iHeight);
      void Labeling(uchar** aBinaryArray, int* pLabelCount, int** aLabelTable);
      void SetBoundingBox(int iLabelCount, int** aLabelTable);
      void BoundBoxVerification(IplImage* frame, uchar** aResForeMap, BoundingBoxInfo* BoundBoxInfo);
      void EvaluateBoxSize(BoundingBoxInfo* BoundBoxInfo);
      void EvaluateOverlapRegionSize(BoundingBoxInfo* SrcBoxInfo);
      void EvaluateGhostRegion(IplImage* frame, uchar** aResForeMap, BoundingBoxInfo* BoundBoxInfo);
      double CalculateHausdorffDist(IplImage* input_image, IplImage* model_image);
      void RemovingInvalidForeRegions(uchar** aResForeMap, BoundingBoxInfo* BoundBoxInfo);

      void UpdateModel_Par();
      void GetEnlargedMap(float** aOriginMap, float** aEnlargedMap);

      //--2) Texture Model Related Functions
      void T_AllocateTextureModelRelatedMemory();
      void T_ReleaseTextureModelRelatedMemory();
      void T_SetNeighborDirection(point*** aNeighborPos);
      void T_ModelConstruction(short nTrainVolRange, float fLearningRate, uchar*** aXYZ, point center, point* aNei, TextureModel** aModel);
      void T_ClearNonEssentialEntries(short nClearNum, TextureModel** aModel);
      void T_ClearNonEssentialEntriesForCachebook(uchar bLandmark, short* nReferredIdxArr, short nClearNum, TextureModel** pCachebook);
      void T_GetConfidenceMap_Par(uchar*** aXYZ, float** aTextureMap, point*** aNeiDirArr, TextureModel**** aModel);
      void T_Absorption(int iAbsorbCnt, point pos, short*** aContinuCnt, short*** aRefferedIndex, TextureModel** pModel, TextureModel** pCache);

      //--3) Color Model Related Functions
      void C_AllocateColorModelRelatedMemory();
      void C_ReleaseColorModelRelatedMemory();
      void C_CodebookConstruction(uchar* aP, int iPosX, int iPosY, short nTrainVolRange, float fLearningRate, ColorModel* pC);
      void C_ClearNonEssentialEntries(short nClearNum, ColorModel* pModel);
      void C_ClearNonEssentialEntriesForCachebook(uchar bLandmark, short nReferredIdx, short nClearNum, ColorModel* pCachebook);
      void C_Absorption(int iAbsorbCnt, point pos, short** aContinuCnt, short** aRefferedIndex, ColorModel* pModel, ColorModel* pCache);

      //----------------------------------------------------
      //	Implemented Variable Lists
      //----------------------------------------------------

      //--1) General Variables
      int g_iFrameCount;							//the counter of processed frames

      int g_iBackClearPeriod;						//the period to clear background models
      int g_iCacheClearPeriod;					//the period to clear cache-book models

      int g_iAbsortionPeriod;						//the period to absorb static ghost regions
      BOOL g_bAbsorptionEnable;					//If True, procedures for ghost region absorption are activated.

      BOOL g_bModelMemAllocated;					//To handle memory..
      BOOL g_bNonModelMemAllocated;				//To handle memory..

      float g_fConfidenceThre;					//the final decision threshold

      int g_iWidth, g_iHeight;					//width and height of input frames
      int g_iRWidth, g_iRHeight;					//width and height of reduced frames (For efficiency, the reduced size of frames are processed)
      int g_iForegroundNum;						//# of detected foreground regions
      BOOL g_bForegroundMapEnable;				//TRUE only when BGS is successful

      IplImage* g_ResizedFrame;					//reduced size of frame (For efficiency, the reduced size of frames are processed)
      uchar*** g_aGaussFilteredFrame;
      uchar*** g_aXYZFrame;
      uchar** g_aLandmarkArray;					//the landmark map
      uchar** g_aResizedForeMap;					//the resized foreground map
      uchar** g_aForegroundMap;					//the final foreground map
      BOOL** g_aUpdateMap;						//the location map of update candidate pixels

      BoundingBoxInfo* g_BoundBoxInfo;			//the array of bounding boxes of each foreground blob

                                            //--2) Texture Model Related
      TextureModel**** g_TextureModel;			//the texture background model
      TextureModel**** g_TCacheBook;				//the texture cache-book
      short*** g_aTReferredIndex;					//To handle cache-book
      short*** g_aTContinuousCnt;					//To handle cache-book
      point*** g_aNeighborDirection;
      float**g_aTextureConfMap;					//the texture confidence map

      short g_nNeighborNum;						//# of neighborhoods
      short g_nRadius;
      short g_nBoundarySize;

      //--3) Texture Model Related
      ColorModel*** g_ColorModel;					//the color background model
      ColorModel*** g_CCacheBook;					//the color cache-book
      short** g_aCReferredIndex;					//To handle cache-book
      short** g_aCContinuousCnt;					//To handle cache-book
    };

    bgs_register(MultiCue);
  }
}

#endif
