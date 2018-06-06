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

#include "lb/BGModelSom.h"
#include "IBGS.h"

using namespace lb_library;
using namespace lb_library::AdaptiveSOM;

namespace bgslibrary
{
  namespace algorithms
  {
    class LBAdaptiveSOM : public IBGS
    {
    private:
      BGModel* m_pBGModel;
      int sensitivity;
      int trainingSensitivity;
      int learningRate;
      int trainingLearningRate;
      int trainingSteps;

    public:
      LBAdaptiveSOM();
      void flush();
      void setParameters(int sensitivity=75, int trainingSensitivity=245, int learningRate=62, int trainingLearningRate=255, int trainingSteps=55);
      ~LBAdaptiveSOM();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };
  }
}
