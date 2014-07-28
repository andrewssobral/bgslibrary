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
/****************************************************************************
*
* Image.h
*
* Purpose:  C++ wrapper for OpenCV IplImage which supports simple and 
*						efficient access to the image data
*
* Author: Donovan Parks, September 2007
*
* Based on code from: 
*  http://www.cs.iit.edu/~agam/cs512/lect-notes/opencv-intro/opencv-intro.html
******************************************************************************/

#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <opencv2/opencv.hpp>
//#include <cxcore.h>

// --- Image Iterator ---------------------------------------------------------

template <class T>
class ImageIterator 
{
public: 
  ImageIterator(IplImage* image, int x=0, int y=0, int dx= 0, int dy=0) :
    i(x), j(y), i0(0) 
  {    
    data = reinterpret_cast<T*>(image->imageData);
    step = image->widthStep / sizeof(T);

    nl= image->height;
    if ((y+dy)>0 && (y+dy) < nl) 
      nl= y+dy;

    if (y<0) 
      j=0;

    data += step*j;

    nc = image->width;
    if ((x+dx) > 0 && (x+dx) < nc) 
      nc = x+dx;

    nc *= image->nChannels;
    if (x>0) 
      i0 = x*image->nChannels;
    i = i0;

    nch = image->nChannels;
  }


  /* has next ? */
  bool operator!() const { return j < nl; }

  /* next pixel */
  ImageIterator& operator++() 
  {
    i++;
    if (i >= nc) 
    { 
      i=i0; 
      j++; 
      data += step; 
    }
    return *this;
  }

  ImageIterator& operator+=(int s) 
  {
    i+=s;
    if (i >= nc) 
    { 
      i=i0; 
      j++; 
      data += step; 
    }
    return *this;
  }

  /* pixel access */
  T& operator*() { return data[i]; }

  const T operator*() const { return data[i]; }

  const T neighbor(int dx, int dy) const
  { 
    return *(data+dy*step+i+dx); 
  }

  T* operator&() const { return data+i; }

  /* current pixel coordinates */
  int column() const { return i/nch; }
  int line() const { return j; }

private:
  int i, i0,j;
  T* data;
  int step;
  int nl, nc;
  int nch;
};

// --- Constants --------------------------------------------------------------

const unsigned char NUM_CHANNELS = 3;

// --- Pixel Types ------------------------------------------------------------

class RgbPixel
{
public:
  RgbPixel() {;}
  RgbPixel(unsigned char _r, unsigned char _g, unsigned char _b)
  {
    ch[0] = _r; ch[1] = _g; ch[2] = _b;
  }

  RgbPixel& operator=(const RgbPixel& rhs)
  {
    ch[0] = rhs.ch[0]; ch[1] = rhs.ch[1]; ch[2] = rhs.ch[2];
    return *this;
  }

  inline unsigned char& operator()(const int _ch)
  {
    return ch[_ch];
  }

  inline unsigned char operator()(const int _ch) const
  {
    return ch[_ch];
  }

  unsigned char ch[3];
};

class RgbPixelFloat
{
public:
  RgbPixelFloat() {;}
  RgbPixelFloat(float _r, float _g, float _b)
  {
    ch[0] = _r; ch[1] = _g; ch[2] = _b;
  }

  RgbPixelFloat& operator=(const RgbPixelFloat& rhs)
  {
    ch[0] = rhs.ch[0]; ch[1] = rhs.ch[1]; ch[2] = rhs.ch[2];
    return *this;
  }

  inline float& operator()(const int _ch)
  {
    return ch[_ch];
  }

  inline float operator()(const int _ch) const
  {
    return ch[_ch];
  }

  float ch[3];
};

// --- Image Types ------------------------------------------------------------

class ImageBase
{
public:
  ImageBase(IplImage* img = NULL) { imgp = img; m_bReleaseMemory = true; }
  ~ImageBase();

  void ReleaseMemory(bool b) { m_bReleaseMemory = b; }

  IplImage* Ptr() { return imgp; }
  const IplImage* Ptr() const { return imgp; }

  void ReleaseImage()
  {
    cvReleaseImage(&imgp);
  }

  void operator=(IplImage* img) 
  { 
    imgp = img;
  }

  // copy-constructor
  ImageBase(const ImageBase& rhs)
  {	
    // it is very inefficent if this copy-constructor is called
    assert(false);
  }

  // assignment operator
  ImageBase& operator=(const ImageBase& rhs)
  {
    // it is very inefficent if operator= is called
    assert(false);

    return *this;
  }

  virtual void Clear() = 0;

protected:
  IplImage* imgp;
  bool m_bReleaseMemory;
};

class RgbImage : public ImageBase
{
public:
  RgbImage(IplImage* img = NULL) : ImageBase(img) { ; }

  virtual void Clear()
  {
    cvZero(imgp);
  }

  void operator=(IplImage* img) 
  { 
    imgp = img;
  }

  // channel-level access using image(row, col, channel)
  inline unsigned char& operator()(const int r, const int c, const int ch)
  {
    return (unsigned char &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels+ch];
  }

  inline const unsigned char& operator()(const int r, const int c, const int ch) const
  {
    return (unsigned char &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels+ch];
  }

  // RGB pixel-level access using image(row, col)
  inline RgbPixel& operator()(const int r, const int c) 
  {
    return (RgbPixel &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels];
  }

  inline const RgbPixel& operator()(const int r, const int c) const
  {
    return (RgbPixel &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels];
  }
};

class RgbImageFloat : public ImageBase
{
public:
  RgbImageFloat(IplImage* img = NULL) : ImageBase(img) { ; }

  virtual void Clear()
  {
    cvZero(imgp);
  }

  void operator=(IplImage* img) 
  { 
    imgp = img;
  }

  // channel-level access using image(row, col, channel)
  inline float& operator()(const int r, const int c, const int ch)
  {
    return (float &)imgp->imageData[r*imgp->widthStep+(c*imgp->nChannels+ch)*sizeof(float)];
  }

  inline float operator()(const int r, const int c, const int ch) const
  {
    return (float)imgp->imageData[r*imgp->widthStep+(c*imgp->nChannels+ch)*sizeof(float)];
  }

  // RGB pixel-level access using image(row, col)
  inline RgbPixelFloat& operator()(const int r, const int c) 
  {
    return (RgbPixelFloat &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels*sizeof(float)];
  }

  inline const RgbPixelFloat& operator()(const int r, const int c) const
  {
    return (RgbPixelFloat &)imgp->imageData[r*imgp->widthStep+c*imgp->nChannels*sizeof(float)];
  }
};

class BwImage : public ImageBase
{
public:
  BwImage(IplImage* img = NULL) : ImageBase(img) { ; }

  virtual void Clear()
  {
    cvZero(imgp);
  }

  void operator=(IplImage* img) 
  { 
    imgp = img;
  }

  // pixel-level access using image(row, col)
  inline unsigned char& operator()(const int r, const int c)
  {
    return (unsigned char &)imgp->imageData[r*imgp->widthStep+c];
  }

  inline unsigned char operator()(const int r, const int c) const
  {
    return (unsigned char)imgp->imageData[r*imgp->widthStep+c];
  }
};

class BwImageFloat : public ImageBase
{
public:
  BwImageFloat(IplImage* img = NULL) : ImageBase(img) { ; }

  virtual void Clear()
  {
    cvZero(imgp);
  }

  void operator=(IplImage* img) 
  { 
    imgp = img;
  }

  // pixel-level access using image(row, col)
  inline float& operator()(const int r, const int c)
  {
    return (float &)imgp->imageData[r*imgp->widthStep+c*sizeof(float)];
  }

  inline float operator()(const int r, const int c) const
  {
    return (float)imgp->imageData[r*imgp->widthStep+c*sizeof(float)];
  }
};

// --- Image Functions --------------------------------------------------------

void DensityFilter(BwImage& image, BwImage& filtered, int minDensity, unsigned char fgValue);

#endif
