#include <iostream> 
#include <fstream>

#include "Error.h"

//using namespace bgslibrary::algorithms::dp;

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
      std::ofstream traceFile;

      bool Error(const char* msg, const char* code, int data)
      {
        std::cerr << code << ": " << msg << std::endl;
        return false;
      }

      bool TraceInit(const char* filename)
      {
        traceFile.open(filename);
        return traceFile.is_open();
      }

      void Trace(const char* msg)
      {
        traceFile << msg << std::endl;
      }

      void TraceClose()
      {
        traceFile.close();
      }
    }
  }
}
