#pragma once

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
    class BgsParams
    {
    public:
      virtual ~BgsParams() {}

      virtual void SetFrameSize(unsigned int width, unsigned int height)
      {
        m_width = width;
        m_height = height;
        m_size = width*height;
      }

      unsigned int &Width() { return m_width; }
      unsigned int &Height() { return m_height; }
      unsigned int &Size() { return m_size; }

    protected:
      unsigned int m_width;
      unsigned int m_height;
      unsigned int m_size;
    };
    }
  }
}
