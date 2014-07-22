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
#include <string>
#include <opencv2/opencv.hpp>


namespace bgslibrary
{
  class ForegroundMaskAnalysis
  {
  private:
    bool firstTime;
    bool showOutput;

  public:
    ForegroundMaskAnalysis();
    ~ForegroundMaskAnalysis();

    long stopAt;
    std::string img_ref_path;

    void process(const long &frameNumber, const std::string &name, const cv::Mat &img_input);

  private:
    void saveConfig();
    void loadConfig();
  };
}
