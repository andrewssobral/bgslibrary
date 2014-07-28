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
/*
*  TBackground.h
*  Framework
*
*  Created by Robinault Lionel on 07/12/11.
*
*/
#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>


class TBackground
{
public:
  TBackground(void);
  virtual ~TBackground(void);

  virtual void Clear(void);
  virtual void Reset(void);

  virtual int UpdateBackground(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask);
  virtual int UpdateTest(IplImage *pSource, IplImage *pBackground, IplImage *pTest, int nX, int nY, int nInd);
  virtual IplImage *CreateTestImg();

  virtual int GetParameterCount(void);
  virtual std::string GetParameterName(int nInd);
  virtual std::string GetParameterValue(int nInd);
  virtual int SetParameterValue(int nInd, std::string csNew);

protected:
  virtual int Init(IplImage * pSource);
  virtual bool isInitOk(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask);
};
