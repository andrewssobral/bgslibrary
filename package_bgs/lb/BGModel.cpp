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
/*  Scene 1.0.1 -- Background subtraction and object tracking for complex environments  
  BGModel.cpp
    
  Copyright (C) 2011 Laurence Bender <lbender@untref.edu.ar>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "BGModel.h"

namespace lb_library
{
  BGModel::BGModel(int width, int height): m_width(width), m_height(height)
  {
    m_SrcImage = cvCreateImage(cvSize(m_width,m_height), IPL_DEPTH_8U, 3);
    m_BGImage = cvCreateImage(cvSize(m_width,m_height), IPL_DEPTH_8U, 3);
    m_FGImage = cvCreateImage(cvSize(m_width,m_height), IPL_DEPTH_8U, 3);

    cvZero(m_SrcImage);
    cvZero(m_BGImage);
    cvZero(m_FGImage);
  }

  BGModel::~BGModel()
  {
    if (m_SrcImage!=NULL) cvReleaseImage(&m_SrcImage);
    if (m_BGImage!=NULL) cvReleaseImage(&m_BGImage);
    if (m_FGImage!=NULL) cvReleaseImage(&m_FGImage);
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
    cvCopy(image,m_SrcImage);
    Init();
    return;
  }

  void BGModel::UpdateModel(IplImage* image)
  {
    cvCopy(image,m_SrcImage);
    Update();
    return;
  }
}