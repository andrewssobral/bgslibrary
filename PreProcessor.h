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
#pragma once

#include <iostream>
#include <opencv2/opencv.hpp>

namespace bgslibrary
{
  class PreProcessor
  {
  private:
    bool firstTime;
    bool equalizeHist;
    bool gaussianBlur;
    cv::Mat img_gray;
    bool enableShow;

  public:
    PreProcessor();
    ~PreProcessor();

    void setEqualizeHist(bool value);
    void setGaussianBlur(bool value);
    cv::Mat getGrayScale();

    void process(const cv::Mat &img_input, cv::Mat &img_output);

    void rotate(const cv::Mat &img_input, cv::Mat &img_output, float angle);
    void applyCanny(const cv::Mat &img_input, cv::Mat &img_output);

  private:
    void saveConfig();
    void loadConfig();
  };
}
