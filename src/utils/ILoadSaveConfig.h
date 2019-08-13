#pragma once

#include <iostream>
#include <fstream>
#include <string>

#include "GenericMacros.h"

namespace bgslibrary
{
  const std::string DEFAULT_CONFIG_BASE_PATH = "./config";
  const std::string DEFAULT_CONFIG_EXTENSION = ".xml";
  
  class ILoadSaveConfig
  {
  public:
    ILoadSaveConfig() :
    config_base_path(DEFAULT_CONFIG_BASE_PATH),
    config_extension(DEFAULT_CONFIG_EXTENSION),
    config_file_path("")
    {
      //debug_construction(ILoadSaveConfig);
    }
    virtual ~ILoadSaveConfig() {
      //debug_destruction(ILoadSaveConfig);
    }

  protected:
    std::string config_base_path;
    std::string config_extension;
    std::string config_file_path;
    //static const std::string config_base_path;
    //static const std::string config_extension;
    virtual void save_config(cv::FileStorage &fs) = 0;
    virtual void load_config(cv::FileStorage &fs) = 0;
    void initLoadSaveConfig(const std::string _config_file_name) {
      if(!_config_file_name.empty()) {
        config_file_path = config_base_path + "/" + _config_file_name + config_extension;
        if (!std::ifstream(config_file_path))
          _save_config();
        _load_config();
      }
    }
    
  private:
    void _save_config() {
      //std::cout << "_save_config: " << config_file_path << std::endl;
      cv::FileStorage fs(config_file_path, cv::FileStorage::WRITE);
      if (_is_valid(fs))
        save_config(fs);
      fs.release();
    }
    void _load_config() {
      //std::cout << "_load_config: " << config_file_path << std::endl;
      cv::FileStorage fs;
      fs.open(config_file_path, cv::FileStorage::READ);
      if (_is_valid(fs))
        load_config(fs);
      fs.release();
    }
    bool _is_valid(cv::FileStorage &fs) {
      if (!fs.isOpened()) {
        std::cerr << "Failed to open " << config_file_path << std::endl;
        //std::cerr << "Please check if the path above is valid" << std::endl;
        return false;
      }
      return true;
    }
  };
  //const std::string ILoadSaveConfig::config_base_path = "./config";
  //const std::string ILoadSaveConfig::config_extension = ".xml";
}
