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
/****************************************************************************
*
* Error.cpp
*
* Purpose:  Error checking routines.
*
* Author: Donovan Parks, July 2007
*
******************************************************************************/

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
