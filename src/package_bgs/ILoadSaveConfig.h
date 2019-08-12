#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "../GenericMacros.h"

namespace bgslibrary
{
  class ILoadSaveConfig
  {
  public:
    ILoadSaveConfig(){
      //debug_construction(ILoadSaveConfig);
    }
    virtual ~ILoadSaveConfig() {
      //debug_destruction(ILoadSaveConfig);
    }

  protected:
    std::string config_xml;
    void setup(const std::string _config_xml) {
      config_xml = _config_xml;
      if (!config_xml.empty()) {
        if (!std::ifstream(config_xml))
          saveConfig();
        loadConfig();
      }
    }

  private:
    virtual void saveConfig() = 0;
    virtual void loadConfig() = 0;
  };
}
