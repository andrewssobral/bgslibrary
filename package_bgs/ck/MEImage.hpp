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

#ifndef MEImage_H
#define MEImage_H

/**
 *  @addtogroup mindeye
 *  @{
 */

/**
 * MEImage
 * @brief Basic image functions
 */
class MEImage
{
public:
  /// Types of LBP operator
  typedef enum {
    lbp_Min = 0,          /*!< Minimum value */
    lbp_Normal = lbp_Min, /*!< Normal LBP pattern */
    lbp_Special,          /*!< Special LBP pattern */
    lbp_Max = lbp_Special /*!< Maximum value */
  } LBPType;

  /// Types of image subtraction
  typedef enum {
    sub_Min = 0,          /*!< Minimum value */
    sub_Normal = sub_Min, /*!< Normal */
    sub_Absolut,          /*!< Absolut */
    sub_Max = sub_Absolut /*!< Maximum value */
  } SubtractModeType;

  /// Types of image addition
  typedef enum {
    a_Min = 0,         /*!< Minimum value */
    a_Average = a_Min, /*!< Average */
    a_Union,           /*!< Union */
    a_Max = a_Union    /*!< Maximum value */
  } AdditionType;

  /// Types of image multiplication
  typedef enum {
    m_Min = 0,              /*!< Minimum value */
    m_Normal = m_Min,       /*!< Normal */
    m_Neighbourhood,        /*!< Neighbourhood */
    m_Max = m_Neighbourhood /*!< Maximum value */
  } MultiplicationType;

  /// Types of grayscale conversation
  typedef enum {
    g_Min = 0,         /*!< Minimum value */
    g_Average = g_Min, /*!< Average */
    g_OpenCV,          /*!< OpenCV */
    g_Max = g_OpenCV   /*!< Maximum value */
  } GrayscaleType;

  /// Types of pixel neighbourhoods
  typedef enum {
    n_Min = 0,     /*!< Minimum value */
    n_2x2 = n_Min, /*!< 2x2 */
    n_3x2,         /*!< 3x2 */
    n_3x3,         /*!< 3x3 */
    n_5x5,         /*!< 5x5 */
    n_7x7,         /*!< 7x7 */
    n_Max = n_7x7  /*!< Maximum value */
  } NeighbourhoodType;

  /// Types of special pixels
  typedef enum {
    p_Min = 0,         /*!< Minimum value */
    p_Minimum = p_Min, /*!< Minimum */
    p_Maximum,         /*!< Maximum */
    p_Counter,         /*!< Counter */
    p_Max = p_Counter  /*!< Maximum value */
  } PixelType;

  /// Types of smooth operation
  typedef enum {
    s_Min = 0,       /*!< Minimum value */
    s_Blur = s_Min,  /*!< Blur */
    s_Gaussian,      /*!< Gaussian */
    s_Median,        /*!< Medium */
    s_Max = s_Median /*!< Maximum value */
  } SmoothType;

  /// Types of color space conversions
  typedef enum {
    csc_Min = 0,                  /*!< Minimum value */
    csc_RGBtoXYZCIED65 = csc_Min, /*!< RGB to XYZCIED65 */
    csc_XYZCIED65toRGB,           /*!< XYZCIED65 to RGB */
    csc_RGBtoHSV,                 /*!< RGB to HSV */
    csc_HSVtoRGB,                 /*!< HSV to RGB */
    csc_RGBtoHLS,                 /*!< RGB to HLS */
    csc_HLStoRGB,                 /*!< HLS to RGB */
    csc_RGBtoCIELab,              /*!< RGB to CIELab */
    csc_CIELabtoRGB,              /*!< CIELab to RGB */
    csc_RGBtoCIELuv,              /*!< RGB to CIELuv */
    csc_CIELuvtoRGB,              /*!< CIELuv to RGB */
    csc_RGBtoYUV,                 /*!< RGB to YUV */
    csc_RGBtoYIQ,                 /*!< RGB to YIQ */
    csc_RGBtorgI,                 /*!< RGB to rgI */
    csc_Max = csc_RGBtorgI        /*!< Maximum value */
  } ColorSpaceConvertType;

  /*!
   * @brief Class constructor
   *
   * @param width Image width
   * @param height Image height
   * @param layers Layers
   *
   * Class constructor with the possibility to specify the image width,
   * height and the layers. The default options are 16x16x1.
   *
   */

  MEImage(int width = 16, int height = 16, int layers = 1);

  /*!
   * @brief Class constructor
   *
   * @param other Other image
   *
   * Class constructor with the possibility to specify the image width,
   * height and the layers. The default options are 16x16x1.
   *
   */

  MEImage(const MEImage& other);
  /// Destructor of class
  ~MEImage();

  /*
  -------------------------------------------------------------------
                          Basic functions
  -------------------------------------------------------------------
  */

  /*!
   * @brief Clear image
   *
   * This function clears image by filling all image data with zero
   * value.
   *
   */

  void Clear();

  /*!
   * @brief Get an color layer of image
   *
   * @param new_layer new image of layer
   * @param layernumber number of layer which will be copied
   *
   * Copy an image layer (R, G or B) to @a new_layer image. @a new_layer has to
   * have only one color layer (greyscale). If @a new_layer is not
   * greyscale or it has got different width or height like source image
   * than function reallocates it with appropriate features before
   * copying image data.
   *
   */

  void GetLayer(MEImage& new_layer, int layernumber) const;

  /*!
   * @brief Copy a new color layer to image
   *
   * @param new_layer image data of new color layer
   * @param layernumber number of layer where image data will copy
   *
   * Copy a new image layer from @a new_layer image. @a new_layer has to
   * have only one color layer (greyscale). If @a new_layer is not
   * greyscale or it has got different width or height like source image
   * than function halts with an error message.
   *
   */

  void SetLayer(MEImage& new_layer, int layernumber);

  /*!
   * @brief Copy image data to a pointer
   *
   * @param data pointer where image data will be copied
   *
   * Function in order to acquire image data to an external
   * (unsigned char*) pointer.
   *
   */

  void CopyImageData(unsigned char* data);

  /*!
   * @brief Get a pointer to the internal IplImage
   *
   * @return Pointer to the IplImage
   *
   * This function returns the internal IplImage of the class. The
   * image data can not be modified.
   *
   */

  void* GetIplImage() const;

  /*!
   * @brief Set the internal IplImage
   *
   * @param image Pointer to the IplImage
   *
   * This function sets the internal IplImage of the class.
   *
   */

  void SetIplImage(void* image);

  /*!
   * @brief Handle operator == for MEImage
   *
   * @param image image to check
   *
   * @return true if the images are equal otherwise false.
   *
   * The operator checks the equality of two images.
   *
   */

  bool operator==(const MEImage& image);

  /*!
   * @brief Handle operator != for MEImage
   *
   * @param image image to check
   *
   * @return true if the images are not equal otherwise false.
   *
   * The operator checks the non-equality of two images.
   *
   */

  bool operator!=(const MEImage& image);

  /*!
   * @brief Handle operator = for MEImage
   *
   * @param other_image image to copy operation
   *
   * @return Reference to the actual instance.
   *
   * Copy image data to @a other_image image. Function calls only
   * _Copy() directly.
   *
   */

  MEImage& operator=(const MEImage& other_image);

  /*!
   * @brief Get the width of the image
   *
   * @return Width of the image
   *
   * Get the width of the image.
   *
   */

  int GetWidth() const;

  /*!
   * @brief Get the height of the image
   *
   * @return Height of the image
   *
   * Get the height of the image.
   */

  int GetHeight() const;

  /*!
   * @brief Get the length of a pixel row of the image
   *
   * @return Length of a pixel row
   *
   * Get the row width of the image.
   *
   */

  int GetRowWidth() const;

  /*!
   * @brief Get the number of color layers of the image
   *
   * @return Number of color layer of the image
   *
   * Get the number of color layer of the image.
   *
   */

  int GetLayers() const;

  /*!
   * @brief Get the number of the image pixel data
   *
   * @return Number of the image pixel data
   *
   * Get the number of the image pixel data.
   *
   */

  int GetPixelDataNumber() const;

  /*!
   * @brief Get the image data
   *
   * @return Pointer to the image data
   *
   * Get a pointer to the image.
   *
   */

  unsigned char* GetImageData() const;

  /*!
   * @brief Set the image data
   *
   * @param image_data New image data
   * @param width New image width
   * @param height New image height
   * @param channels New image color channels
   *
   * Get a pointer to the image.
   *
   */

  void SetData(unsigned char* image_data, int width, int height, int channels);

  /*!
   * @brief Get ratio of image width and height
   *
   * @return float ratio of image dimensions
   *
   * Function calculates ratio of image width and height with
   * following equation: ratio = height / width.
   */

  float GetRatio() const;

  /*
  -------------------------------------------------------------------
                       Basic image manipulation
  -------------------------------------------------------------------
  */

  /*!
   * @brief Reallocate image data
   *
   * @param width New width of the image
   * @param height New height of the image
   *
   * Image data will be reallocated with new dimensions @a width
   * and @a height. Number of color channels is not changed.
   *
   */

  void Realloc(int width, int height);

  /*!
   * @brief Reallocate image data
   *
   * @param width New width of the image
   * @param height New height of the image
   * @param layers Number of color channels of the image
   *
   * Image data will be reallocated with new dimensions @a width,
   * @a height and new number of color channels @a layers.
   *
   */

  void Realloc(int width, int height, int layers);

  /*!
   * @brief Resize image
   *
   * @param newwidth new width of image
   * @param newheight new height of image
   *
   * Resize image to @a newwidth width and @a newheight
   * height dimensions.
   *
   */

  void Resize(int newwidth, int newheight);

  /*!
   * @brief Resize image with new width
   *
   * @param newwidth new width of image
   *
   * Image is resized with only new width information therefore
   * fit to original ratio.
   *
   */

  void ResizeScaleX(int newwidth);

  /*!
   * @brief Resize image with new height
   *
   * @param newheight new height of image
   *
   * Image is resized with only new height information therefore
   * fit to original ratio.
   *
   */

  void ResizeScaleY(int newheight);

  /*!
   * @brief Reverse image in horizontal direction
   *
   * Function makes a mirror transformation on image in horizontal
   * direction.
   *
   */

  void MirrorHorizontal();

  /*!
   * @brief Reverse image in vertical direction
   *
   * Function makes a mirror transformation on image in vertical
   * direction.
   *
   */

  void MirrorVertical();

  /*!
   * @brief Crop image
   *
   * @param x1, y1 coordinates of top-left point of rectangle
   * @param x2, y2 coordinates of bottom-right point of rectangle
   *
   * Crop the image in a smaller piece whose dimensions are
   * specified as a rectangle. Top-left and bottom-right
   * coordinates of rectangle are (x1, y1) and (x2, y2) wherefrom
   * comes that the width of the new image is x2-x1 and height is
   * y2-y1.
   *
   */

  void Crop(int x1, int y1, int x2, int y2);

  /*!
   * @brief Copy all image data from an other picture
   *
   * @param x0 x coordinate to paste the new image data
   * @param y0 y coordinate to paste the new image data
   * @param source_image source image
   *
   * Function copies all image data from @a source_image
   * to the given coordinate (x0,y0).
   *
   */

  void CopyImageInside(int x0, int y0, MEImage& source_image);

  /*
  -------------------------------------------------------------------
                     Image processing functions
  -------------------------------------------------------------------
  */

  /*!
   * @brief Erode function
   *
   * @param iterations iterations of erode method
   *
   * Method makes an erode filter on an image @a iterations
   * times with standard 3x3 matrix size.
   *
   */

  void Erode(int iterations);

  /*!
   * @brief Dilate function
   *
   * @param iterations iterations of dilate method
   *
   * Method makes an dilate filter on an image
   * @a iterations times with standard 3x3 matrix size.
   *
   */

  void Dilate(int iterations);

  /*!
   * @brief Smooth function
   *
   * Method smooths with median filter and standard 3x3 matrix size.
   * (Median filter works fine and fast.)
   *
   */

  void Smooth();

  /*!
   * @brief Smooth function with defined parameters
   *
   * @param filtermode type of smooth method
   * @param filtersize the size of the convolution matrix
   *
   * Method smooths with median filter and the given matrix
   * size (@a filtersize x @a filtersize). There are more
   * types of smooth function (@a filtermode):
   *
   * - s_Blur: Blur filter.
   * - s_Gaussian: Gaussian filter.
   * - s_Median: Median filter.
   *
   */

  void SmoothAdvanced(SmoothType filtermode, int filtersize);

  /*!
   * @brief Canny function
   *
   * Canny operator is usable for edge detection. Function makes
   * this operation with standard 3x3 matrix
   * size. Canny has two threshold value which are set to zero
   * in this function by default.
   *
   */

  void Canny();

  /*!
   * @brief Laplace function
   *
   * Laplace operator is usable for edge detection like Canny.
   * This function makes a laplace filter with
   * standard 3x3 matrix size. After calculating destination image will
   * be converted from 16 bit back to 8 bit.
   *
   */

  void Laplace();

  /*!
   * @brief Image quantisation
   *
   * @param levels level of quantisation
   *
   * Quantize an image with @a levels level. It means by 16
   * level color range 0-255 quantizes to 0-15, by 4 level to 0-63 etc.
   *
   */

  void Quantize(int levels);

  /*!
   * @brief Threshold a picture
   *
   * @param threshold_limit limit for threshold
   *
   * Threshold an image with @a threshold_limit limit. Value range
   * of @a threshold_limit is between 0-255. E.g. by value 160 functions
   * will eliminate all color values under 160 with black color
   * (color value zero).
   *
   */

  void Threshold(int threshold_limit);

  /*!
   * @brief Adaptive threshold function
   *
   * This function does adaptive threshold function.
   *
   */

  void AdaptiveThreshold();

  /*!
   * @brief Threshold a picture by a mask image
   *
   * @param mask_image mask image for thresholding
   *
   * Threshold an image with a mask image @a mask_image.
   *
   */

  void ThresholdByMask(MEImage& mask_image);

  /*!
   * @brief Convert an image into a new color space
   *
   * @param transformation Definition of color transformation
   *
   * This function converts an image from a specified color space
   * to an other.
   * Current supported conversions (@a transformation):
   * - csc_RGBtoXYZCIED65: RGB to XYZ (D65 reference light),
   * - csc_XYZCIED65toRGB: XYZ to RGB (D65 reference light),
   * - csc_RGBtoHSV: RGB to HSV,
   * - csc_HSVtoRGB: HSV to RGB,
   * - csc_RGBtoHLS: RGB to HSV,
   * - csc_HLStoRGB: HSV to RGB,
   * - csc_RGBtoCIELab: RGB to CIELab,
   * - csc_CIELabtoRGB: CIELuv to RGB,
   * - csc_RGBtoCIELuv: RGB to CIELuv,
   * - csc_CIELuvtoRGB: CIELuv to RGB,
   * - csc_RGBtoYUV: RGB to YUV color space,
   * - csc_RGBtoYIQ: RGB to YIQ color space.
   *
   */

  void ColorSpace(ColorSpaceConvertType transformation);

  /*!
   * @brief Convert an image to grayscale
   *
   * @param grayscale_mode mode of grayscale conversation
   *
   * The function converts the image to grayscale version
   * (one color channel after the conversion). There is four
   * different ways to convert the image to grayscale what we
   * can define with @a grayscale_mode:
   *
   *  - g_Average: It computes the average grayscale
   * values of the pixels with arithmetical average.
   *  - g_OpenCV: It computes the average grayscale
   * values by help of the values of the Y channel.
   *
   */

  void ConvertToGrayscale(GrayscaleType grayscale_mode = g_OpenCV);

  /*!
   * @brief Convert a grayscale image to RGB
   *
   * The function converts the grayscale image to RGB version.
   * (It copies the info from a single color channel to
   * three color channel.)
   *
   */

  void ConvertGrayscaleToRGB();

  /*!
   * @brief Change the red and blue components of every pixels
   *
   * Function changes the red component with the blue of
   * every pixels. (Simple conversion from RGB->BGR.)
   *
   */

  void ConvertBGRToRGB();

  /*!
   * @brief Compute an LBP filter on the image
   *
   * @param mode The LBP operator type
   *
   * The function converts the image to binary version over the
   * threshold value.
   *
   */

  void LBP(LBPType mode = lbp_Special);

  /*!
   * @brief Binarize an image
   *
   * @param threshold Threshold value
   *
   * The function converts the image to binary version over the
   * threshold value.
   *
   */

  void Binarize(int threshold);

  /*!
   * @brief Subtract an image from the internal picture
   *
   * @param source Source image for subtraction
   * @param mode Calculation mode of difference feature
   *
   * Function generates a difference image between two image:
   * the internal picture of this class and @a source_image.
   * The calculation mode is determined by @a mode parameter.
   * Function supports the following modes:
   *
   *  - sub_Normal: Simple subtraction between each
   * correspondent pixel (per color channels). The result values
   * are converted to absolute value and normalized to
   * range 0-255.
   *
   */

  void Subtract(MEImage& source, SubtractModeType mode);

  /*!
   * @brief Multiple an image with the internal picture
   *
   * @param source Second source image for multiplication
   * @param mode Multiplication mode
   *
   * Function multiples an image with the internal image of this class and
   * the result is stored in the internal image. The implemented calculation
   * modes:
   *
   *  - m_Normal: It multiples the corresponding pixel values
   * of the two images. The original pixel values are divided by 128 and
   * multiplied together. If the result is at least 1 then the new pixel value
   * is 255 otherwise 0.
   *  - m_Neighbourhood: It multiples all pixel values of its
   * 3x3 neighbourhood separately (see the method at MULTIPLICATION_NORMAL)
   * and the new pixel value is 255 if at least two pixel is active in the
   * 3x3 neighbourhood otherwise 0.
   *
   */

  void Multiple(MEImage& source, MultiplicationType mode);

  /*!
   * @brief Addition of an image and the internal picture
   *
   * @param source second source image for addition method
   * @param mode the declaration of the used addition mode
   *
   * Function makes an addition operation between an image and the internal
   * image of this class and the result is stored in the internal image.
   * Supported modes:
   *
   *  - a_Average: It sums the average of the corresponding pixels
   * of each pictures.
   *  - a_Union: It sums the union of the corresponding pixels
   * of each pictures.
   *
   */

  void Addition(MEImage& source, AdditionType mode);

  /*!
   * @brief Eliminate the single pixels from a binary image
   *
   * Function eliminates such a pixels which do not have neighbour pixels with
   * 255 value in a 3x3 neighbourhood. The image should be converted to binary
   * version.
   *
   */

  void EliminateSinglePixels();

  /*!
   * @brief Calculate an area difference feature between two images
   *
   * @param reference Reference image
   * @param difference Difference
   *
   * @return The percentage of image areas representing the conditions
   *
   * Function calculates a similarity feature between two pictures.
   * Counts the number of the pixels whose intensity difference is
   * higher than @a difference. (Range: 0..100)
   *
   */

  float DifferenceAreas(MEImage& reference, int difference) const;

  /*!
   * @brief Calculate an average difference between two images
   *
   * @param reference Reference image
   *
   * @return Average difference of the pixels
   *
   * Function calculates a similarity feature between
   * two images. It returns a simple sum of the absolute difference
   * of each pixel in the two images and averaged by the pixel number.
   * (Range: 0..255)
   *
   */

  int AverageDifference(MEImage& reference) const;

  /*!
   * @brief Calculate minimum of image data
   *
   * @param image Second image
   *
   * Function calculates the minimum of current and given image.
   *
   */

  void Minimum(MEImage& image);

  /*!
   * @brief Calculate average brightness level
   *
   * @return Brightness level in range 0-255.
   *
   * Function calculates the average brightness level of the image.
   *
   */

  float AverageBrightnessLevel() const;

  /*!
   * @brief Check the equalization with a reference image
   *
   * @param reference Reference image
   *
   * @return true in case of binary equalization, otherwise false.
   *
   * Function calculates the binary difference between
   * the image and the reference image.
   *
   */

  bool Equal(const MEImage& reference) const;

  /*!
   * @brief Check the equalization with a reference image
   *
   * @param reference Reference image
   * @param maxabsdiff Maximal absolute difference
   *
   * @return true in case of equalization, otherwise false.
   *
   * Function checks the difference between the image and
   * the reference image. Two pixels are equal in a range of
   * a maximal absolute difference.
   *
   */

  bool Equal(const MEImage& reference, int maxabsdiff) const;

  /*!
   * @brief Get the grayscale value of a pixel
   *
   * @param x X coordinate of the pixel
   * @param y Y coordinate of the pixel
   *
   * @return grayscale value of the pixel
   *
   * The method gives the grayscale value of a pixel back. If
   * the image has 3 color channels (e.g. RGB) then Y value of
   * YIQ/YUV color space will be calculated otherwise normal
   * averaged grayscale value.
   *
   */

  unsigned char GrayscalePixel(int x, int y) const;

  /*!
   * @brief Count the number of neighbourhood pixels with maximum intensity
   *
   * @param startx X coordinate of the top-left pixel
   * @param starty Y coordinate of the top-left pixel
   * @param neighbourhood Specific subset of pixels
   *
   * @return number of the pixels with maximum intensity.
   *
   * The method counts the number of the pixels with maximum
   * intensity (255) in a specified subset of pixels.
   * The grayscale values of the pixels are used in the counter
   * process. The following neighbourhood forms are allowed with
   * the @a neighbourhood parameter:
   *
   *  - n_2X2: Simple 2x2 matrix.
   *  - n_3X3: Simple 3x3 matrix.
   *  - n_3x2: Simple 3x2 matrix.
   *
   */

  int NeighbourhoodCounter(int startx, int starty, NeighbourhoodType neighbourhood) const;

  /*!
   * @brief Calculate the gradient vector in a point
   *
   * @param smooth compute smooth filter
   * @param x X coordinate of the point
   * @param y Y coordinate of the point
   * @param mask_size The mask size to calculate the gradient
   *
   * @param result_x X component of the calculated vector
   * @param result_y Y component of the calculated vector
   *
   * The method calculates the gradient vector in a given point.
   * The image is preprocessed with a Gauss filter to smooth the
   * image content. The filter size of the Gauss filter depends on
   * mask size of the gradient vector: filter size = mask size*3.
   * Eight points are assigned to the initial point to compute
   * a vector sum: (x, y-mask_size), (x+mask_size/√2, y-mask_size/√2),
   * (x+mask_size, y), (x+mask_size/√2, y+mask_size/√2), (x, y+mask_size),
   * (x-mask_size/√2, y+mask_size/√2), (x-mask_size, y), (x-mask_size/√2, y-mask_size/√2).
   * The lengths of all vectors equalize with the mask size.
   * After that each vector is multiplied with the gradient difference between
   * its two end points. The results are summarized and normalized by
   * the mask size.
   *
   */

  void GradientVector(bool smooth, int x, int y, int mask_size, int& result_x, int& result_y);

  /*!
   * @brief Visualize gradient vectors
   *
   * @param vector_x Number of points horizontally
   * @param vector_y Number of points vertically
   *
   * This function draws a wire (@a vector_x * @a vector_y) with
   * gradient vectors.
   *
   */

  void GradientVisualize(int vector_x, int vector_y);

private:

  /*
  -------------------------------------------------------------------
                          Internal methods
  -------------------------------------------------------------------
  */

  /*!
   * @brief Copy image data
   *
   * @param other_image Input image with new image data
   *
   * @return true if it is successful, otherwise false.
   *
   * Copy image data from @a other_image to MEImage image data.
   *
   */

  bool _Copy(const MEImage& other_image);

  /*!
   * @brief Inherent initialization function
   *
   * @param width Width of the image
   * @param height Height of the image
   * @param layer Number of color channels of the image
   *
   * Initialization function of MEImage class which allocates
   * memory to internal MEImage image and sets its properties.
   *
   */

  void _Init(int width, int height, int layer);

  /*!
   * @brief Compute an image to a different color space
   *
   * @param mode Mode of the conversion
   *
   * Currently, the internal function allows to use a few
   * mode to convert an image between color spaces.
   * Current supported conversions (@a mode):
   * - RGBtoYUV: RGB to YUV color space,
   * - RGBtoYIQ: RGB to YIQ color space.
   *
   */

  void ComputeColorSpace(ColorSpaceConvertType mode);

private:
  /// This matrix stores the matrix of the actual color space transform
  float TransformMatrix[3][3];
  /// The OpenCV image which contains the image data
  void* cvImg;
};

/** @} */

#endif
