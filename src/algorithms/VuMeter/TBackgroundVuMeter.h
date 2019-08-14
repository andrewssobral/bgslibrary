#pragma once

#include "TBackground.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace vumeter
    {
      class TBackgroundVuMeter : public TBackground
      {
      public:
        TBackgroundVuMeter(void);
        virtual ~TBackgroundVuMeter(void);

        virtual void Clear(void);
        virtual void Reset(void);

        virtual int UpdateBackground(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask);

        virtual IplImage *CreateTestImg();
        virtual int UpdateTest(IplImage *pSource, IplImage *pBackground, IplImage *pTest, int nX, int nY, int nInd);

        virtual int GetParameterCount(void);
        virtual std::string GetParameterName(int nInd);
        virtual std::string GetParameterValue(int nInd);
        virtual int SetParameterValue(int nInd, std::string csNew);

        inline void SetBinSize(int nNew) { m_nBinSize = (nNew > 0 && nNew < 255) ? nNew : 8; }
        inline double GetBinSize() { return m_nBinSize; }

        inline void SetAlpha(double fNew) { m_fAlpha = (fNew > 0.0 && fNew < 1.0) ? fNew : 0.995; }
        inline double GetAlpha() { return m_fAlpha; }

        inline void SetThreshold(double fNew) { m_fThreshold = (fNew > 0.0 && fNew < 1.0) ? fNew : 0.03; }
        inline double GetThreshold() { return m_fThreshold; }

      protected:
        IplImage **m_pHist;

        int m_nBinCount;
        int m_nBinSize;
        int m_nCount;
        double m_fAlpha;
        double m_fThreshold;

        virtual int Init(IplImage * pSource);
        virtual bool isInitOk(IplImage * pSource, IplImage *pBackground, IplImage *pMotionMask);
      };
    }
  }
}
