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
#include <sstream>
#include <vector>
#include <string>

#include "src/bgslibrary_BgsLib.h"
#include "../package_bgs/bgslibrary.h"

bool constructObject(std::string algorithm);
void computeForegroundMask(const cv::Mat &img_input, cv::Mat &img_output);
void destroyObject();

template <typename T>
std::string ToString(T val)
{
  std::stringstream stream;
  stream << val;
  return stream.str();
}

jstring StringToJString(JNIEnv * env, const std::string & nativeString)
{
  return env->NewStringUTF(nativeString.c_str());
}

void GetJStringContent(JNIEnv *AEnv, jstring AStr, std::string &ARes)
{
  if (!AStr)
  {
    ARes.clear();
    return;
  }

  const auto *s = AEnv->GetStringUTFChars(AStr, nullptr);
  ARes = s;
  AEnv->ReleaseStringUTFChars(AStr, s);
}
