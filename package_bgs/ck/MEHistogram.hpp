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

#ifndef MEHistogram_hpp
#define MEHistogram_hpp

/**
 *  @addtogroup mindeye
 *  @{
 */

class MEImage;

/**
 * MEHistogram
 * @brief The class provides basic histogram operations
 */
class MEHistogram
{
public:

  /// Types of histogram calculation
  typedef enum {
    h_Min = 0,           /*!< Minimum value */
    h_Overwrite = h_Min, /*!< Overwrite */
    h_Add,               /*!< Add */
    h_Max = h_Add        /*!< Maximum value */
  } HistogramType;

  /// Types of histogram stretching
  typedef enum {
    s_Min = 0,          /*!< Minimum value */
    s_OwnMode = s_Min,  /*!< Own mode */
    s_GimpMode,         /*!< Gimp mode */
    s_Max = s_GimpMode  /*!< Maximum value */
  } StretchType;

  /// Constructor of class
  MEHistogram();
  /// Destructor of class
  ~MEHistogram();

  /*!
   * @brief Clear histogram data
   *
   * Clear histogram data.
   *
   */

  void Clear();

  /*!
   * @brief Equality (==) operator
   *
   * @param histogram Histogram to be compared
   *
   * @return True if the two histograms are equal.
   *
   * Compare two histograms.
   *
   */

  bool operator==(MEHistogram& histogram) const;

  /*!
   * @brief Calculate the histogram of one color channel
   *
   * @param image Given image for the calculations
   * @param channel Selected color channel for calculation (Range: 1..x)
   * @param mode The mode of calculation.
   *
   * The method calculates the histograms of a color channel.
   * There is two different type of the function:
   *
   * - h_Add: Add the data to the existing histogram.
   * - h_Overwrite: Clear the histogram data before the
   * calculation.
   *
   */

  void Calculate(MEImage& image, int channel, HistogramType mode);

  /*!
   * @brief Calculate the average histogram of an image
   *
   * @param image Given image for the calculations
   * @param mode Histogram calculation mode
   *
   * The method calculates the average histogram of an image.
   *
   */

  void Calculate(MEImage& image, HistogramType mode = h_Overwrite);

  /*!
   * @brief Calculate the histogram of an image region
   *
   * @param image Given image for the calculations
   * @param channel Selected color channel for calculation (Range: 1..x)
   * @param x0 x0 coordinate of the region
   * @param y0 y0 coordinate of the region
   * @param x1 x1 coordinate of the region
   * @param y1 y1 coordinate of the region
   *
   * The method calculates the average histogram of an image region
   * (x0,y0)-(x1,y1).
   *
   */

  void Calculate(MEImage& image, int channel, int x0, int y0, int x1, int y1);

  /*!
   * @brief Get the index of maximum value of the histogram
   *
   * @return Index number
   *
   * Function gives an index value back where is the highest
   * peak of the histogram.
   *
   */

  int GetPeakIndex() const;

  /*!
   * @brief Get the lowest histogram index with an threshold value
   *
   * @param threshold Specified threshold (in percent: 0..100 %)
   *
   * @return Index number
   *
   * Function gives the lowest index back whose value reaches
   * an threshold value calculated by (counted pixel number /
   * 10*threshold / 100).
   *
   */

  int GetLowestLimitIndex(int threshold) const;

  /*!
   * @brief Get the highest histogram index with an threshold value
   *
   * @param threshold Specified threshold (in percent: 0..100 %)
   *
   * @return Index number
   *
   * Function gives the highest index back whose value reaches
   * an threshold value calculated by (counted pixel number /
   * 10*threshold / 100).
   *
   */

  int GetHighestLimitIndex(int threshold) const;

  /*!
   * @brief Get the amount of the histogram values in an interval
   *
   * @param minindex Minimal index of the interval
   * @param maxindex Maximal index of the interval
   *
   * @return Amount of the values
   *
   * Function calculates the amount of the histogram values
   * in a given interval.
   *
   */

  int GetPowerAmount(int min_index, int max_index) const;

  /*!
   * @brief Get index value of the centroid point of the histogram
   *
   * @return Index number
   *
   * Function calculates the centre of area of the histogram and
   * gives the index number back.
   *
   */

  int GetCentroidIndex() const;

  /*!
   * @brief Stretch the histogram
   *
   * @param mode Mode of the histogram stretching
   *
   * @return True if successful, otherwise false.
   *
   * The function selects and stretches the main power
   * interval of the histogram. The following calculation
   * modes are available:
   *
   * - s_OwnMode: The calculation of the power
   * interval is selected by functions Histogram::GetHistogramLowestLimitIndex()
   * and Histogram::GetHistogramHighestLimitIndex() where the
   * threshold is 20, 10, 5, 2, 1 in order. The power range will
   * be selected if the length is at least 52 long or the used
   * threshold reaches the 1 value.
   * - s_GimpMode: The minimum index of power interval is
   * specified by the first fulfilled abs(percentage[i]-0.006) <
   * fabs(percentage[i+1]-0.006) where the percentage[i] means
   * the amount of the histogram values in the interval [0, i].
   * The maximum index is specified by the first fulfilled
   * (from the end of the histogram) abs(percentage[i]-0.006) <
   * fabs(percentage[i-1]-0.006) where the percentage[i] means
   * the amount of the histogram values in the interval [i, 255].
   *
   * The stretch operation is rejected if the power interval is
   * less than 10 or less than 20 and the percentage[min_index, max_index]
   * / percentage[0, 255] < 0.2.
   *
   */

  bool Stretch(StretchType mode);

  /// Histogram spectrum
  int HistogramData[256];
};


/**
 * MEHistogramTransform
 * @brief The class provides histogram operations
 */
class MEHistogramTransform
{
public:
  /// Types of histogram processing
  typedef enum {
    p_Min = 0,                   /*!< Minimum value */
    p_SeparateChannels = p_Min, /*!< Separate channels */
    p_Average,                   /*!< Average */
    p_Max = p_Average           /*!< Maximum value */
  } ProcessingType;

  /// Types of histogram transformations
  typedef enum {
    t_Min = 0,             /*!< Minimum value */
    t_Continuous = t_Min, /*!< Continuous */
    t_Discrete,            /*!< Discrete */
    t_Max = t_Discrete    /*!< Maximum value */
  } TransformType;

  /// Constructor of class
  MEHistogramTransform();
  /// Destructor of class
  ~MEHistogramTransform();

  /*!
   * @brief Histogram stretching an image
   *
   * @param image Source image to stretch
   *
   * The function stretches the histogram of the given image with
   * default parameters: process the color channels separately
   * and continuously.
   *
   */

  void HistogramStretch(MEImage& image);

  /*!
   * @brief Histogram stretching with specified parameters
   *
   * @param image Source image to stretch
   * @param time_mode Mode of the histogram stretching
   *
   * The function transformations the histogram of the image.
   * There is some different possibilities to make the operation:
   *
   * - t_Continuous: The function always stretches the
   * image at each call of the method.
   * - t_Discrete: A histogram is calculated at the first
   * call of the function and all further images will be
   * stretched by this initial histogram.
   *
   */

  void HistogramStretch(MEImage& image, TransformType time_mode);

  /*!
   * @brief Histogram equalization on an image
   *
   * @param image Source image to equalize
   *
   * The source image is transformed by histogram
   * equalization.
   *
   */

  void HistogramEqualize(MEImage& image);

  /*!
   * @brief Set the process mode of the histogram transformation
   *
   * @param new_channel_mode New mode of processing channels
   * @param new_stretch_mode New mode of histogram stretching
   *
   * The process mode of histogram transformation can be
   * set by this method. Two process modes are available for
   * processing channels:
   *
   * - p_SeparateChannels: The class processes the color channels
   * separately.
   * - p_Average: The color channels are averaged
   * in the histogram operations.
   *
   * Two process modes are usable for histogram stretching:
   * s_OwnMode and s_GimpMode. See Histogram::Stretch()
   * for more details.
   *
   */

  void SetStretchProcessingMode(ProcessingType new_channel_mode, MEHistogram::StretchType new_stretch_mode);

private:
  /// Type of the process of histograms
  ProcessingType ChannelMode;
  /// Stretch mode
  MEHistogram::StretchType StretchMode;
  /// Histograms for red, green and blue color channels
  MEHistogram RedChannel, GreenChannel, BlueChannel;
  /// Histogram for average calculation
  MEHistogram AverageChannel;
  /// Continuous histogram stretch is done already
  bool DiscreteStretchingDone;
};

/** @} */

#endif
