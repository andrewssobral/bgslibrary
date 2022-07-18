#pragma once

#include <iostream>
#include <fstream>
#include <list>
#include <memory>
#include <string>
#include <functional>
#include <map>

#include <opencv2/opencv.hpp>

// opencv legacy includes
#include <opencv2/imgproc/types_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#ifndef MEX_COMPILE_FLAG
#include <opencv2/highgui/highgui_c.h>
#endif

#include "../utils/ILoadSaveConfig.h"

#if !defined(bgs_register)
#define bgs_register(x) static BGS_Register<x> register_##x(quote(x))
#endif

namespace bgslibrary
{
  namespace algorithms
  {
    class IBGS : public ILoadSaveConfig
    {
    private:
      friend std::ostream& operator<<(std::ostream& o, const std::shared_ptr<IBGS>& ibgs) {
        return ibgs.get()->dump(o);
      }
      friend std::ostream& operator<<(std::ostream& o, const IBGS *ibgs) {
        return ibgs->dump(o);
      }
      friend std::string to_string(const std::shared_ptr<IBGS>& ibgs) {
        std::ostringstream ss;
        ss << ibgs;
        return ss.str();
      }
    public:
      virtual std::ostream& dump(std::ostream& o) const {
        return o << getAlgorithmName();
      }
      std::string getAlgorithmName() const {
        return algorithmName;
      }
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
      IBGS(const std::string _algorithmName){
        //debug_construction(IBGS);
        algorithmName = _algorithmName;
      }
      IBGS(){
        //debug_construction(IBGS);
        algorithmName = "";
      }
      virtual ~IBGS() {
        //debug_destruction(IBGS);
      }
      virtual void process(const cv::Mat &img_input, cv::Mat &img_foreground, cv::Mat &img_background) = 0;
    protected:
      std::string algorithmName;
      bool firstTime = true;
      bool showOutput = true;
      cv::Mat img_background;
      cv::Mat img_foreground;
      void init(const cv::Mat &img_input, cv::Mat &img_outfg, cv::Mat &img_outbg) {
        assert(img_input.empty() == false);
        //img_outfg = cv::Mat::zeros(img_input.size(), img_input.type());
        //img_outbg = cv::Mat::zeros(img_input.size(), img_input.type());
        img_outfg = cv::Mat::zeros(img_input.size(), CV_8UC1);
        img_outbg = cv::Mat::zeros(img_input.size(), CV_8UC3);
      }
    };
    
    class BGS_Factory
    {
    public:
      BGS_Factory() {
        //debug_construction(BGS_Factory);
      }
      virtual ~BGS_Factory() {
        //debug_destruction(BGS_Factory);
      }
      static BGS_Factory* Instance() {
        static BGS_Factory factory;
        return &factory;
      }

      std::shared_ptr<IBGS> Create(std::string name) {
        IBGS* instance = nullptr;

        // find name in the registry and call factory method.
        auto it = factoryFunctionRegistry.find(name);
        if (it != factoryFunctionRegistry.end())
          instance = it->second();

        // wrap instance in a shared ptr and return
        if (instance != nullptr)
          return std::shared_ptr<IBGS>(instance);
        else
          return nullptr;
      }

      std::vector<std::string> GetRegisteredAlgorithmsName() {
        std::vector<std::string> algorithmsName;
        for (auto it = factoryFunctionRegistry.begin(); it != factoryFunctionRegistry.end(); ++it) {
          algorithmsName.push_back(it->first);
        }
        return algorithmsName;
      }

      void RegisterFactoryFunction(std::string name,
        std::function<IBGS*(void)> classFactoryFunction) {
        // register the class factory function
        factoryFunctionRegistry[name] = classFactoryFunction;
      }
      
    private:
      std::map<std::string, std::function<IBGS*(void)>> factoryFunctionRegistry;
    };

    template<class T>
    class BGS_Register
    {
    public:
      BGS_Register(const std::string className) {
        //debug_construction(BGS_Register);
        // register the class factory function
        BGS_Factory::Instance()->RegisterFactoryFunction(className,
          [](void) -> IBGS* { return new T(); });
      }
      virtual ~BGS_Register() {
        //debug_destruction(BGS_Register);
      }
    };
  }
}
