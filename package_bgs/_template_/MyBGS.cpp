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
#include "MyBGS.h"

using namespace bgslibrary::algorithms;

MyBGS::MyBGS() {}
MyBGS::~MyBGS() {}

void MyBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if (img_input.empty())
    return;

  if (img_previous.empty())
    img_input.copyTo(img_previous);

  cv::Mat img_foreground;
  cv::absdiff(img_previous, img_input, img_foreground);

  if (img_foreground.channels() == 3)
    cv::cvtColor(img_foreground, img_foreground, CV_BGR2GRAY);

  cv::threshold(img_foreground, img_foreground, 15, 255, cv::THRESH_BINARY);

  img_foreground.copyTo(img_output);
  img_previous.copyTo(img_bgmodel);

  img_input.copyTo(img_previous);
}
