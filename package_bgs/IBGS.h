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
#include <fstream>
#include <list>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>

#ifndef CV_RGB
  #define  CV_RGB(r, g, b)   cv::Scalar((b), (g), (r), 0)
#endif

namespace bgslibrary
{
  namespace algorithms
  {
    class IBGS
    {
    public:
      void setShowOutput(const bool _showOutput) {
        showOutput = _showOutput;
      }
      cv::Mat apply(const cv::Mat &img_input) {
        setShowOutput(false);
        cv::Mat _img_foreground;
        cv::Mat _img_background;
        process(img_input, _img_foreground, _img_background);
                _img_background.copyTo(img_background);
        return _img_foreground;
      }
      cv::Mat getBackgroundModel() {
        return img_background;
      }
      virtual void process(const cv::Mat &img_input, cv::Mat &img_foreground, cv::Mat &img_background) = 0;
      virtual ~IBGS() {}
      //static IBGS* create(const std::string alg_name);
      //static std::list<std::string> get_algs_name();
    protected:
      bool firstTime = true;
      bool showOutput = true;
      cv::Mat img_background;
      cv::Mat img_foreground;
      std::string config_xml;
      void setup(const std::string _config_xml) {
        config_xml = _config_xml;
        if (!config_xml.empty()) {
          if (!std::ifstream(config_xml))
            saveConfig();
          loadConfig();
        }
      }
      void init(const cv::Mat &img_input, cv::Mat &img_outfg, cv::Mat &img_outbg) {
        assert(img_input.empty() == false);
        //img_outfg = cv::Mat::zeros(img_input.size(), img_input.type());
        //img_outbg = cv::Mat::zeros(img_input.size(), img_input.type());
        img_outfg = cv::Mat::zeros(img_input.size(), CV_8UC1);
        img_outbg = cv::Mat::zeros(img_input.size(), CV_8UC3);
      }
    
    private:
      virtual void saveConfig() = 0;
      virtual void loadConfig() = 0;
    };
  }
}

namespace ibgs = bgslibrary::algorithms;
