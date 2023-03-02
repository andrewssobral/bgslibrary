#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>
// opencv legacy includes
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>

namespace bgslibrary
{
  namespace algorithms
  {
    namespace vumeter
    {
      class TBackground
      {
      public:
        TBackground();
        virtual ~TBackground();

        virtual void Clear();
        virtual void Reset();

        virtual int UpdateBackground(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask);
        virtual int UpdateTest(IplImage *pSource, IplImage *pBackground, IplImage *pTest, int nX, int nY, int nInd);
        virtual IplImage *CreateTestImg();

        virtual int GetParameterCount();
        virtual std::string GetParameterName(int nInd);
        virtual std::string GetParameterValue(int nInd);
        virtual int SetParameterValue(int nInd, std::string csNew);

      protected:
        virtual int Init(IplImage * pSource);
        virtual bool isInitOk(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask);
      };
    }
  }
}
