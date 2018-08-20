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
#include "CodeBook.h"

using namespace bgslibrary::algorithms;

CodeBook::CodeBook() :
  t(0), learningFrames(DEFAULT_LEARNFRAMES), alpha(DEFAULT_ALPHA), beta(DEFAULT_BETA)
{
  std::cout << "CodeBook()" << std::endl;
  setup("./config/CodeBook.xml");
}

CodeBook::~CodeBook()
{
  std::cout << "~CodeBook()" << std::endl;
}

void CodeBook::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);
  
  if(firstTime)
  {
    img_foreground = cv::Mat::zeros(img_input.size(), CV_8UC1);
    //img_background = cv::Mat::zeros(img_input.size(), CV_8UC3);

    initializeCodebook(img_input.rows, img_input.cols);
  } 

  cv::Mat img_input_gray;

  if (img_input.channels() == 1)
  {
    img_input_gray = img_input; 
  } else
  { 
    cv::cvtColor(img_input, img_input_gray, CV_BGR2GRAY);
  }

  fg_cb(img_input_gray, img_foreground);

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
  {
    cv::imshow("Codebook FG", img_foreground);
    //cv::imshow("Codebook BG", img_background);
  }
#endif

  img_foreground.copyTo(img_output);
  //img_background.copyTo(img_bgmodel);

  firstTime = false;
}

void CodeBook::initializeCodebook(int w, int h)
{
  cbMain = new std::vector<codeword>*[w];
  for (int i = 0; i < w; ++i)
    cbMain[i] = new std::vector<codeword>[h];

  cbCache = new std::vector<codeword>*[w];
  for (int i = 0; i < w; ++i)
    cbCache[i] = new std::vector<codeword>[h];
}

void CodeBook::update_cb(const cv::Mat& frame)
{
  if (t > learningFrames)
    return;

  for (int i = 0; i < frame.rows; i++)
  {
    for (int j = 0; j < frame.cols; j++)
    {
      int pix = frame.at<uchar>(i, j);
      std::vector<codeword>& cm = cbMain[i][j];
      bool found = false;
      for (int k = 0; k < (int)cm.size(); k++)
      {
        if (cm[k].min <= pix && pix <= cm[k].max && !found)
        {
          found = true;
          cm[k].min = ((pix - alpha) + (cm[k].f*cm[k].min)) / (cm[k].f + 1);
          cm[k].max = ((pix + alpha) + (cm[k].f*cm[k].max)) / (cm[k].f + 1);
          cm[k].l = 0;
          cm[k].last = t;
          cm[k].f++;
        }
        else
        {
          cm[k].l++;
        }
      }
      if (!found)
      {
        codeword n = {};
        n.min = std::max(0, pix - alpha);
        n.max = std::min(255, pix + alpha);
        n.f = 1;
        n.l = 0;
        n.first = t;
        n.last = t;
        cm.push_back(n);
      }
    }
  }
  t++;
}

void CodeBook::fg_cb(const cv::Mat& frame, cv::Mat& fg)
{
  //fg = cv::Mat::zeros(frame.size(), CV_8UC1);
  //if (cbMain == 0) initializeCodebook(frame.rows, frame.cols);
  
  if (t <= learningFrames)
  {
    update_cb(frame);
    return;
  }

  for (int i = 0; i<frame.rows; i++)
  {
    for (int j = 0; j<frame.cols; j++)
    {
      int pix = frame.at<uchar>(i, j);
      std::vector<codeword>& cm = cbMain[i][j];
      bool found = false;
      for (int k = 0; k < (int)cm.size(); k++)
      {
        if (cm[k].min <= pix && pix <= cm[k].max && !found)
        {
          cm[k].min = ((1 - beta)*(pix - alpha)) + (beta*cm[k].min);
          cm[k].max = ((1 - beta)*(pix + alpha)) + (beta*cm[k].max);
          cm[k].l = 0;
          cm[k].first = t;
          cm[k].f++;
          found = true;
        }
        else
          cm[k].l++;
      }
      cm.erase(remove_if(cm.begin(), cm.end(), [](codeword& c) { return c.l >= Tdel; }), cm.end());
      fg.at<uchar>(i, j) = found ? 0 : 255;
      if (found) continue;
      found = false;
      std::vector<codeword>& cc = cbCache[i][j];
      for (int k = 0; k < (int)cc.size(); k++)
      {
        if (cc[k].min <= pix && pix <= cc[k].max && !found)
        {
          cc[k].min = ((1 - beta)*(pix - alpha)) + (beta*cc[k].min);
          cc[k].max = ((1 - beta)*(pix + alpha)) + (beta*cc[k].max);
          cc[k].l = 0;
          cc[k].first = t;
          cc[k].f++;
          found = true;
        }
        else
          cc[k].l++;
      }

      if (!found)
      {
        codeword n = {};
        n.min = std::max(0, pix - alpha);
        n.max = std::min(255, pix + alpha);
        n.f = 1;
        n.l = 0;
        n.first = t;
        n.last = t;
        cc.push_back(n);
      }

      cc.erase(remove_if(cc.begin(), cc.end(), [](codeword& c) { return c.l >= Th; }), cc.end());

      for (std::vector<codeword>::iterator it = cc.begin(); it != cc.end(); it++)
        if (it->f > Tadd)
          cm.push_back(*it);

      cc.erase(remove_if(cc.begin(), cc.end(), [](codeword& c) { return c.f > Tadd; }), cc.end());
    }
  }
}

void CodeBook::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_WRITE);

  cvWriteInt(fs, "alpha", alpha);
  cvWriteReal(fs, "beta", beta);
  cvWriteInt(fs, "learningFrames", learningFrames);
  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void CodeBook::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

  alpha = cvReadIntByName(fs, nullptr, "alpha", DEFAULT_ALPHA);
  beta = cvReadRealByName(fs, nullptr, "beta", DEFAULT_BETA);
  learningFrames = cvReadIntByName(fs, nullptr, "learningFrames", DEFAULT_LEARNFRAMES);
  showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
