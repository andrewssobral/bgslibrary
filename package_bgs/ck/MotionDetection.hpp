/*
 *  This file is part of the AiBO+ project
 *
 *  Copyright (C) 2005-2013 Csaba Kertész (csaba.kertesz@gmail.com)
 *
 *  AiBO+ is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AiBO+ is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 *
 */

#ifndef MotionDetection_hpp
#define MotionDetection_hpp

/**
 *  @addtogroup mindeye
 *  @{
 */

#include "MEDefs.hpp"
#include "MEImage.hpp"

class CvBGStatModel;
class CvPoint2D32f;

// Struct for histogram update data of a pixel
struct MEPixelDataType;

/**
 * MotionDetection
 * @brief Extract moving objects from image sequence
 */
class MotionDetection
{
public:

  /// Types of motion detection
  typedef enum
  {
    md_Min = 0,               /*!< Minimum value */
    md_NotDefined = md_Min,   /*!< Not defined */
    md_DLBPHistograms,        /*!< Dynamic LBP */
    md_LBPHistograms,         /*!< Normal LBP */
    md_Max = md_LBPHistograms /*!< Maximum value */
  } DetectorType;

  /// Types of sample mask
  typedef enum
  {
    sm_Min = 0,              /*!< Minimum value */
    sm_Circle = sm_Min,      /*!< Circle */
    sm_Square,               /*!< Square */
    sm_Ellipse,              /*!< Ellipse */
    sm_RandomPixels,         /*!< Random pixels */
    sm_Max = sm_RandomPixels /*!< Maximum value */
  } SampleMaskType;

  /// Types of motion detection parameters
  typedef enum
  {
    mdp_Min = 0,                         /*!< Minimum value */
    mdp_HUProximityThreshold = mdp_Min,  /*!< Proximity threshold */
    mdp_HUBackgroundThreshold,           /*!< Background threshold */
    mdp_HUHistogramLearningRate,         /*!< Histogram learning rate */
    mdp_HUWeightsLearningRate,           /*!< Weights learning rate */
    mdp_HUMinCutWeight,                  /*!< Minimum cut weight */
    mdp_HUDesiredSamplePixels,           /*!< Desired sample pixels */
    mdp_HUHistogramsPerPixel,            /*!< Histogram per pixel */
    mdp_HUHistogramArea,                 /*!< Histogram area */
    mdp_HUHistogramBins,                 /*!< Histogram bins */
    mdp_HUColorSpace,                    /*!< Color space */
    mdp_HULBPMode,                       /*!< LBP mode */
    mdp_Max = mdp_HULBPMode              /*!< Maximum value */
  } ParametersType;

  /*!
   * @brief Class constructor
   *
   * @param mode Detection mode
   *
   * Class constructor with the possibility to specify the detection mode.
   * The default is dynamic LBP.
   *
   */

  MotionDetection(DetectorType mode = md_DLBPHistograms);
  /// Destructor of class
  ~MotionDetection();

  /*
  -------------------------------------------------------------------
                           Motion methods
  -------------------------------------------------------------------
  */

  /*!
   * @brief Set the mode of the motion detection
   *
   * @param newmode New mode of detection
   *
   * Set the mode of the motion detection.
   *
   */

  void SetMode(DetectorType newmode);

  /*!
   * @brief Get a parameter value of the motion detection
   *
   * @param param Parameter of the detection
   *
   * @return Queried value
   *
   * Get the value of a parameter of the motion detection.
   *
   */

  float GetParameter(ParametersType param) const;

  /*!
   * @brief Set a parameter of the motion detection
   *
   * @param param Parameter of the detection
   * @param value New value
   *
   * Set a new value to a parameter of the motion detection.
   *
   */

  void SetParameter(ParametersType param, float value);

  /*!
   * @brief Detect the motions on an image
   *
   * @param image Image to process
   *
   * The function designed to search motions in image streams
   * thus it needs to process the image sequence frame by frame.
   * It processes an image from this sequence and searches moving blobs
   * on that.
   *
   */

  void DetectMotions(MEImage& image);

  /*!
   * @brief Get mask image with detected motions
   *
   * @param mask_image Result mask image
   *
   * The function creates a mask image on which the objects are
   * indicated by white blobs.
   *
   */

  void GetMotionsMask(MEImage& mask_image);

  /*!
   * @brief Calculate results of the motion detection
   *
   * @param referenceimage Reference mask image
   * @param tnegatives True negative pixels
   * @param tpositives True positive pixels
   * @param ttnegatives Total true negative pixels
   * @param ttpositives Total true positive pixels
   *
   * The function calculates the results of the motion detection
   * between the current motion mask and a given reference mask
   * image.
   *
   */

  void CalculateResults(MEImage& referenceimage, int& tnegatives, int& tpositives,
                        int& ttnegatives, int& ttpositives);

private:

  /*!
   * @brief Release data structures
   *
   * Function releases the data structures.
   *
   */

  void ReleaseData();

  /*
  -------------------------------------------------------------------
                    Histogram update methods
  -------------------------------------------------------------------
  */

  /*!
   * @brief Init HU data structures
   *
   * @param imagewidth Image width for HU to process
   * @param imageheight Image height for HU to process
   *
   * Function allocates/re-allocates the HU data structures and they
   * are cleared if needs be.
   *
   */

  void InitHUData(int imagewidth, int imageheight);

  /*!
   * @brief Init HU optical flow data structures
   *
   * @param imagewidth Image width for HU to process
   * @param imageheight Image height for HU to process
   *
   * Function allocates/re-allocates the HU optical flow
   * data structures.
   *
   */

  void InitHUOFData(int imagewidth, int imageheight);

  /*!
   * @brief Release HU data structures
   *
   * Function releases the HU data structures.
   *
   */

  void ReleaseHUData();

  /*!
   * @brief Release HU optical flow data structures
   *
   * Function releases the HU optical flow data structures.
   *
   */

  void ReleaseHUOFData();

  /*!
   * @brief Clear HU data structures
   *
   * Function clears the HU data structures.
   *
   */

  void ClearHUData();

  /*!
   * @brief Get mask image with detected motions by histogram update
   *
   * @param mask_image Result mask image
   *
   * The function creates a mask image on which the objects are
   * indicated by white blobs.
   *
   */

  void GetMotionsMaskHU(MEImage& mask_image);

  /*!
   * @brief Set the sample mask
   *
   * @param mask_type Type of the mask
   * @param desiredarea The desired area size of the mask
   *
   * The function creates a sample mask with a desired form
   * (square, circle, ellipse, random pixels) and size.
   *
   */

  void SetSampleMaskHU(SampleMaskType mask_type, int desiredarea);

  /*!
   * @brief Detect the motions on an image with histogram update
   *
   * @param image Image to process
   *
   * The function designed to search motions in image streams
   * thus it needs to process the image sequence frame by frame.
   * It processes an image from this sequence and searches moving blobs
   * on that. It uses histogram update method.
   *
   */

  void DetectMotionsHU(MEImage& image);

  /*!
   * @brief Update a model
   *
   * @param image Image to process
   * @param model Model to update
   *
   * The function updates a histogram model of the image.
   *
   */

  void UpdateModelHU(MEImage& image, MEPixelDataType*** model);

  /*!
   * @brief Update the HU data structure for one pixel
   *
   * @param pixeldata Pixel data
   * @param histogram Current histogram
   *
   * This method updates the HU data for one pixel.
   *
   */

  void UpdateHUPixelData(MEPixelDataType* pixeldata, const float *histogram);

  /*!
   * @brief Optical flow correction of the camera movements
   *
   * The function trackes some points on the scene if a camera movement is
   * detected, then the LBP pixel data is corrected.
   *
   */

  void OpticalFlowCorrection();

private:
  // GENERAL VARIABLES
  /// Motion detection type
  DetectorType MDMode;
  /// State of the data structures
  MEProcessStateType MDDataState;
  /// Processed number in the image sequence
  int Frames;
  /// Store the current image
  MEImage CurrentImage;
  /// Store the previous image
  MEImage PreviousImage;
  /// Store the current mask image
  MEImage MaskImage;
  /// Store the current mask image
  bool ReadyMask;
  // HISTOGRAM UPDATE VARIABLES
  /// Color space (-1 = no conversion)
  int HUColorSpace;
  /// LBP calculation mode (-1 = no conversion)
  int HULBPMode;
  /// Histograms per pixel
  int HUHistogramsPerPixel;
  /// Histogram area
  int HUHistogramArea;
  /// Histogram bins
  int HUHistogramBins;
  /// Image width for histogram update
  int HUImageWidth;
  /// Image height for histogram update
  int HUImageHeight;
  /// Data of the LBP histograms
  MEPixelDataType ***HULBPPixelData;
  /// Store the previous blue layer
  MEImage PreviousBlueLayer;
  /// Histogram proximity threshold
  float HUPrThres;
  /// Background selection threshold
  float HUBackgrThres;
  /// Histogram learning rate
  float HUHistLRate;
  /// Weights learning rate
  float HUWeightsLRate;
  /// Pixel number used to calculate the histograms
  int HUSamplePixels;
  /// The desired pixel number used to calculate the histograms (-1 = Auto)
  int HUDesiredSamplePixels;
  /// Min cut weight
  float HUMinCutWeight;
  /// Auxiliary variable for computing the histograms in a column
  int **HUMaskColumnAddDel;
  /// Auxiliary variable for computing the histograms in a row
  int **HUMaskRowAddDel;
  // OPTICAL FLOW VARIABLES
  /// State of the optical flow
  MEProcessStateType HUOFDataState;
  /// Number of the tracked points with optical flow
  int HUOFPointsNumber;
  /// Tracked points
  CvPoint2D32f* HUOFPoints[2];
  /// The rest x component of previous camera movement
  int HUOFCamMovementX;
  /// Maximum tracked points detected in one cycle
  int MaxTrackedPoints;
  /// Processed frame number with optical flow in the image sequence
  int HUOFFrames;
  /// Indicator of a new camera movement
  bool HUOFCamMovement;
};

/** @} */

#endif
