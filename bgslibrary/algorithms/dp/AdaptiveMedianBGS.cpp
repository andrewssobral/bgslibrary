#include "AdaptiveMedianBGS.h"
#include "opencv2/core/types_c.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

using namespace bgslibrary::algorithms::dp;

void AdaptiveMedianBGS::Initalize(const BgsParams& param)
{
  m_params = (AdaptiveMedianParams&)param;
  m_median = cvCreateImage(cvSize(m_params.Width(), m_params.Height()), IPL_DEPTH_8U, 3);
  cvSet(m_median.Ptr(), cvScalar(CV_RGB(BACKGROUND, BACKGROUND, BACKGROUND)));
}

RgbImage* AdaptiveMedianBGS::Background()
{
  return &m_median;
}

void AdaptiveMedianBGS::InitModel(const RgbImage& data)
{
  // initialize the background model
  for (unsigned int r = 0; r < m_params.Height(); ++r)
  {
    for (unsigned int c = 0; c < m_params.Width(); ++c)
    {
      m_median(r, c) = data(r, c);
    }
  }
}

void AdaptiveMedianBGS::Update(int frame_num, const RgbImage& data, const BwImage& update_mask)
{
  if (frame_num % m_params.SamplingRate() == 1)
  {
    // update background model
    for (unsigned int r = 0; r < m_params.Height(); ++r)
    {
      for (unsigned int c = 0; c < m_params.Width(); ++c)
      {
        // perform conditional updating only if we are passed the learning phase
        if (update_mask(r, c) == BACKGROUND || frame_num < m_params.LearningFrames())
        {
          for (int ch = 0; ch < NUM_CHANNELS; ++ch)
          {
            if (data(r, c, ch) > m_median(r, c, ch))
            {
              m_median(r, c, ch)++;
            }
            else if (data(r, c, ch) < m_median(r, c, ch))
            {
              m_median(r, c, ch)--;
            }
          }
        }
      }
    }
  }
}

void AdaptiveMedianBGS::SubtractPixel(int r, int c, const RgbPixel& pixel,
  unsigned char& low_threshold, unsigned char& high_threshold)
{
  // perform background subtraction
  low_threshold = high_threshold = FOREGROUND;

  int diffR = abs(pixel(0) - m_median(r, c, 0));
  int diffG = abs(pixel(1) - m_median(r, c, 1));
  int diffB = abs(pixel(2) - m_median(r, c, 2));

  if (diffR <= m_params.LowThreshold() && diffG <= m_params.LowThreshold() && diffB <= m_params.LowThreshold())
  {
    low_threshold = BACKGROUND;
  }

  if (diffR <= m_params.HighThreshold() && diffG <= m_params.HighThreshold() && diffB <= m_params.HighThreshold())
  {
    high_threshold = BACKGROUND;
  }
}

///////////////////////////////////////////////////////////////////////////////
//Input:
//  data - a pointer to the image data
//Output:
//  output - a pointer to the data of a gray value image
//					(the memory should already be reserved) 
//					values: 255-foreground, 0-background
///////////////////////////////////////////////////////////////////////////////
void AdaptiveMedianBGS::Subtract(int frame_num, const RgbImage& data,
  BwImage& low_threshold_mask, BwImage& high_threshold_mask)
{
  unsigned char low_threshold, high_threshold;

  // update each pixel of the image
  for (unsigned int r = 0; r < m_params.Height(); ++r)
  {
    for (unsigned int c = 0; c < m_params.Width(); ++c)
    {
      // perform background subtraction
      SubtractPixel(r, c, data(r, c), low_threshold, high_threshold);

      // setup silhouette mask
      low_threshold_mask(r, c) = low_threshold;
      high_threshold_mask(r, c) = high_threshold;
    }
  }
}

#endif
