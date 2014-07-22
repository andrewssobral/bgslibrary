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
  BGModel.h
    
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

#ifndef BGMODEL_H
#define BGMODEL_H

#include <opencv2/opencv.hpp>
#include <math.h>
#include <float.h>

#include "Types.h"

namespace lb_library
{
  class BGModel
  {
  public:

    BGModel(int width, int height);
    virtual ~BGModel();

    void InitModel(IplImage* image);
    void UpdateModel(IplImage* image);
  
    virtual void setBGModelParameter(int id, int value) {};

    virtual IplImage* GetSrc();
    virtual IplImage* GetFG();
    virtual IplImage* GetBG();

  protected:
  
    IplImage* m_SrcImage;
    IplImage* m_BGImage;
    IplImage* m_FGImage;

    const int m_width;
    const int m_height;
  
    virtual void Init() = 0;
    virtual void Update() = 0;
  };
}

#endif
