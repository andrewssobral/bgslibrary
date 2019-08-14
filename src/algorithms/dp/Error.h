#pragma once

namespace bgslibrary
{
  namespace algorithms
  {
    namespace dp
    {
      bool Error(const char* msg, const char* code, int data);
      bool TraceInit(const char* filename);
      void Trace(const char* msg);
      void TraceClose();
    }
  }
}
