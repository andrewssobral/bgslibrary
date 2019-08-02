#include <iostream> 
#include <fstream>

#include "Error.h"

using namespace std;

ofstream traceFile;

bool Error(const char* msg, const char* code, int data)
{
  cerr << code << ": " << msg << endl;

  return false;
}

bool TraceInit(const char* filename)
{
  traceFile.open(filename);
  return traceFile.is_open();
}

void Trace(const char* msg)
{
  traceFile << msg << endl;
}

void TraceClose()
{
  traceFile.close();
}
