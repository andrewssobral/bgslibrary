#pragma once

#include <math.h>
#include <sys/types.h>

#include "../IBGS.h"
#include "../ILoadSaveConfig.h"
#include "amber/amber.h"

namespace bgslibrary
{
  namespace algorithms
  {
    class Amber : public IBGS, public ILoadSaveConfig
    {
    private:
      amberModel* model;

    public:
      Amber();
      ~Amber();

      void process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel);

    private:
      void saveConfig();
      void loadConfig();
    };

    static BGS_Register<Amber> register_Amber("Amber");
  }
}
