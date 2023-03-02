#include "BGModel.h"

using namespace bgslibrary::algorithms::lb;

BGModel::BGModel(int width, int height) : m_width(width), m_height(height)
{
  m_SrcImage = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 3);
  m_BGImage = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 3);
  m_FGImage = cvCreateImage(cvSize(m_width, m_height), IPL_DEPTH_8U, 3);

  cvZero(m_SrcImage);
  cvZero(m_BGImage);
  cvZero(m_FGImage);
}

BGModel::~BGModel()
{
  if (m_SrcImage != NULL) cvReleaseImage(&m_SrcImage);
  if (m_BGImage != NULL) cvReleaseImage(&m_BGImage);
  if (m_FGImage != NULL) cvReleaseImage(&m_FGImage);
}

IplImage* BGModel::GetSrc()
{
  return m_SrcImage;
}

IplImage* BGModel::GetFG()
{
  return m_FGImage;
}

IplImage* BGModel::GetBG()
{
  return m_BGImage;
}

void BGModel::InitModel(IplImage* image)
{
  cvCopy(image, m_SrcImage);
  Init();
  return;
}

void BGModel::UpdateModel(IplImage* image)
{
  cvCopy(image, m_SrcImage);
  Update();
  return;
}
